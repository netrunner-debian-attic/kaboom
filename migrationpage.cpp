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
#include "kaboomsettings.h"
#include "migrationtool.h"


MigrationPagePrivate::MigrationPagePrivate(MigrationPage* parent)
{
  q=parent;
  complete=false;
  backup=true;
  selection=MigrationTool::Migrate;
  progress=new ProgressWidget(q);
  start = new QPushButton(tr("Start"),q);
  start->setToolTip(tr("Start migration process"));
  start->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  start->setMinimumSize(start->minimumSizeHint().width()+30, start->minimumSizeHint().height()+10);

  error = new QLabel(q);
  error->setWordWrap(true);
  error->setAlignment(Qt::AlignJustify);
  QPalette pal = error->palette();
  pal.setColor(QPalette::Text, Qt::red);
  error->setPalette(pal);

  QLabel *warning = new QLabel(q);
  warning->setText(tr("WARNING: depending on the severity of the errors above, it might not be safe "
                      "to go back and you may need to resolve problems manually!"));
  warning->setWordWrap(true);
  warning->setAlignment(Qt::AlignJustify);

  errorbox = new QGroupBox(tr("The following error(s) occurred during migration process:"),q);
  QBoxLayout *errorboxlayout = new QVBoxLayout(errorbox);
  errorboxlayout->addWidget(error);
  errorboxlayout->addSpacing(5);
  errorboxlayout->addWidget(warning);
  errorbox->hide();
}

void MigrationPagePrivate::doMagic()
{
  start->setEnabled(false);
  errorhandling();
  q->setTitle(tr("Migration is in progress ..."));
  if(backup)
  {
    if(KaboomSettings::instance().kdehomeDir().exists())
    {
      try
      {
        DirOperations::recursiveCpDir(KaboomSettings::instance().kdehomeDir().canonicalPath(),
                                      KaboomSettings::instance().kde3backupDir().path(),
                                      DirOperations::RemoveDestination, progress);
      }
      catch (DirOperations::Exception &e)
      {
        qDebug() << e.what();
        errorhandling(e.what());
        // emit q->error(e) or something
      }
      qDebug() << "doing recursive copy of .kde to .kde3-backup";
    }
  }
  try
  {
    switch(selection)
    {
      case MigrationTool::Migrate:
        progress->setMaximum(1); //fake the progress bar progress.
        progress->setValue(1);
        qDebug() << "do nothing, let kconf_update do magic";
        break;
      case MigrationTool::Merge:
        DirOperations::recursiveCpDir(KaboomSettings::instance().kde4homeDir().canonicalPath(),
                                      KaboomSettings::instance().kdehomeDir().canonicalPath(),
                                      DirOperations::OverWrite,progress);
        qDebug() << "do magic experimental merge";
        break;
      case MigrationTool::Clean:
        qDebug() << "do recursive rm of .kde dir if exists";
        DirOperations::recursiveRmDir(KaboomSettings::instance().kdehomeDir().path(), progress);
        break;
      case MigrationTool::Move:
        DirOperations::recursiveCpDir(KaboomSettings::instance().kde4homeDir().canonicalPath(),
                                      KaboomSettings::instance().kdehomeDir().path(),
                                      DirOperations::RemoveDestination, progress);
        qDebug() << "move .kde4 over .kde";
        break;
    }
  }
  catch(DirOperations::Exception & e)
  {
    errorhandling(e.what());
  }
  if (error->text().isEmpty()) // No error
  {
    complete=true;
    q->wizard()->setOptions(q->wizard()->options()|QWizard::DisabledBackButtonOnLastPage); //no way back
    emit q->completeChanged();
  }
}
void MigrationPagePrivate::errorhandling(const QString& err)
{
  q->wizard()->button(QWizard::CancelButton)->setEnabled(!err.isEmpty());
  static_cast<MigrationTool*>(q->wizard())->setMigrationError(err);
  error->setText(err);
  errorbox->setVisible(!err.isEmpty());
}

MigrationPage::MigrationPage(QWidget *parent) : QWizardPage(parent)
{
  d=new MigrationPagePrivate(this);
  QLabel *text = new QLabel(tr("Please click \"Start\" button to proceed."),this);
  text->setWordWrap(true);
  
  setTitle(tr("Ready to start migration process"));

  connect(d->start,SIGNAL(clicked()),d,SLOT(doMagic()));
  
  QVBoxLayout *lay = new QVBoxLayout(this);
  lay->addWidget(text);
  lay->addWidget(d->progress);
  lay->addWidget(d->start);
  lay->addWidget(d->errorbox);
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
  d->start->setEnabled(true);

  // Initialize error handling and reenable Cancel button
  d->errorhandling();
  d->q->wizard()->button(QWizard::CancelButton)->setEnabled(true);

  if(field("backup").toBool())
  {
      d->backup=true;
      qDebug() << "BACKUP";
  }
  else
  {
      d->backup=false;
      qDebug() << "NOBACKUP";
  }
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
