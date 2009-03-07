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
#ifndef DIROPERATIONS_H
#define DIROPERATIONS_H

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QCoreApplication>

namespace DirOperations {

    class ProgressDialogInterface
    {
    public:
        virtual ~ProgressDialogInterface() {}

        virtual void setLabelText(const QString & text) = 0;
        virtual void setMaximum(quint64 max) = 0;
        virtual void setValue(quint64 value) = 0;
    };

    class Exception
    {
        Q_DECLARE_TR_FUNCTIONS(Exception)
    public:
        enum Type { OperationCanceled, AccessDenied, NoSuchFileOrDirectory,
                    FileOrDirectoryExists, CopyFail, MkdirFail, RmFail, NoError };

        explicit Exception() : m_type(NoError) {}
        explicit Exception(Type type, const QString & extraInfo = QString())
                    : m_type(type), m_info(extraInfo) {}

        Type type() const { return m_type; }
        QString what() const;

    private:
        Type m_type;
        QString m_info;
    };

    enum CopyOption { NoOptions = 0x0, RemoveDestination = 0x1, OverWrite = 0x2 };
    Q_DECLARE_FLAGS(CopyOptions, CopyOption);

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

    /*! Calculates the size of a directory. Works like "du -hs". */
    quint64 calculateDirSize(const QString & dir, ProgressDialogInterface *pd = 0);

    /*! Copies directory \a sourcePath and all of its contents
     * to directory \a destPath . Works like "cp -r".
     * \a options can be the following:
     * \li NoOptions - Nothing special happens
     * \li RemoveDestination - It removes the destination directory before trying to copy.
     * \li OverWrite - If a file exists both in \a sourcePath and in \a destPath, the the file will be overwritten.
     */
    void recursiveCpDir(const QString & sourcePath, const QString & destPath,
                        CopyOptions options = NoOptions, ProgressDialogInterface *pd = 0);

    /*! Deletes directory \a dir and all of its contents. Works like "rm -r". */
    void recursiveRmDir(const QString & dir, ProgressDialogInterface *pd = 0);

} //namespace DirOperations

Q_DECLARE_OPERATORS_FOR_FLAGS(DirOperations::CopyOptions)
Q_DECLARE_METATYPE(DirOperations::CopyOptions)

#endif
