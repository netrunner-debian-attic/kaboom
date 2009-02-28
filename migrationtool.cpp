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
  
  setWindowTitle(tr("Kaboom - Debian KDE Settings Migration Wizard"));
  setOptions(QWizard::NoBackButtonOnStartPage|QWizard::NoCancelButton|options());  
  
  setButtonText(QWizard::BackButton, tr("&Back"));
  setButtonText(QWizard::NextButton, tr("&Next"));
  setButtonText(QWizard::FinishButton,tr("&Finish"));
  
  setPixmap(QWizard::WatermarkPixmap,QPixmap("watermark.png"));
  //setPixmap(QWizard::LogoPixmap,QPixmap("logo.png"));  - we don't use subtitles, so this is never shown.
  setWizardStyle(QWizard::ModernStyle);
  
  setPage(Intro,d->intro);
  setPage(Choice, d->choice);
  setPage(Warning, d->warning); 
  setPage(Migration,d->migration);

  setStartId(MigrationTool::Intro);

}
int MigrationTool::nextId() const
{
    switch(currentId())
    {
      case Intro:
	return Choice;
	break;
      case Choice:
	if(!d->choice->backupSelected() && (d->choice->selected()==Clean||d->choice->selected()==Merge) && QFile::exists(QDir::homePath()+KDEDIR))
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
