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
#include "../kaboomsettings.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStack>
#include <QtCore/QList>
#include <QtCore/QEventLoop>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

#include <cstdio> //for perror()

//for lstat()
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

QString RecursiveDirJob::Error::message() const
{
    switch (m_type) {
        case AccessDenied: return tr("Access was denied to the file or directory \"%1\"").arg(m_info);
        case NoSuchFileOrDirectory: return tr("\"%1\": No such file or directory").arg(m_info);
        case FileOrDirectoryExists: return tr("\"%1\" already exists").arg(m_info);
        case CopyFail: return tr("Could not copy \"%1\"").arg(m_info);
        case MkdirFail: return tr("Could not create directory \"%1\"").arg(m_info);
        case RmFail: return tr("Could not remove \"%1\"").arg(m_info);
        default: return tr("Unknown error");
    }
}

struct RecursiveDirJob::Private
{
    RecursiveDirJob::JobType m_jobType;
    QVariantList m_args;
    QVariant m_result;
    //QList<Error> m_exceptions;
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

#if 0
bool RecursiveDirJob::hasError() const
{
    return (isFinished()) ? (!d->m_exceptions.isEmpty()) : false;
}

QList<RecursiveDirJob::Error> RecursiveDirJob::errors() const
{
    return (isFinished()) ? d->m_exceptions : QList<Error>();
}
#endif

void RecursiveDirJob::slotErrorOccured(RecursiveDirJob::Error e)
{
    //d->m_exceptions.append(e);
    emit errorOccured(e.message());
}

QVariant RecursiveDirJob::result() const
{
    return (isFinished()) ? d->m_result : QVariant();
}

void RecursiveDirJob::setProgressDialogInterface(ProgressDialogInterface *pd)
{
    m_pd = pd;
}

void RecursiveDirJob::synchronousRun(ProgressDialogInterface *pd)
{
    QEventLoop loop;
    if (pd) {
        setProgressDialogInterface(pd);
    }
    //we will start the thread as soon as we are in the event loop, because if the thread
    //finishes too early (before the event loop starts), quit() will not work and we will stay
    //in this event loop forever.
    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(finished()), &loop, SLOT(quit()) );
    loop.exec();
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

    qRegisterMetaType<RecursiveDirJob::Error>();
    connect(&helper, SIGNAL(errorOccured(RecursiveDirJob::Error)),
            this, SLOT(slotErrorOccured(RecursiveDirJob::Error)), Qt::QueuedConnection);

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
}

quint64 RecursiveDirJobHelper::stat_size(const QString & fileName)
{
    struct stat buf;
    if ( ::lstat(QFile::encodeName(fileName), &buf) != 0 ) {
        perror("lstat");
        qWarning() << "lstat failed on" << fileName;
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
    if ( !currentDir.exists() ) {
        emit errorOccured(Error(Error::NoSuchFileOrDirectory, dir));
        return 0;
    }

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
                if ( !currentDir.cd(currentItem.fileName()) ) {
                    emit errorOccured(Error(Error::AccessDenied, currentItem.absoluteFilePath()));
                } else {
                    stack.push(currentList);
                    currentList = currentDir.entryInfoList(dirFilters);
                }
            }

            if ( m_reportProgress && (++refreshCounter % 100 == 0) )
                emit setLabelText( tr("Calculating the size of \"%1\"... %2")
                                    .arg(dir).arg(DirOperations::bytesToString(totalSize)) );

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

void RecursiveDirJobHelper::recursiveCpDir(const QString & sourcePath, const QString & destPath,
                                            RecursiveDirJob::CopyOptions options)
{
    QDir source(sourcePath);
    if ( !source.exists() ) {
        emit errorOccured(Error(Error::NoSuchFileOrDirectory, sourcePath));
        return;
    }

    QDir dest(destPath);
    if ( dest.exists() ) {
        if ( options & RecursiveDirJob::RemoveDestination )
            recursiveRmDir(destPath);
        else if ( !(options & RecursiveDirJob::OverWrite) ) {
            emit errorOccured(Error(Error::FileOrDirectoryExists, destPath));
            return;
        }
    }

    if ( dest.mkdir(dest.absolutePath()) ) {
        QFile::setPermissions(destPath, QFile::permissions(sourcePath));
    }

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
                if ( options & RecursiveDirJob::OverWrite ) {
                    if ( !QFile::remove(dest.absoluteFilePath(currentName)) )
                        emit errorOccured(Error(Error::RmFail, dest.absoluteFilePath(currentName)));
                }
                if ( !QFile::link( DirOperations::relativeSymLinkTarget(source.absoluteFilePath(currentName)),
                                    dest.absoluteFilePath(currentName) ) )
                    emit errorOccured(Error(Error::CopyFail, source.absoluteFilePath(currentName)));
            }
            else if ( currentItem.isDir() )
            {
                bool ok = false;
                QFile::Permissions sourcePermissions = QFile::permissions(source.absoluteFilePath(currentName));

                if ( !(ok = source.cd(currentName)) ) {
                    emit errorOccured(Error(Error::AccessDenied, source.absoluteFilePath(currentName)));
                }
                if ( ok && !dest.cd(currentName) ) {
                    //if the target dir doesn't exist, create it and try again.
                    if ( !dest.mkdir(currentName) ) {
                        emit errorOccured(Error(Error::MkdirFail, dest.absoluteFilePath(currentName)));
                    }

                    //preserve permissions of the directory
                    QFile::setPermissions(dest.absoluteFilePath(currentName), sourcePermissions);

                    if ( !dest.cd(currentName) ) {
                         //quite impossible to happen
                        emit errorOccured(Error(Error::AccessDenied, dest.absoluteFilePath(currentName)));
                        ok = false;
                        source.cdUp(); //revert the state of source, as we are not going to copy this dir.
                    }
                }

                if (ok) {
                    stack.push(currentList);
                    currentList = source.entryInfoList(dirFilters);
                }
            }
            else if ( currentItem.isFile() )
            {
                if ( options & RecursiveDirJob::OverWrite ) {
                    if ( !QFile::remove(dest.absoluteFilePath(currentName)) )
                        emit errorOccured(Error(Error::RmFail, dest.absoluteFilePath(currentName)));
                }
                if ( !internal_copy( source.absoluteFilePath(currentName), dest.absoluteFilePath(currentName),
                                        options & RecursiveDirJob::ReplaceKde4InFiles ) )
                    emit errorOccured(Error(Error::CopyFail, source.absoluteFilePath(currentName)));
            }
            else
            {
                if ( currentItem.exists() ) {
                    qDebug() << "Ignoring special file" << source.absoluteFilePath(currentName);
                } else {
                    //this can happen with filename encoding bugs
                    emit errorOccured(Error(Error::NoSuchFileOrDirectory, source.absoluteFilePath(currentName)));
                }
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
                if ( !currentDir.cd(currentItem.fileName()) ) {
                    emit errorOccured(Error(Error::AccessDenied, currentItem.absoluteFilePath()));
                } else {
                    stack.push(currentList);
                    currentList = currentDir.entryInfoList(dirFilters);
                }
            }
            else
            {
                if ( !currentDir.remove(currentItem.fileName()) )
                    emit errorOccured(Error(Error::RmFail, currentItem.absoluteFilePath()));
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
                emit errorOccured(Error(Error::RmFail, currentDir.absoluteFilePath(tmpname)));

            if ( quit )
                break;
        }
    }
}


struct UpdateEntry {
    enum Type { DirWithSubdirs, File };
    Type type;
    const char *relativePath;
};

static const UpdateEntry updateEntries[] = {
    {UpdateEntry::DirWithSubdirs, "/share/config/"},
};

bool RecursiveDirJobHelper::internal_copy(const QString & sourceFile, const QString & destFile, bool replaceKde4InFiles)
{
    if ( !replaceKde4InFiles ) {
        return QFile::copy(sourceFile, destFile);
    } else {
        for (unsigned int i = 0; i < sizeof(updateEntries) / sizeof(UpdateEntry); i++) {
            UpdateEntry entry = updateEntries[i];
            QString path = KaboomSettings::instance().kdehomeDir().absolutePath() + entry.relativePath;

            if ( (entry.type == UpdateEntry::File && destFile == path) ||
                 (entry.type == UpdateEntry::DirWithSubdirs && destFile.startsWith(path)) )
            {
                qDebug() << "Doing copy with s/.kde4/.kde/ of" << sourceFile;
                return copyWithReplaceKde4(sourceFile, destFile);
            }
        }

        return QFile::copy(sourceFile, destFile);
    }
}

bool RecursiveDirJobHelper::copyWithReplaceKde4(const QString & sourceFileName, const QString & destFileName)
{
    QFile sourceFile(sourceFileName);
    QFile destFile(destFileName);

    if ( !sourceFile.open(QIODevice::ReadOnly) ) {
        qCritical() << "copyWithReplaceKde4:" << "Could not open" << sourceFileName << "for reading";
        return false;
    }

    if ( !destFile.open(QIODevice::WriteOnly) ) {
        qCritical() << "copyWithReplaceKde4:" << "Could not open" << destFileName << "for writing";
        return false;
    }

    while ( !sourceFile.atEnd() ) {
        QByteArray b = sourceFile.readLine();
        if ( b.isEmpty() ) {
            qCritical() << "copyWithReplaceKde4:" << "Could not read from" << sourceFileName;
            return false;
        }
        if ( destFile.write( b.replace("/.kde4/", "/.kde/") ) == -1 ) {
            qCritical() << "copyWithReplaceKde4:" << "Could not write to" << destFileName;
            return false;
        }
    }

    if ( !destFile.flush() ) {
        qCritical() << "copyWithReplaceKde4:" << "Could not flush" << destFileName;
        return false;
    }

    destFile.close();
    sourceFile.close();
    return true;
}
