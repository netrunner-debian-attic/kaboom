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
#include "migrationtool_p.h"


MigrationToolPrivate::MigrationToolPrivate(MigrationTool* parent ) : QObject(parent)
{
  q=parent;
};

MigrationTool::MigrationTool(QWidget *parent) : QWizard(parent)
{
  d=new MigrationToolPrivate(this);
  d->intro = new IntroPage(this);
  d->choice = new ChoicePage(this);
  d->warning = new WarningPage(this);
  d->migration = new MigrationPage(this);
  
  setOptions(QWizard::NoBackButtonOnStartPage|QWizard::NoCancelButton|options());  
  
  setPage(Intro,d->intro);
  setPage(Choice, d->choice);
  setPage(Warning, d->warning); 
  setPage(Migration,d->migration);

  setStartId(MigrationTool::Intro);

}
int MigrationTool::nextId() const
{
    qDebug() << "nextId()";
    switch(currentId())
    {
      case Intro:
	return Choice;
	break;
      case Choice:
	if(!d->choice->backupSelected() && d->choice->selected()==Clean)
	{
	    return Warning;
	}
	return Migration;
	break;
      case Warning:
	return Migration;
      case Migration:
	return -1;
	break;
      default:
	abort();
	break;
    }
}
