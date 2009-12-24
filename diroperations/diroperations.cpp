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
#include <QtCore/QVarLengthArray>
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
    const QByteArray encodedFileName = QFile::encodeName(fileName);
#ifdef PATH_MAX
    char buff[PATH_MAX+1];
    int buff_size = sizeof(buff);
#else
    int path_max = pathconf(encodedFileName.data(), _PC_PATH_MAX);
    if (path_max <= 0) {
        path_max = 4096;
    }
    QVarLengthArray<char, 4096> varbuff(path_max);
    char *buff = varbuff.data();
    int buff_size = varbuff.size();
#endif
    int len = ::readlink(encodedFileName.data(), buff, buff_size - 1);
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
    quint64 result = quint64(info.f_bsize)*quint64(info.f_bavail);
    qDebug() << "freeDirSpace" << dir << result;
    return result;

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
    quint64 result = quint64(info.f_bsize)*quint64(info.f_blocks);
    qDebug() << "totalPartitionSize" << dir << result;
    return result;

}

} //namespace DirOperations
