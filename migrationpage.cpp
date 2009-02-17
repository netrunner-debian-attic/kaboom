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
#include "migrationpage.h"
#include "choicepage.h"
#include "migrationpage_p.h"
#include "diroperations/diroperations.h"


MigrationPagePrivate::MigrationPagePrivate(MigrationPage* parent)
{
  q=parent;
  complete=false;
  backup=true;
  selection=MigrationTool::Migrate;
  progress=new ProgressWidget(q);
}

void MigrationPagePrivate::doMagic()
{
  q->setTitle("Migration running");
  if(backup)
  {
    qDebug() << "doing recursive copy of .kde to .kde3-backup";
  }
  switch(selection)
  {
    case MigrationTool::Migrate:
      qDebug() << "do nothing, let kconf_update do magic";
      break;
    case MigrationTool::Merge:
      qDebug() << "do magic experimental merge";
      break;
    case MigrationTool::Clean:
      qDebug() << "do recursive rm of .kde dir";
      break;
    case MigrationTool::Move:
      qDebug() << "move .kde4 over .kde";
      break;
  }
  complete=true;
  
  emit q->completeChanged();
}

MigrationPage::MigrationPage(QWidget *parent) : QWizardPage(parent)
{
  d=new MigrationPagePrivate(this);
  QLabel *text = new QLabel("When you click \"start\", migration will start",this);
  QPushButton *start = new QPushButton("Start",this);
  
  setTitle("Start Migration");

  connect(start,SIGNAL(clicked()),d,SLOT(doMagic()));
  
  QVBoxLayout *lay = new QVBoxLayout(this);
  lay->addWidget(text);
  lay->addWidget(d->progress);
  lay->addWidget(start);
  setLayout(lay);
  
}

void MigrationPage::setMigrationType(MigrationTool::Selection selection)
{
  d->selection = selection;
}

void MigrationPage::setBackup(bool b)
{
  d->backup = b;
}

bool MigrationPage::isComplete() const
{
  return d->complete;
}

void MigrationPage::initializePage()
{
  d->complete=false;
  emit completeChanged();
  if(field("backup").toBool())
  {
      qDebug() << "BACKUP";
  }
  else
      qDebug() << "NOBACKUP";
  QWizardPage* page = wizard()->page(MigrationTool::Choice);
  ChoicePage* choice = static_cast<ChoicePage*>(page);
  if(choice)
  {
    d->selection=choice->selected();
  }
  else
  {
    qFatal("Cast failed");
  }
  
}
