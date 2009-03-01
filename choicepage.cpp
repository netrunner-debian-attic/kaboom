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
#include "choicepage.h"
#include "diroperations/diroperations.h"
#include "richradiobutton.h"
#include "diroperations/progresswidget.h"
#include "kaboomsettings.h"

class ChoicePagePrivate
{
  public:
    ChoicePagePrivate() : backup(NULL) {}
    QLabel *text;
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
  setTitle(tr("Kaboom - Migration Tool"));
  d->buttons = new QButtonGroup(this);
  d->text = new QLabel(tr("Please select your settings migration option"),this);
  d->text->setWordWrap(true);
  lay->addWidget(d->text);
  if(KaboomSettings::instance().kdehomeDir().exists())
  {
    d->migrate = new RichRadioButton(tr("Migrate settings from KDE3 to KDE4 (standard)"),this);
    d->buttons->addButton(d->migrate,MigrationTool::Migrate);
    lay->addWidget(d->migrate);
    d->migrate->setChecked(true);
  }
  if(KaboomSettings::instance().kde4homeDir().exists())
  {
    d->move = new RichRadioButton(tr("Use existing KDE 4 settings and <b>replace</b> KDE 3 settings"));
    d->buttons->addButton(d->move,MigrationTool::Move);
    lay->addWidget(d->move);
    if(KaboomSettings::instance().kdehomeDir().exists())
    {
      d->merge = new RichRadioButton(tr("Merge settings from KDE3 and KDE4 (experimental)"));
      d->buttons->addButton(d->merge,MigrationTool::Merge);
      lay->addWidget(d->merge);
    }
    else
    {
      d->move->setChecked(true);
    }
  }
  d->clean = new RichRadioButton(tr("Start with default KDE settings and data. This option will <b>remove</b> all data and settings such as contacts, local stored mails, accounts in KMail and Kopete, bookmarks and other such data"),this);
  d->buttons->addButton(d->clean,MigrationTool::Clean);
  lay->addWidget(d->clean);
  d->progresswidget = new ProgressWidget;
  d->progresswidget->setMaximum(10);
  lay->addWidget(d->progresswidget);
  d->backup = new QCheckBox(tr("Backup existing settings from KDE3 into .kde3-backup. (Highly recommended)"));
  registerField("backup",d->backup);
  lay->addWidget(d->backup);
  d->backup->hide();
  d->backup->setChecked(false);
  if(KaboomSettings::instance().kdehomeDir().exists()) //if no kdedir, nothing to backup.
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
      dirsize = DirOperations::calculateDirSize(QDir::homePath()+KDEDIR,d->progresswidget);
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
