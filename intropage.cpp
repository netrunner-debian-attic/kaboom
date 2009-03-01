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
#include "intropage.h"
#include "kaboomsettings.h"

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
  setTitle(tr("Welcome to the Debian KDE Settings Migration Wizard!"));
  QLabel *text = new QLabel(tr(
    "<p align='justify'>This version of KDE 4 in Debian uses <u>%1</u> "
    "directory (where ~ refers to the path of your Home directory) to "
    "store user settings and data. The wizard has detected that you have "
    "recently upgraded to this version of KDE 4 from KDE 3 and/or previous "
    "Debian KDE 4 release that used <u>%2</u> directory to store user "
    "settings.</p>"

    "<p align='justify'>This wizard will guide you through the process of "
    "backing up, copying, moving or merging your user settings and data which "
    "were created by the old KDE installation. Please note, however, that it "
    "does not migrate configuration files directly. It operates on the "
    "filesystem level and sets up <u>%1</u> directory the way you request "
    "in the next step.</p>"
    
    "<p align='justify'>Once you finish with this wizard, migration of the "
    "configuration files will be performed by KDE 4 applications themselves as "
    "necessary. Therefore, if you are upgrading from KDE 3, it is "
    "<strong>highly recommended</strong> to enable backing up of your old "
    "settings and data in the next step to avoid data loss in hopefully rare "
    "cases when some KDE 4 applications fail to migrate your important user "
    "settings and/or data properly.</p>"
    ).arg(KaboomSettings::instance().kdehomePrettyPath())
     .arg(KaboomSettings::instance().kde4homePrettyPath()), this);
  text->setWordWrap(true);
  QVBoxLayout *lay = new QVBoxLayout();
  lay->addWidget(text);
  setLayout(lay);
}
