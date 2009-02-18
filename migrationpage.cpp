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
  start = new QPushButton("Start",q);
}

void MigrationPagePrivate::doMagic()
{
  start->setEnabled(false);
  q->wizard()->setOptions(q->wizard()->options()|QWizard::DisabledBackButtonOnLastPage); //no way back
  q->setTitle("Migration running");
  if(backup)
  {
    if(QFile::exists(QDir::homePath()+KDEDIR))
    {
      try
      {
        DirOperations::recursiveCpDir(QDir::homePath()+KDEDIR,QDir::homePath()+KDE3BACKUPDIR,true,progress);
      }
      catch (DirOperations::Exception e)
      {
        qDebug() << e.what();
        // emit q->error(e) or something
      }
      qDebug() << "doing recursive copy of .kde to .kde3-backup";
    }
  }
  switch(selection)
  {
    case MigrationTool::Migrate:
      qDebug() << "do nothing, let kconf_update do magic";
      break;
    case MigrationTool::Merge:
      DirOperations::mergeDirs(QDir::homePath()+KDE4DIR,
			         QDir::homePath()+KDEDIR,progress);
      qDebug() << "do magic experimental merge";
      break;
    case MigrationTool::Clean:
      qDebug() << "do recursive rm of .kde dir";
      DirOperations::recursiveRmDir(QDir::homePath()+KDEDIR,progress);
      break;
    case MigrationTool::Move:
      DirOperations::recursiveCpDir(QDir::homePath()+KDE4DIR,QDir::homePath()+KDEDIR,true /*force*/, progress);
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
  
  setTitle("Start Migration");

  connect(d->start,SIGNAL(clicked()),d,SLOT(doMagic()));
  
  QVBoxLayout *lay = new QVBoxLayout(this);
  lay->addWidget(text);
  lay->addWidget(d->progress);
  lay->addWidget(d->start);
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
