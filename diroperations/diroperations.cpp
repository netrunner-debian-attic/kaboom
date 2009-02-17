/*
    Copyright (C) 2007-2009  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "diroperations.h"
#include <QDir>
#include <QFileInfo>
#include <QStack>
#include <climits> //for PATH_MAX

namespace DirOperations {

QString Exception::what() const
{
    switch (m_type) {
        case OperationCanceled: return QObject::tr("User canceled the operation");
        case AccessDenied: return QObject::tr("Access was denied to the file or directory %1").arg(m_info);
        case NoSuchFileOrDirectory: return QObject::tr("%1: No such file or directory").arg(m_info);
        case FileOrDirectoryExists: return QObject::tr("%1 already exists").arg(m_info);
        case CopyFail: return QObject::tr("Could not copy %1").arg(m_info);
        case MkdirFail: return QObject::tr("Could not create directory %1").arg(m_info);
        case RmFail: return QObject::tr("Could not remove %1").arg(m_info);
        default: return QObject::tr("Unknown error");
    }
}


QString relativeSymLinkTarget(const QString & fileName)
{
    char buff[PATH_MAX+1];
    int len = readlink(QFile::encodeName(fileName), buff, PATH_MAX);
    if ( len < 0 )
        return QString();
    buff[len] = '\0';
    return QString(buff);
}


qint64 calculateDirSize(const QString & dir, ProgressDialogInterface *pd)
{
    QDir currentDir(dir);
    if ( !currentDir.exists() )
        throw Exception(Exception::NoSuchFileOrDirectory, dir);

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System | QDir::CaseSensitive;
    QFileInfoList currentList = currentDir.entryInfoList( filters, QDir::DirsLast );
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    qint64 totalSize = 0;
    int refreshCounter = 0;

    while(1){
        if ( !currentList.isEmpty() ){
            currentItem = currentList.takeFirst();

            if ( currentItem.isDir() && !currentItem.isSymLink() ) {
                if ( !currentDir.cd(currentItem.fileName()) )
                    throw Exception(Exception::AccessDenied, currentItem.absoluteFilePath());
                stack.push(currentList);
                currentList = currentDir.entryInfoList( filters, QDir::DirsLast );
            } else {
                totalSize += currentItem.size();
            }

            if ( pd && (++refreshCounter % 100 == 0) ) {
                pd->setLabelText(QObject::tr("Calculating... ") + QString::number(totalSize) + QObject::tr(" bytes"));
                pd->processEvents();
                if (pd->wasCanceled())
                    throw Exception(Exception::OperationCanceled);
            }

        } else { // list is empty
            if ( !stack.isEmpty() ){
                currentList = stack.pop();
                currentDir.cdUp();
            } else
                break;
        }
    }

    return totalSize;
}

void recursiveCpDir(const QString & sourcePath, const QString & destPath, bool force, ProgressDialogInterface *pd)
{
    QDir source(sourcePath);
    if ( !source.exists() )
        throw Exception(Exception::NoSuchFileOrDirectory, sourcePath);

    QDir dest(destPath);
    if ( dest.exists() ) {
        if ( force )
            recursiveRmDir(destPath, pd);
        else
            throw Exception(Exception::FileOrDirectoryExists, destPath);
    }

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System | QDir::CaseSensitive;
    QFileInfoList currentList = source.entryInfoList( filters, QDir::DirsLast );
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    QString currentName;
    qint64 bytesCopied = 0;

    if ( pd ) {
        pd->setMaximum(calculateDirSize(sourcePath, pd));
        pd->setLabelText(QObject::tr("Copying files..."));
    }

    dest.mkdir(dest.absolutePath());

    while(1)
    {
        if ( !currentList.isEmpty() )
        {
            currentItem = currentList.takeFirst();
            currentName = currentItem.fileName();

            if ( currentItem.isSymLink() )
            {
                if ( !QFile::link( relativeSymLinkTarget(source.absoluteFilePath(currentName)),
                                    dest.absoluteFilePath(currentName) ) )
                    throw Exception(Exception::CopyFail, currentItem.absoluteFilePath());
            }
            else if ( currentItem.isDir() )
            {
                if ( !dest.mkdir(currentName) )
                    throw Exception(Exception::MkdirFail, dest.absoluteFilePath(currentName));
                if ( !source.cd(currentName) )
                    throw Exception(Exception::AccessDenied, source.absoluteFilePath(currentName));
                if ( !dest.cd(currentName) )
                    throw Exception(Exception::AccessDenied, dest.absoluteFilePath(currentName)); //quite impossible

                stack.push(currentList);
                currentList = source.entryInfoList( filters, QDir::DirsLast );
            }
            else
            {
                if ( !QFile::copy( source.absoluteFilePath(currentName), dest.absoluteFilePath(currentName) ) )
                    throw Exception(Exception::CopyFail, source.absoluteFilePath(currentName));

                if ( pd ) {
                    bytesCopied += currentItem.size();
                    pd->setValue(bytesCopied);
                }
            }
        }
        else // list is empty
        {
            if ( !stack.isEmpty() )
            {
                currentList = stack.pop();
                source.cdUp();
                dest.cdUp();
            }
            else
                break;
        }

        if ( pd ) {
            pd->processEvents();
            if (pd->wasCanceled())
                throw Exception(Exception::OperationCanceled);
        }
    }
}

void mergeDirs(const QString & sourcePath, const QString & destPath, ProgressDialogInterface *pd)
{
    QDir source(sourcePath);
    if ( !source.exists() )
        throw Exception(Exception::NoSuchFileOrDirectory, sourcePath);

    QDir dest(destPath);
    if ( !dest.exists() ) {
        dest.mkdir(dest.absolutePath());
    }

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System | QDir::CaseSensitive;
    QFileInfoList currentList = source.entryInfoList( filters, QDir::DirsLast );
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    QString currentName;
    qint64 bytesCopied = 0;

    if ( pd ) {
        pd->setMaximum(calculateDirSize(sourcePath, pd));
        pd->setLabelText(QObject::tr("Copying files..."));
    }


    while(1)
    {
        if ( !currentList.isEmpty() )
        {
            currentItem = currentList.takeFirst();
            currentName = currentItem.fileName();

            if ( currentItem.isSymLink() )
            {
               if (dest.absoluteFilePath(currentName).exists())
                   if(!QFile::remove(dest.absoluteFilePath(currentName)))
                         throw Exception(Exception::RmFail,currentName.absoluteFilePath());
                if ( !QFile::link( relativeSymLinkTarget(source.absoluteFilePath(currentName)),
                                    dest.absoluteFilePath(currentName) ) )
                    throw Exception(Exception::CopyFail, currentItem.absoluteFilePath());
            }
            else if ( currentItem.isDir() )
            {
                if ( !dest.cd(currentName) )
                    if ( !dest.mkdir(currentName) )
                        throw Exception(Exception::MkdirFail, dest.absoluteFilePath(currentName));
                if ( !source.cd(currentName) )
                    throw Exception(Exception::AccessDenied, source.absoluteFilePath(currentName));
                if ( !dest.cd(currentName) )
                    throw Exception(Exception::AccessDenied, dest.absoluteFilePath(currentName)); //quite impossible

                stack.push(currentList);
                currentList = source.entryInfoList( filters, QDir::DirsLast );
            }
            else
            {
                if ( QFile::exists( dest.absoluteFilePath(currentName)))
                     if(!QFile::remove(absoluteFilePath(currentName)))
                        throw Exception(Exception::RmFail, dest.absoluteFilePath());
                if ( !QFile::copy( source.absoluteFilePath(currentName), dest.absoluteFilePath(currentName) ) )
                    throw Exception(Exception::CopyFail, source.absoluteFilePath(currentName));

                if ( pd ) {
                    bytesCopied += currentItem.size();
                    pd->setValue(bytesCopied);
                }
            }
        }
        else // list is empty
        {
            if ( !stack.isEmpty() )
            {
                currentList = stack.pop();
                source.cdUp();
                dest.cdUp();
            }
            else
                break;
        }

        if ( pd ) {
            pd->processEvents();
            if (pd->wasCanceled())
                throw Exception(Exception::OperationCanceled);
        }
    }
}

void recursiveRmDir(const QString & dir, ProgressDialogInterface *pd)
{
    QDir currentDir(dir);
    if ( !currentDir.exists() )
        throw Exception(Exception::NoSuchFileOrDirectory, dir);

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System | QDir::CaseSensitive;
    QFileInfoList currentList = currentDir.entryInfoList( filters, QDir::DirsLast );
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    qint64 bytesRemoved = 0;

    if ( pd ) {
        pd->setMaximum(calculateDirSize(dir, pd));
        pd->setLabelText(QObject::tr("Removing files..."));
    }

    while(1)
    {
        if ( !currentList.isEmpty() ){
            currentItem = currentList.takeFirst();

            if ( currentItem.isDir() && !currentItem.isSymLink() )
            {
                if ( !currentDir.cd(currentItem.fileName()) )
                    throw Exception(Exception::AccessDenied, currentItem.absoluteFilePath());
                stack.push(currentList);
                currentList = currentDir.entryInfoList( filters, QDir::DirsLast );
            }
            else
            {
                if ( pd ) {
                    bytesRemoved += currentItem.size();
                    pd->setValue(bytesRemoved);
                }

                if ( !currentDir.remove(currentItem.fileName()) )
                    throw Exception(Exception::RmFail, currentItem.absoluteFilePath());
            }
        }
        else // list is empty
        {
            bool quit = false;
            if ( !stack.isEmpty() )
                currentList = stack.pop();
            else
                quit = true;

            //if quit == true, we remove the original dir itself, now that it is empty for sure...
            QString tmpname = currentDir.dirName();
            currentDir.cdUp();
            if ( !currentDir.rmdir(tmpname) )
                throw Exception(Exception::RmFail, currentDir.absoluteFilePath(tmpname));

            if ( quit )
                break;
        }

        if ( pd ) {
            pd->processEvents();
            if (pd->wasCanceled())
                throw Exception(Exception::OperationCanceled);
        }
    }
}

} //namespace DirOperations
