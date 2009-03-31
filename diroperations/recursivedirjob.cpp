/*
    Copyright (C) 2007-2009  George Kiagiadakis <gkiagia@users.sourceforge.net>
    Copyright (C) 2009 Sune Vuorela <sune@vuorela.dk>

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
#include "recursivedirjob_p.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStack>
#include <QtCore/QDebug>

#include <cstdio> //for perror()

//for lstat()
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace DirOperations;

struct RecursiveDirJob::Private
{
    RecursiveDirJob::JobType m_jobType;
    QVariantList m_args;
    QVariant m_result;
    Exception m_exception;
};

//static
RecursiveDirJob* RecursiveDirJob::calculateDirSize(const QString & dir)
{
    return new RecursiveDirJob(CalculateDirSize, QVariantList() << QVariant(dir));
}

//static
RecursiveDirJob* RecursiveDirJob::recursiveCpDir(const QString & sourcePath,
                                                const QString & destPath, CopyOptions options)
{
    return new RecursiveDirJob(CpDir, QVariantList() << QVariant(sourcePath)
                                << QVariant(destPath) << QVariant::fromValue<CopyOptions>(options));
}

//static
RecursiveDirJob* RecursiveDirJob::recursiveRmDir(const QString & dir)
{
    return new RecursiveDirJob(RmDir, QVariantList() << QVariant(dir));
}

bool RecursiveDirJob::hasError() const
{
    return (isFinished()) ? (d->m_exception.type() != Exception::NoError) : false;
}

Exception RecursiveDirJob::lastError() const
{
    return (isFinished()) ? d->m_exception : Exception();
}

QVariant RecursiveDirJob::result() const
{
    return (isFinished()) ? d->m_result : QVariant();
}

void RecursiveDirJob::setProgressDialogInterface(ProgressDialogInterface *pd)
{
    m_pd = pd;
}

RecursiveDirJob::RecursiveDirJob(JobType jtype, const QVariantList & arguments)
    : QThread(), d(new Private), m_pd(NULL)
{
    d->m_jobType = jtype;
    d->m_args = arguments;
}

RecursiveDirJob::~RecursiveDirJob()
{
    delete d;
}

void RecursiveDirJob::run()
{
    RecursiveDirJobHelper helper(m_pd != NULL); //report progress info if pd != NULL

    connect(&helper, SIGNAL(setValue(quint64)),
            this, SLOT(setValue(quint64)), Qt::QueuedConnection);
    connect(&helper, SIGNAL(setMaximum(quint64)),
            this, SLOT(setMaximum(quint64)), Qt::QueuedConnection);
    connect(&helper, SIGNAL(setLabelText(QString)),
            this, SLOT(setLabelText(QString)), Qt::QueuedConnection);

    try {
        switch( d->m_jobType ) {
            case CalculateDirSize:
                d->m_result = helper.calculateDirSize(d->m_args[0].toString());
                break;
            case CpDir:
                helper.recursiveCpDir(d->m_args[0].toString(), d->m_args[1].toString(),
                                        d->m_args[2].value<CopyOptions>());
                break;
            case RmDir:
                helper.recursiveRmDir(d->m_args[0].toString());
                break;
            default:
                Q_ASSERT(false);
        }
    } catch(const Exception & e) {
        d->m_exception = e;
    }
}

static quint64 stat_size(const QString & fileName)
{
    struct stat buf;
    if ( ::lstat(QFile::encodeName(fileName), &buf) != 0 ) {
        perror("lstat");
        qWarning() << "lstat failed on" << QFile::encodeName(fileName);
        return 0;
    }
    return buf.st_size;
}

//constant defining the QDir filters that are used in the RecursiveDirJobHelper functions
static const QDir::Filters dirFilters = QDir::AllEntries | QDir::NoDotAndDotDot |
                                 QDir::Hidden | QDir::System | QDir::CaseSensitive;

quint64 RecursiveDirJobHelper::calculateDirSize(const QString & dir)
{
    QDir currentDir(dir);
    if ( !currentDir.exists() )
        throw Exception(Exception::NoSuchFileOrDirectory, dir);

    QFileInfoList currentList = currentDir.entryInfoList(dirFilters);
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    quint64 totalSize = 0;
    int refreshCounter = 0;

    if ( m_reportProgress ) {
        //show busy waiting indicator
        emit setMaximum(0);
        emit setValue(0);
    }

    while(1){
        if ( !currentList.isEmpty() ){
            currentItem = currentList.takeFirst();
            totalSize += stat_size(currentItem.absoluteFilePath());

            if ( currentItem.isDir() && !currentItem.isSymLink() ) {
                if ( !currentDir.cd(currentItem.fileName()) )
                    throw Exception(Exception::AccessDenied, currentItem.absoluteFilePath());
                stack.push(currentList);
                currentList = currentDir.entryInfoList(dirFilters);
            }

            if ( m_reportProgress && (++refreshCounter % 100 == 0) )
                emit setLabelText( tr("Calculating the size of \"%1\"... %2")
                                    .arg(dir).arg(bytesToString(totalSize)) );

        } else { // list is empty
            if ( !stack.isEmpty() ){
                currentList = stack.pop();
                currentDir.cdUp();
            } else
                break;
        }
    }

    totalSize += stat_size(dir);
    qDebug() << "calculateDirSize" << dir << totalSize;
    return totalSize;
}

void RecursiveDirJobHelper::recursiveCpDir(const QString & sourcePath, const QString & destPath, CopyOptions options)
{
    QDir source(sourcePath);
    if ( !source.exists() )
        throw Exception(Exception::NoSuchFileOrDirectory, sourcePath);

    QDir dest(destPath);
    if ( dest.exists() ) {
        if ( options & DirOperations::RemoveDestination )
            recursiveRmDir(destPath);
        else if ( !(options & DirOperations::OverWrite) )
            throw Exception(Exception::FileOrDirectoryExists, destPath);
    }

    dest.mkdir(dest.absolutePath());

    QFileInfoList currentList = source.entryInfoList(dirFilters);
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    QString currentName;
    quint64 bytesCopied = 0;

    if ( m_reportProgress ) {
        quint64 dirSize = calculateDirSize(sourcePath);
        emit setLabelText(tr("Copying files from \"%1\" to \"%2\"...").arg(sourcePath).arg(destPath));
        if (dirSize > 0) {
            emit setMaximum(dirSize);
            //the directory special file is already (almost) copied in dest.mkdir() above
            bytesCopied += stat_size(sourcePath);
            emit setValue(bytesCopied);
        } else {
            //no files to be copied, so set the progressbar to 100%
            emit setMaximum(1);
            emit setValue(1);
        }
    }

    while(1)
    {
        if ( !currentList.isEmpty() )
        {
            currentItem = currentList.takeFirst();
            currentName = currentItem.fileName();

            if ( currentItem.isSymLink() )
            {
                if ( options & OverWrite ) {
                    if ( QFile::exists(dest.absoluteFilePath(currentName)) &&
                         !QFile::remove(dest.absoluteFilePath(currentName)) )
                        throw Exception(Exception::RmFail, dest.absoluteFilePath(currentName));
                }
                if ( !QFile::link( relativeSymLinkTarget(source.absoluteFilePath(currentName)),
                                    dest.absoluteFilePath(currentName) ) )
                    throw Exception(Exception::CopyFail, source.absoluteFilePath(currentName));
            }
            else if ( currentItem.isDir() )
            {
                if ( !source.cd(currentName) )
                    throw Exception(Exception::AccessDenied, source.absoluteFilePath(currentName));
                if ( !dest.cd(currentName) ) {
                    //if the target dir doesn't exist, create it and try again.
                    if ( !dest.mkdir(currentName) )
                        throw Exception(Exception::MkdirFail, dest.absoluteFilePath(currentName));
                    if ( !dest.cd(currentName) )
                        throw Exception(Exception::AccessDenied, dest.absoluteFilePath(currentName)); //quite impossible
                }

                stack.push(currentList);
                currentList = source.entryInfoList(dirFilters);
            }
            else if ( currentItem.isFile() )
            {
                if ( options & OverWrite ) {
                    if ( QFile::exists(dest.absoluteFilePath(currentName)) &&
                         !QFile::remove(dest.absoluteFilePath(currentName)) )
                        throw Exception(Exception::RmFail, dest.absoluteFilePath(currentName));
                }
                if ( !QFile::copy( source.absoluteFilePath(currentName), dest.absoluteFilePath(currentName) ) )
                    throw Exception(Exception::CopyFail, source.absoluteFilePath(currentName));
            }
            else
            {
                qDebug() << "Ignoring special file" << source.absoluteFilePath(currentName);
            }

            if ( m_reportProgress ) {
                bytesCopied += stat_size(currentItem.absoluteFilePath());
                emit setValue(bytesCopied);
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
    }
}

void RecursiveDirJobHelper::recursiveRmDir(const QString & dir)
{
    QDir currentDir(dir);
    if ( !currentDir.exists() ) {
        qWarning() << "recursiveRmDir: trying to remove non-existent directory" << dir;
        if (m_reportProgress) {
            //no files to be removed, so set the progressbar to 100%
            emit setMaximum(1);
            emit setValue(1);
        }
        return; // directory gone, no work to do
    }

    QFileInfoList currentList = currentDir.entryInfoList(dirFilters);
    QFileInfo currentItem;
    QStack<QFileInfoList> stack;
    quint64 bytesRemoved = 0;

    if (m_reportProgress) {
        quint64 dirSize = calculateDirSize(dir);
        emit setLabelText(tr("Removing directory \"%1\"...").arg(dir));
        if (dirSize > 0) {
            emit setMaximum(dirSize);
            //start with the size of the directory to be removed.
            //we do this before starting removing files, because on some filesystems
            //(like reiserfs) the directory size is variable and will be smaller
            //when all files have been removed
            bytesRemoved += stat_size(dir);
            emit setValue(bytesRemoved);
        } else {
            //no files to be removed, so set the progressbar to 100%
            emit setMaximum(1);
            emit setValue(1);
        }
    }

    while(1)
    {
        if ( !currentList.isEmpty() ){
            currentItem = currentList.takeFirst();

            if ( m_reportProgress ) {
                bytesRemoved += stat_size(currentItem.absoluteFilePath());
                emit setValue(bytesRemoved);
            }

            if ( currentItem.isDir() && !currentItem.isSymLink() )
            {
                if ( !currentDir.cd(currentItem.fileName()) )
                    throw Exception(Exception::AccessDenied, currentItem.absoluteFilePath());
                stack.push(currentList);
                currentList = currentDir.entryInfoList(dirFilters);
            }
            else
            {
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
    }
}

