/*
    Copyright (C) 2009 George Kiagiadakis <gkiagia@users.sourceforge.net>
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
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <climits> //for PATH_MAX
#include <unistd.h> //for readlink()
#define _FILE_OFFSET_BITS 64
#include <sys/statvfs.h> // for statvfs for calculating free space

namespace DirOperations {

QString bytesToString(quint64 bytes)
{
    if ( bytes > (1<<30) )
        return QCoreApplication::translate("DirOperations", "%L1 GiB")
                                .arg(double(bytes) / (1<<30), 0, 'f', 2);
    else if ( bytes > (1<<20) )
        return QCoreApplication::translate("DirOperations", "%L1 MiB")
                                .arg(double(bytes) / (1<<20), 0, 'f', 2);
    else if ( bytes > (1<<10) )
        return QCoreApplication::translate("DirOperations", "%L1 KiB")
                                .arg(double(bytes) / (1<<10), 0, 'f', 2);
    else
        return QCoreApplication::translate("DirOperations", "%L1 bytes")
                                .arg(bytes);
}

QString relativeSymLinkTarget(const QString & fileName)
{
    char buff[PATH_MAX+1];
    int len = ::readlink(QFile::encodeName(fileName), buff, PATH_MAX);
    if ( len < 0 )
        return QString();
    buff[len] = '\0';
    return QFile::decodeName(buff);
}

quint64 freeDirSpace(const QString & dir)
{
    struct statvfs info;
    int ret = ::statvfs(QFile::encodeName(dir),&info);
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
    int ret = ::statvfs(QFile::encodeName(dir),&info);
    if(ret < 0) {
      qDebug() << "statvfs errors" << ret;
      //error handling
      return -1;
    }
    qDebug() << "totalPartitionSize" << dir << info.f_bsize*info.f_blocks;
    return info.f_frsize*info.f_blocks;

}

} //namespace DirOperations
