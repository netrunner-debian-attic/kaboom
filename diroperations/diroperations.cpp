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
#include "diroperations.h"
#include "recursivedirjob.h"
#include <QtCore/QFile>
#include <QtCore/QEventLoop>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <climits> //for PATH_MAX
#define _FILE_OFFSET_BITS 64
#include <sys/statvfs.h> // for statvfs for calculating free space

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

quint64 freeDirSpace(const QString & dir)
{
    struct statvfs info;
    int ret = statvfs(dir.toLocal8Bit(),&info);
    if(ret < 0) {
      qDebug() << "statvfs errors" << ret;
      //error handling
      return -1;
    }
    qDebug() << "freeDirSpace" << dir << info.f_bsize*info.f_bavail;
    return info.f_bsize*info.f_bavail;

}

quint64 totalPartitionSize(const QString & dir)
{
    struct statvfs info;
    int ret = statvfs(dir.toLocal8Bit(),&info);
    if(ret < 0) {
      qDebug() << "statvfs errors" << ret;
      //error handling
      return -1;
    }
    qDebug() << "totalPartitionSize" << dir << info.f_bsize*info.f_blocks;
    return info.f_frsize*info.f_blocks;

}

quint64 calculateDirSize(const QString & dir, ProgressDialogInterface *pd)
{
    QEventLoop loop;
    RecursiveDirJob *j = RecursiveDirJob::calculateDirSize(dir);
    j->setProgressDialogInterface(pd);
    //we will start the thread as soon as we are in the event loop, because if the thread
    //finishes too early (before the event loop starts), quit() will not work and we will stay
    //in this event loop forever.
    QMetaObject::invokeMethod(j, "start", Qt::QueuedConnection);
    QObject::connect(j, SIGNAL(finished()), &loop, SLOT(quit()) );
    loop.exec();
    j->deleteLater();
    if ( j->hasError() )
        throw j->lastError();
    return qvariant_cast<quint64>(j->result());
}

void recursiveCpDir(const QString & sourcePath, const QString & destPath,
                    CopyOptions options, ProgressDialogInterface *pd)
{
    QEventLoop loop;
    RecursiveDirJob *j = RecursiveDirJob::recursiveCpDir(sourcePath, destPath, options);
    j->setProgressDialogInterface(pd);
    QMetaObject::invokeMethod(j, "start", Qt::QueuedConnection);
    QObject::connect(j, SIGNAL(finished()), &loop, SLOT(quit()) );
    loop.exec();
    j->deleteLater();
    if ( j->hasError() )
        throw j->lastError();
}

void recursiveRmDir(const QString & dir, ProgressDialogInterface *pd)
{
    QEventLoop loop;
    RecursiveDirJob *j = RecursiveDirJob::recursiveRmDir(dir);
    j->setProgressDialogInterface(pd);
    QMetaObject::invokeMethod(j, "start", Qt::QueuedConnection);
    QObject::connect(j, SIGNAL(finished()), &loop, SLOT(quit()) );
    loop.exec();
    j->deleteLater();
    if ( j->hasError() )
        throw j->lastError();
}

} //namespace DirOperations
