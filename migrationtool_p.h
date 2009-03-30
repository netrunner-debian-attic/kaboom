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
#include "migrationtool.h"

#ifndef MIGRATIONTOOL_P_H
#define MIGRATIONTOOL_P_H
#include "intropage.h"
#include "choicepage.h"
#include "warningpage.h"
#include "migrationpage.h"
class MigrationToolPrivate: public QObject
{
  Q_OBJECT

  public:
    MigrationToolPrivate(MigrationTool *parent);
    MigrationTool *q;
    IntroPage *intro;
    ChoicePage *choice;
    WarningPage *warning;
    MigrationPage *migration;
    QString migrationError;
};


#endif // MIGRATIONTOOL_P_H
