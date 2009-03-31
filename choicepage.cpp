/*
    Copyright (C) 2009 Sune Vuorela <sune@vuorela.dk>
              (C) 2009 Modestas Vainius <modestas@vainius.eu>
              (C) 2009 George Kiagiadakis <gkiagia@users.sourceforge.net>

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
#include "choicepage.h"
#include "diroperations/diroperations.h"
#include "richradiobutton.h"
#include "diroperations/progresswidget.h"
#include "kaboomsettings.h"

class ChoicePagePrivate
{
  public:
    ChoicePagePrivate() : backup(NULL) {}
    QLabel *configInfoLabel, *configLabel, *onceDoneLabel;
    QLabel *scenariosLabel;
    RichRadioButton *clean;
    RichRadioButton *migrate;
    RichRadioButton *move;
    RichRadioButton *merge;
    QButtonGroup *buttons;
    QCheckBox *backup;
    QLabel *freewarning;
    ProgressWidget *progresswidget;
    QPushButton *recheck;
};

ChoicePage::ChoicePage(QWidget *parent) : QWizardPage(parent)
{
  d = new ChoicePagePrivate;
  QVBoxLayout *lay = new QVBoxLayout(this);
  KaboomSettings& s = KaboomSettings::instance();

  setTitle(tr("Setting migration options"));
  d->buttons = new QButtonGroup(this);

  QFont boldFont;
  boldFont.setBold(true);

  d->configInfoLabel = new QLabel(tr("Current configuration:"));
  d->configInfoLabel->setFont(boldFont);
  d->configInfoLabel->setWordWrap(true);

  d->configLabel = new QLabel(tr("%1%2")
    .arg((s.kdehomeDir().exists()) ?
        tr("<strong>KDE&nbsp;3</strong> settings and data are at <u>%1</u><br/>").arg(s.kdehomePrettyPath()) : "")
    .arg((s.kde4homeDir().exists()) ?
        tr("<strong>KDE&nbsp;4</strong> settings and data are at <u>%1</u><br/>").arg(s.kde4homePrettyPath()) : ""),
    this);
  d->configLabel->setWordWrap(true);
  d->configLabel->setIndent(20);

  d->onceDoneLabel = new QLabel(
    tr("Once you finish with this wizard, <strong>KDE&nbsp;4</strong> will use <u>%1</u> "
       "to store user settings and data. Optionally, <strong>KDE&nbsp;3</strong> "
       "settings and data may be backed up to <u>%2</u>.")
            .arg(s.kdehomePrettyPath())
            .arg(s.kdehomePrettyPath(KaboomSettings::Kde3Backup)),
    this);
  d->onceDoneLabel->setWordWrap(true);

  d->scenariosLabel = new QLabel(tr("Please choose one of the following migration scenarios:"),this);
  d->scenariosLabel->setFont(boldFont);
  d->scenariosLabel->setWordWrap(true);

  lay->addWidget(d->configInfoLabel);
  lay->addWidget(d->configLabel);
  lay->addWidget(d->onceDoneLabel);
  lay->addSpacing(10);
  lay->addWidget(d->scenariosLabel);

  if(s.kdehomeDir().exists())
  {
    d->migrate = new RichRadioButton(
        tr("Use current KDE&nbsp;3 settings as initial for KDE&nbsp;4. (<i>standard</i>)"),
        tr("<p>The wizard will not make any changes to your current KDE&nbsp;3 settings directory. "
           "It may only optionally back this directory up if the appropriate option below is "
           "enabled. This scenario is recommended for users who have not actively used KDE&nbsp;4 "
           "or any of its applications (e.g. Okular or KTorrent which were released with Lenny) on "
           "this system previously and wish the new KDE&nbsp;4 desktop and its applications "
           "to use as much settings from the old KDE&nbsp;3 counterparts as possible.</p>"),
        this);
    d->buttons->addButton(d->migrate,MigrationTool::Migrate);
    lay->addWidget(d->migrate);
    d->migrate->setChecked(true);
  }
  if(s.kde4homeDir().exists())
  {
    d->move = new RichRadioButton(
        tr("<html>Use existing KDE&nbsp;4 settings%1.</html>")
          .arg(s.kdehomeDir().exists() ?
              tr(" and <strong>replace</strong> current KDE&nbsp;3 settings") : ""),
        (s.kdehomeDir().exists()) ?
          tr("<p>The wizard will remove current KDE&nbsp;3 settings directory and move "
           "current KDE&nbsp;4 settings directory into its place. Effectively, you will "
           "lose all settings and data the KDE&nbsp;3 desktop and applications have stored "
           "unless the backup option below is enabled. This scenario should be useful for "
           "users who already actively and almost exclusively use KDE&nbsp;4 desktop and "
           "applications as previously packaged by Debian and do not care about losing "
           "settings of (a few) KDE&nbsp;3 applications anymore.</p>") :
          tr("<p>The wizard will rename your current old KDE&nbsp;4 settings directory. "
              "Select this option if you would like to keep all KDE 4 settings and "
              "continue using KDE 4 desktop as before.</p>"),
        this);
    d->buttons->addButton(d->move,MigrationTool::Move);
    lay->addWidget(d->move);
    if(s.kdehomeDir().exists())
    {
      d->merge = new RichRadioButton(
        tr("Merge current KDE&nbsp;4 settings with current KDE&nbsp;3 settings. (<i>experimental</i>)"),
        tr("<p>The wizard will copy contents of the current KDE&nbsp;4 settings directory "
            "on top of the current KDE&nbsp;3 settings directory giving preference to "
            "the KDE&nbsp;4 configuration files in case of filename collisions. In theory, "
            "this should let you preserve settings of both KDE&nbsp;4 and a few KDE&nbsp;3 "
            "applications you still use. The downside is that this migration path is not well "
            "tested and the resulting KDE&nbsp;4 settings directory might get polluted with useless "
            "configuration and data files which KDE&nbsp;4 ports no longer use. Use this scenario "
            "at your own risk.</p>"),
        this);
      d->buttons->addButton(d->merge,MigrationTool::Merge);
      lay->addWidget(d->merge);
    }
    else
    {
      d->move->setChecked(true);
    }
  }
  d->clean = new RichRadioButton(
    tr("Start with default KDE settings and data."),
    tr("<p>%1The wizard will not touch KDE&nbsp;4 settings directory if it exists. Choose "
        "this scenario if you would like to start with default KDE&nbsp;4 "
       "desktop and/or want to do migration of old KDE settings manually.</p>")
    .arg((s.kdehomeDir().exists()) ?
      tr("<p>The wizard will <strong>remove</strong> (or move to backup) existing KDE&nbsp;3 "
       "settings directory including such data as contacts, locally stored mails, accounts "
       "in KMail and Kopete, bookmarks, etc. ") : ""),
    this);
  d->buttons->addButton(d->clean,MigrationTool::Clean);
  lay->addWidget(d->clean);

  d->backup = new QCheckBox(tr("Backup existing KDE 3 settings (highly recommended)"), this);
  d->backup->setChecked(false);
  registerField("backup",d->backup);
  lay->addWidget(d->backup);

  if(s.kdehomeDir().exists()) //if no kdedir, nothing to backup.
  {
    d->freewarning = new QLabel(this);
    d->freewarning->setWordWrap(true);
    d->freewarning->hide();

    QHBoxLayout *hlay = new QHBoxLayout;
    d->progresswidget = new ProgressWidget(this);
    hlay->addWidget(d->progresswidget);
    d->recheck = new QPushButton(tr("Check again"));
    hlay->addWidget(d->recheck, 0, Qt::AlignBottom);
    connect(d->recheck,SIGNAL(clicked()),this,SLOT(checkSpaceForBackup()));
    lay->addLayout(hlay);
    lay->addWidget(d->freewarning);
  }
  else
  {
    d->backup->hide();
  }
}

void ChoicePage::initializePage()
{
  if ( KaboomSettings::instance().kdehomeDir().exists() )
    QTimer::singleShot(0, this, SLOT(checkSpaceForBackup()));
}

bool ChoicePage::backupSelected() const
{
  return d->backup ? d->backup->isChecked() : false;
}

void ChoicePage::setFreeWarningText(bool show)
{
    d->freewarning->setText((show) ?
        tr("<p><strong>Warning:</strong> Insufficient free space to complete a backup. "
        "Consider freeing up some space.") : QString::null);
    d->freewarning->setToolTip((show) ?
        tr("To free up some disk case, cancel the wizard now or switch to virtual terminal.") :
        QString::null);
    if (show) d->freewarning->show();
}

void ChoicePage::checkSpaceForBackup()
{
  Q_ASSERT(KaboomSettings::instance().kdehomeDir().exists());

  //don't allow the user to change page while we are calculating...
  wizard()->button(QWizard::BackButton)->setEnabled(false);
  wizard()->button(QWizard::NextButton)->setEnabled(false);

  d->progresswidget->show();
  d->progresswidget->setToolTip(QString::null);
  d->recheck->hide();
  d->backup->setEnabled(false);
  d->backup->setChecked(false);
  setFreeWarningText(false);

  quint64 dirsize = -1;
  quint64 freespace = DirOperations::freeDirSpace(QDir::homePath());
  try {
      dirsize = DirOperations::calculateDirSize(
                    KaboomSettings::instance().kdehomeDir().canonicalPath(),
                    d->progresswidget
                );
  } catch (const DirOperations::Exception&) {}

  if(dirsize > freespace)
  {
    quint64 partsize = DirOperations::totalPartitionSize(QDir::homePath());
    d->progresswidget->setMaximum(partsize);
    d->progresswidget->setValue(partsize-freespace);
    d->progresswidget->setLabelText(
        tr("<p><i>The current KDE&nbsp;3 settings and data directory takes up %1</i></p>")
            .arg(DirOperations::bytesToString(dirsize))
    );
    d->progresswidget->setToolTip(tr("% of disk space currently used"));
    d->recheck->show();
    setFreeWarningText(true);
  }
  else
  {
    d->progresswidget->hide();
    d->recheck->hide();
    d->freewarning->hide();
    d->backup->setEnabled(true);
    d->backup->setChecked(true);
  }

  wizard()->button(QWizard::BackButton)->setEnabled(true);
  wizard()->button(QWizard::NextButton)->setEnabled(true);
}

MigrationTool::Selection ChoicePage::selected() const
{
  switch(d->buttons->checkedId())
  {
    case MigrationTool::Clean:
      return MigrationTool::Clean;
    case MigrationTool::Migrate:
      return MigrationTool::Migrate;
    case MigrationTool::Merge:
      return MigrationTool::Merge;
    case MigrationTool::Move:
      return MigrationTool::Move;
    default:
      abort();
  }
}
