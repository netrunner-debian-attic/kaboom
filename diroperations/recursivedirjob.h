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

class RecursiveDirJob : public QThread
{
    Q_OBJECT
public:
    virtual ~RecursiveDirJob();

    static RecursiveDirJob* calculateDirSize(const QString & dir);
    static RecursiveDirJob* recursiveRmDir(const QString & dir);
    static RecursiveDirJob* recursiveCpDir(const QString & sourcePath, const QString & destPath,
                                           DirOperations::CopyOptions options = DirOperations::NoOptions);

    void setProgressDialogInterface(DirOperations::ProgressDialogInterface *pd);

    bool hasError() const;
    DirOperations::Exception lastError() const;
    QVariant result() const;

protected:
    enum JobType { CalculateDirSize, CpDir, RmDir };
    RecursiveDirJob(JobType jtype, const QVariantList & arguments);

    virtual void run();

private:
    struct Private;
    Private *const d;
    DirOperations::ProgressDialogInterface *m_pd;

    //these will be called in the main thread.
    Q_PRIVATE_SLOT(m_pd, void setValue(quint64));
    Q_PRIVATE_SLOT(m_pd, void setMaximum(quint64));
    Q_PRIVATE_SLOT(m_pd, void setLabelText(QString));
};

#endif
