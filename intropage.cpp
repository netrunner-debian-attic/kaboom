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

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
  setTitle(tr("Kaboom - MigrationTool"));
  QLabel *text = new QLabel(tr("This wizard will guide you thru the migration of KDE 3 and KDE 4 settings as required"));
  QVBoxLayout *lay = new QVBoxLayout();
  lay->addWidget(text);
  setLayout(lay);
}
