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
#ifndef DIROPERATIONS_H
#define DIROPERATIONS_H

#include <QtCore/QString>

namespace DirOperations {

    /*! Returns a string that describes the number of bytes given in a human-friendly way.
     * (i.e. suffixed with KiB, MiB, GiB etc...)
     */
    QString bytesToString(quint64 bytes);

    /*! Returns the target path of the symbolic link \a fileName .
     * The path returned is relative to the symlink. For example if
     * we have the following link:
     * \code
     * lrwxrwxrwx 1 gkiagia gkiagia  10 2006-09-11 15:43 c: -> ../drive_c
     * \endcode then this function will return "../drive_c".
     */
    QString relativeSymLinkTarget(const QString & fileName);

    /*! Calculates the free space in the partition of a dir */
    quint64 freeDirSpace(const QString & dir);

    /*! Calculates the total space of a partition */
    quint64 totalPartitionSize(const QString & dir);

} //namespace DirOperations

#endif
