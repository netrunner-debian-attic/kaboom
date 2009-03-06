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
#ifndef RECURSIVEDIRJOB_P_H
#define RECURSIVEDIRJOB_P_H

#include "recursivedirjob.h"

class RecursiveDirJobHelper : public QObject
{
    Q_OBJECT
public:
    RecursiveDirJobHelper(bool reportProgress, QObject *parent = 0)
        : QObject(parent), m_reportProgress(reportProgress) {}

    quint64 calculateDirSize(const QString & dir);
    void recursiveCpDir(const QString & sourcePath, const QString & destPath,
                        DirOperations::CopyOptions options);
    void recursiveRmDir(const QString & dir);

signals:
    void setValue(quint64 value);
    void setMaximum(quint64 maxValue);
    void setLabelText(QString text);

private:
    bool m_reportProgress;
};

#endif
