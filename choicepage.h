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
#ifndef CHOICEPAGE_H
#define CHOICEPAGE_H
#include <QtGui>
#include "migrationtool.h"

class ChoicePagePrivate;
class ChoicePage : public QWizardPage
{
  Q_OBJECT
  public:
    ChoicePage(QWidget *parent=0);
    bool backupSelected() const;
    MigrationTool::Selection selected() const;
    void initializePage();
  private slots:
    void checkSpaceForBackup();
  private:
    enum SpaceWidget {
        SpaceChecking = 0,
        SpaceLack = 1,
        SpaceOk = 2,
    };
    ChoicePagePrivate *d;

    void showSpaceWidget(SpaceWidget widgetIndex);
};


#endif // CHOICEPAGE_H
