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
#ifndef DIROPERATIONS_H
#define DIROPERATIONS_H

#include <QString>

namespace DirOperations {

    class ProgressDialogInterface
    {
    public:
        virtual ~ProgressDialogInterface() {}

        virtual void setLabelText(const QString & text) = 0;
        virtual bool wasCanceled() const = 0;
        virtual void setMaximum(int max) = 0;
        virtual void setValue(int value) = 0;
        virtual void processEvents() = 0;
    };

    class Exception
    {
    public:
        enum Type { OperationCanceled, AccessDenied, NoSuchFileOrDirectory,
                    FileOrDirectoryExists, CopyFail, MkdirFail, RmFail };

        explicit Exception(Type type, const QString & extraInfo = QString())
                    : m_type(type), m_info(extraInfo) {}

        Type exceptionType() const { return m_type; }
        QString what() const;

    private:
        Type m_type;
        QString m_info;
    };

    /*! Returns the target path of the symbolic link \a fileName .
     * The path returned is relative to the symlink. For example if
     * we have the following link:
     * \code
     * lrwxrwxrwx 1 gkiagia gkiagia  10 2006-09-11 15:43 c: -> ../drive_c
     * \endcode then this function will return "../drive_c".
     */
    QString relativeSymLinkTarget(const QString & fileName);

    /*! Calculates the size of a directory. Works like "du -hs". */
    qint64 calculateDirSize(const QString & dir, ProgressDialogInterface *pd = 0);

    /*! Copies directory \a sourcePath and all of its contents
     * to directory \a destPath . Works like "cp -r".
     */
    void recursiveCpDir(const QString & sourcePath, const QString & destPath,
                        bool force = false, ProgressDialogInterface *pd = 0);

    /*! Deletes directory \a dir and all of its contents. Works like "rm -r". */
    void recursiveRmDir(const QString & dir, ProgressDialogInterface *pd = 0);

} //namespace DirOperations

#endif
