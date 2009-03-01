/*
    Copyright (C) 2009 Sune Vuorela <sune@vuorela.dk>
              (C) 2009 Modestas Vainius <modestas@vainius.eu>

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
    QWidget *backupinformation;
    QProgressBar *spacebar;
    ProgressWidget *progresswidget;
    QLabel *freeinfo;
};

ChoicePage::ChoicePage(QWidget *parent) : QWizardPage(parent)
{
  d = new ChoicePagePrivate;
  QVBoxLayout *lay = new QVBoxLayout(this);
  KaboomSettings& s = KaboomSettings::instance();

  setTitle(tr("Setting migration options"));
  d->buttons = new QButtonGroup(this);

  d->configInfoLabel = new QLabel(tr("Current configuration:"));
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
  QFont font;
  font.setBold(true);
  d->scenariosLabel->setFont(font);
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
        tr("<p>The wizard won't make any changes to your current KDE&nbsp;3 settings directory. "
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
        tr("Use existing KDE&nbsp;4 settings and <strong>replace</strong> current KDE&nbsp;3 settings."),
        tr("<p>The wizard will remove current KDE&nbsp;3 settings directory and move "
           "current KDE&nbsp;4 settings directory into its place. Effectively, you will "
           "lose all settings and data the KDE&nbsp;3 desktop and applications have stored "
           "unless the backup option below is enabled. This scenario should be useful for "
           "users who already actively and almost exclusively use KDE&nbsp;4 desktop and "
           "applications as previously packaged by Debian and do not care about losing "
           "settings of (a few) KDE&nbsp;3 applications anymore.</p>"),
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
    tr("<p>The wizard will <strong>remove</strong> existing KDE&nbsp;3 settings directory "
       "(optionally backing it up first) and won't touch existing KDE&nbsp;4 settings "
       "directory if it exists. Effectively, once this wizard is complete you will be "
       "presented with default KDE&nbsp;4 desktop. Do not select this scenario (without "
       "enabling the backup option below) if you don't want to irreversibly lose such data "
       "as contacts, locally stored mails, accounts in KMail and Kopete, bookmarks etc. "
       "as stored by previous KDE&nbsp;3 versions of the respective applications. This "
       "scenario is recommended for users who want to start with fresh KDE&nbsp;4 profile and, "
       "optionally, do migration of KDE&nbsp;3 and/or old KDE&nbsp;4 settings manually.</p>"),
    this);
  d->buttons->addButton(d->clean,MigrationTool::Clean);
  lay->addWidget(d->clean);
  d->progresswidget = new ProgressWidget;
  d->progresswidget->setMaximum(10);
  lay->addWidget(d->progresswidget);
  d->backup = new QCheckBox(
    tr("Backup existing KDE 3 settings into %1. (Highly recommended)")
        .arg(s.kdehomePrettyPath(KaboomSettings::Kde3Backup)),
    this);
  registerField("backup",d->backup);
  lay->addWidget(d->backup);
  d->backup->hide();
  d->backup->setChecked(false);
  if(s.kdehomeDir().exists()) //if no kdedir, nothing to backup.
  {
    d->backupinformation = new QWidget(this);
    QVBoxLayout *blay = new QVBoxLayout(d->backupinformation);
    QLabel *freewarning = new QLabel(tr("Insufficient free space to complete a backup, please consider freeing up some space. You can go to TTY1 to do this."),this);
    d->spacebar = new QProgressBar(this);
    d->spacebar->setMaximum(100);
    d->freeinfo = new QLabel;
    QPushButton *recheck = new QPushButton(tr("Recheck"));
    blay->addWidget(freewarning);
    blay->addWidget(d->spacebar);
    blay->addWidget(d->freeinfo);
    blay->addWidget(recheck);
    lay->addWidget(d->backupinformation);
    connect(recheck,SIGNAL(clicked()),this,SLOT(checkSpaceForBackup()));
  }
}

void ChoicePage::initializePage()
{
  QTimer::singleShot(0, this, SLOT(checkSpaceForBackup()));
}

bool ChoicePage::backupSelected() const
{
  if(0)
    qDebug() << tr("Recheck");
  return d->backup ? d->backup->isChecked() : false;
}

void ChoicePage::checkSpaceForBackup()
{
  d->progresswidget->setVisible(true);
  quint64 dirsize = -1;
  try
  {
      dirsize = DirOperations::calculateDirSize(KaboomSettings::instance().kdehomeDir().canonicalPath(),d->progresswidget);
  }
  catch (const DirOperations::Exception&)
  {
      // nop - default set before.
  }
  quint64 freespace = DirOperations::freeDirSpace(QDir::homePath());
  if(dirsize > freespace)
  {
    quint64 partsize = DirOperations::totalPartitionSize(QDir::homePath());
    d->spacebar->setValue(round(static_cast<double>(partsize-freespace)/static_cast<double>(partsize)*100));
    d->freeinfo->setText(tr("The current KDE settings and data directory takes up %1 bytes").arg(dirsize));
    d->backupinformation->setVisible(true);
    d->backup->hide();
  }
  else
  {
    d->backupinformation->setVisible(false);
    d->progresswidget->setVisible(false);
    d->backup->setChecked(true);
    d->backup->show();
  }
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
