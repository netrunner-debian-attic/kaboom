/*
    Copyright (C) 2009  George Kiagiadakis <gkiagia@users.sourceforge.net>

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
#ifndef RECURSIVEDIRJOB_H
#define RECURSIVEDIRJOB_H

#include "diroperations.h"
#include <QtCore/QThread>
#include <QtCore/QVariant>
#include <QtCore/QMetaType>
#include <QtCore/QCoreApplication> //for Q_DECLARE_TR_FUNCTIONS

class RecursiveDirJob : public QThread
{
    Q_OBJECT
public:

    class ProgressDialogInterface
    {
    public:
        virtual ~ProgressDialogInterface() {}

        virtual void setLabelText(const QString & text) = 0;
        virtual void setMaximum(quint64 max) = 0;
        virtual void setValue(quint64 value) = 0;
    };

    class Error
    {
        Q_DECLARE_TR_FUNCTIONS(Error)
    public:
        enum Type { OperationCanceled, AccessDenied, NoSuchFileOrDirectory,
                    FileOrDirectoryExists, CopyFail, MkdirFail, RmFail, NoError };

        explicit Error() : m_type(NoError) {}
        explicit Error(Type type, const QString & extraInfo = QString())
                    : m_type(type), m_info(extraInfo) {}

        Type type() const { return m_type; }
        QString message() const;

    private:
        Type m_type;
        QString m_info;
    };

    enum CopyOption { NoOptions = 0x0, RemoveDestination = 0x1, OverWrite = 0x2, ReplaceKde4InFiles = 0x4 };
    Q_DECLARE_FLAGS(CopyOptions, CopyOption);

    virtual ~RecursiveDirJob();

    /*! Calculates the size of a directory. Works like "du -hs". */
    static RecursiveDirJob* calculateDirSize(const QString & dir);

    /*! Deletes directory \a dir and all of its contents. Works like "rm -r". */
    static RecursiveDirJob* recursiveRmDir(const QString & dir);

    /*! Copies directory \a sourcePath and all of its contents
     * to directory \a destPath . Works like "cp -r".
     * \a options can be the following:
     * \li NoOptions - Nothing special happens
     * \li RemoveDestination - It removes the destination directory before trying to copy.
     * \li OverWrite - If a file exists both in \a sourcePath and in \a destPath, the the file will be overwritten.
     */
    static RecursiveDirJob* recursiveCpDir(const QString & sourcePath, const QString & destPath,
                                           CopyOptions options = NoOptions);

    void setProgressDialogInterface(ProgressDialogInterface *pd);

    bool hasError() const;
    QList<Error> errors() const;
    QVariant result() const;

    void synchronousRun(ProgressDialogInterface *pd = NULL);

signals:
    void errorOccured(const QString & errorMessage);

protected:
    enum JobType { CalculateDirSize, CpDir, RmDir };
    RecursiveDirJob(JobType jtype, const QVariantList & arguments);

    virtual void run();

private:
    struct Private;
    Private *const d;
    ProgressDialogInterface *m_pd;

    //these will be called in the main thread.
    Q_PRIVATE_SLOT(m_pd, void setValue(quint64));
    Q_PRIVATE_SLOT(m_pd, void setMaximum(quint64));
    Q_PRIVATE_SLOT(m_pd, void setLabelText(QString));

private slots:
    void slotErrorOccured(RecursiveDirJob::Error e);
};

Q_DECLARE_METATYPE(RecursiveDirJob::Error)
Q_DECLARE_OPERATORS_FOR_FLAGS(RecursiveDirJob::CopyOptions)
Q_DECLARE_METATYPE(RecursiveDirJob::CopyOptions)

#endif
