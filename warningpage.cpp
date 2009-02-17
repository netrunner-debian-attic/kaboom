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
#include "warningpage.h"

WarningPage::WarningPage(QWidget *parent) : QWizardPage(parent)
{
  QLabel *text = new QLabel("You have selected to start from a clean KDE and to not backup data. You will most likely use data here. Are you sure?");
  QHBoxLayout *lay = new QHBoxLayout();
  lay->addWidget(text);
  setLayout(lay);
}