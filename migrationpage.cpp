/*
    Copyright (C) 2009 Sune Vuorela <sune@vuorela.dk>
                  2009 Modestas Vainius <modestas@vainius.eu>
                  2009 George Kiagiadakis <gkiagia@users.sourceforge.net>

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
#include "diroperations/recursivedirjob.h"
#include "kaboomsettings.h"
#include "migrationtool.h"


MigrationPagePrivate::MigrationPagePrivate(MigrationPage* parent)
{
  q=parent;
  complete=false;
  backup=true;
  selection=MigrationTool::Migrate;

  scenario = new QLabel;
  text = new QLabel;
  text->setWordWrap(true);

  progress = new ProgressWidget;
  start = new QPushButton(tr("Start"));
  start->setToolTip(tr("Start migration process"));
  start->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  start->setMinimumSize(start->minimumSizeHint().width()+30, start->minimumSizeHint().height()+10);

  error = new QTextBrowser;
  error->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
  QPalette pal = error->palette();
  pal.setColor(QPalette::Text, Qt::red);
  error->setPalette(pal);

  QLabel *warning = new QLabel;
  warning->setText(tr("WARNING: if you go back, the migration process will be restarted over the current state."));
  warning->setWordWrap(true);
  warning->setAlignment(Qt::AlignJustify);

  // Errorbox and its layout
  errorbox = new QGroupBox(tr("The following error(s) occurred during migration process:"));
  QBoxLayout *errorboxlayout = new QVBoxLayout;
  errorboxlayout->addWidget(error);
  errorboxlayout->addSpacing(5);
  errorboxlayout->addWidget(warning);
  errorbox->setLayout(errorboxlayout);

  // Main layout
  QVBoxLayout *lay = new QVBoxLayout;
  lay->addWidget(scenario);
  lay->addSpacing(8);
  lay->addWidget(text);
  lay->addWidget(start);
  lay->addWidget(progress);
  lay->addWidget(errorbox);
  q->setLayout(lay);
}

void MigrationPagePrivate::setPageState(PageState s)
{
  switch(s) {
      case Initial:
        //setup default widget visibility
        text->show();
        progress->hide();
        errorhandling(); // hide the errorbox & init error handling
        if (haveSomethingToDo()) {
            q->setTitle(tr("Ready to start migration process"));
            text->setText(tr("Please click \"Start\" button to proceed."));
            complete=false;
            emit q->completeChanged();
            start->show();
        } else {
            q->setTitle(tr("Migration - nothing to do"));
            text->setText(tr("According to the migration options you selected, nothing needs to be done."));
            complete = true;
            emit q->completeChanged();
            start->hide();
        }
        // reenable Cancel button (disabled by errorhandling())
        q->wizard()->button(QWizard::CancelButton)->setEnabled(true);
        break;
      case InProgress:
        //previous state was Initial
        text->hide();
        start->hide();
        progress->show();
        progress->reset();
        errorhandling(); // hide errorbox & disable cancel button
        q->wizard()->setOptions(q->wizard()->options()|QWizard::DisabledBackButtonOnLastPage); //don't go back during migration.
        q->setTitle(tr("Migration is in progress..."));
        break;
      case FinishedSuccessfully:
      case FinishedWithError:
        //previous state was InProgress
        text->show();
        progress->hide();
        q->setTitle(tr("Migration finished"));

        if ( s == FinishedWithError ) {
            text->setText(tr("Settings migration completed with errors."));
            //allow people going back now.>
            q->wizard()->setOptions(q->wizard()->options() & ~QWizard::DisabledBackButtonOnLastPage);
        } else {
            text->setText(tr("Settings migration completed successfully."));
        }
        complete=true;
        emit q->completeChanged();
        break;
      default:
        Q_ASSERT(false);
  }
}

bool MigrationPagePrivate::haveSomethingToDo()
{
  if ((selection == MigrationTool::Migrate && !backup) ||
      (selection == MigrationTool::Clean &&
       !KaboomSettings::instance().kdehomeDir().exists()))
      return false;
  else
      return true;
}

void MigrationPagePrivate::doMagic()
{
  setPageState(InProgress);

  if(backup)
  {
    if(KaboomSettings::instance().kdehomeDir().exists())
    {
      qDebug() << "doing recursive copy of .kde to .kde3-backup";
      RecursiveDirJob *job = RecursiveDirJob::recursiveCpDir(KaboomSettings::instance().kdehomeDir().canonicalPath(),
                                                             KaboomSettings::instance().kde3backupDir().path(),
                                                             RecursiveDirJob::RemoveDestination);
      connect(job, SIGNAL(errorOccured(QString)), this, SLOT(errorhandling(QString)) );
      job->synchronousRun(progress);
      delete job;
    }
  }

  RecursiveDirJob *job = NULL;
  switch(selection)
  {
      case MigrationTool::Migrate:
        progress->setMaximum(1); //fake the progress bar progress.
        progress->setValue(1);
        qDebug() << "operation: do nothing, let kconf_update do magic";
        break;
      case MigrationTool::Merge:
        job = RecursiveDirJob::recursiveCpDir(KaboomSettings::instance().kde4homeDir().canonicalPath(),
                                              KaboomSettings::instance().kdehomeDir().canonicalPath(),
                                              RecursiveDirJob::OverWrite | RecursiveDirJob::ReplaceKde4InFiles);
        qDebug() << "operation: do magic experimental merge";
        break;
      case MigrationTool::Clean:
        qDebug() << "operation: do recursive rm of .kde dir if exists";
        job = RecursiveDirJob::recursiveRmDir(KaboomSettings::instance().kdehomeDir().path());
        break;
      case MigrationTool::Move:
        job = RecursiveDirJob::recursiveCpDir(KaboomSettings::instance().kde4homeDir().canonicalPath(),
                                              KaboomSettings::instance().kdehomeDir().path(),
                                              RecursiveDirJob::RemoveDestination | RecursiveDirJob::ReplaceKde4InFiles);
        qDebug() << "operation: copymove .kde4 over .kde";
        break;
  }

  if ( job ) {
    connect(job, SIGNAL(errorOccured(QString)), this, SLOT(errorhandling(QString)) );
    job->synchronousRun(progress);
    delete job;
  }

  if (errorbox->isVisible()) // if errors, ...
  {
    setPageState(FinishedWithError);
  } else {
    setPageState(FinishedSuccessfully);
  }
}

void MigrationPagePrivate::errorhandling(const QString& err)
{
  q->wizard()->button(QWizard::CancelButton)->setEnabled(!err.isEmpty());
  static_cast<MigrationTool*>(q->wizard())->setMigrationError(err);
  if ( err.isEmpty() ) {
      error->clear();
  } else {
      error->append(err);
  }
  bool isHidden = errorbox->isHidden();
  if (isHidden != err.isEmpty()) {
    errorbox->setVisible(!err.isEmpty());
    // trigger updateLayout
    q->wizard()->setTitleFormat(q->wizard()->titleFormat());
  }
}

MigrationPage::MigrationPage(QWidget *parent) : QWizardPage(parent)
{
  d=new MigrationPagePrivate(this);
  connect(d->start,SIGNAL(clicked()),d,SLOT(doMagic()));
  connect(this,SIGNAL(completeChanged()),this,SLOT(operationComplete()));
}

void MigrationPage::operationComplete()
{
  if (isComplete()) {
    if (!d->text->text().endsWith(" ")) {
        d->text->setText(d->text->text() + " ");
    }
    d->text->setText(d->text->text() + tr("Click \"Finish\" to exit the wizard and start loading KDE 4."));
  }
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
  ChoicePage* choice = qobject_cast<ChoicePage*>(page);
  if(choice)
  {
    d->selection=choice->selected();
    d->scenario->setText(tr("<strong>Selected scenario:</strong> %1<br/>"
        "<strong>Backup:</strong> %2")
            .arg(choice->selectedText())
            .arg((d->backup) ? tr("enabled") : tr("disabled")));
  }
  else
  {
    qFatal("Cast failed");
  }

  d->setPageState(MigrationPagePrivate::Initial);
}

