/*
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
#ifndef MIGRATIONPAGE_P_H
#define MIGRATIONPAGE_P_H

#include <QtCore>
#include "migrationpage.h"
#include "migrationtool.h"
#include "diroperations/progresswidget.h"

class MigrationPagePrivate : public QObject
{
  Q_OBJECT
  public:
    MigrationPagePrivate(MigrationPage *parent);
    MigrationTool::Selection selection;
    ProgressWidget *progress;
    QPushButton *start;
    MigrationPage *q;
    QLabel *error;
    QGroupBox *errorbox;
    bool backup;
    bool complete;
    void errorhandling(const QString &s = QString::null);
  public slots:
    void doMagic();
};

#endif // MigrationPagePrivate
