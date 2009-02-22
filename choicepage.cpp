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
    bool haskde4dir;
    bool haskdedir;
};

ChoicePage::ChoicePage(QWidget *parent) : QWizardPage(parent)
{
  d = new ChoicePagePrivate;
  QVBoxLayout *lay = new QVBoxLayout(this);
  setTitle("Kaboom - MigrationTool");
  d->haskde4dir = QFile::exists(QDir::homePath()+KDE4DIR);
  d->haskdedir = QFile::exists(QDir::homePath()+KDEDIR);
  d->buttons = new QButtonGroup(this);
  d->text = new QLabel(tr("Please select the option on how you want to migrate your settings"),this);
  lay->addWidget(d->text);
  if(d->haskdedir)
  {
    d->migrate = new RichRadioButton("Migrate settings from KDE3 to KDE4 (recommended)",this);
    d->buttons->addButton(d->migrate,MigrationTool::Migrate);
    lay->addWidget(d->migrate);
    d->migrate->setChecked(true);
  }
  if(d->haskde4dir)
  {
    d->move = new RichRadioButton("Move settings from KDE 4 dir and <b>replace</b> settings from KDE 3");
    d->buttons->addButton(d->move,MigrationTool::Move);
    lay->addWidget(d->move);
    if(d->haskdedir)
    {
      d->merge = new RichRadioButton("Merge settings from KDE3 and KDE4 (experimental)");
      d->buttons->addButton(d->merge,MigrationTool::Merge);
      lay->addWidget(d->merge);
    }
    else
    {
      d->move->setChecked(true);
    }
  }
  d->clean = new RichRadioButton("Start with a fresh KDE. This option will <b>remove</b> data and settings such as contacts, local stored mails, accounts in KMail and Kopete, bookmarks and other such data",this);
  d->buttons->addButton(d->clean,MigrationTool::Clean);
  lay->addWidget(d->clean);
  if(d->haskdedir) //if no kdedir, nothing to backup.
  {
    quint64 dirsize = DirOperations::calculateDirSize(QDir::homePath()+KDEDIR);
    qDebug() << "dirsize" << dirsize;
    quint64 freespace = DirOperations::freeDirSpace(QDir::homePath());
    qDebug() << "freespace" << freespace;
    if(true)//dirsize > freespace)
    {
      quint64 partsize = DirOperations::totalPartitionSize(QDir::homePath());
      qDebug() << "partsize" << partsize;
      QLabel *freewarning = new QLabel("You have not enough free space to actually do a backup, please consider freeing up some space by going to TTY1",this);
      QProgressBar *bar = new QProgressBar(this);
      bar->setMaximum(100);
      qDebug() << "freepercent" << (partsize-freespace)/partsize*100;
      bar->setValue(round(static_cast<double>(partsize-freespace)/static_cast<double>(partsize)*100));
      QLabel *freeinfo = new QLabel(QString("The current kde settings and data dir takes up %1 bytes").arg(dirsize));
      lay->addWidget(freewarning);
      lay->addWidget(bar);
      lay->addWidget(freeinfo);
    }
    else
    {
      d->backup = new QCheckBox("Backup existing settings from KDE3 into .kde3-backup. (Highly recommended)");
      d->backup->setChecked(true);
      registerField("backup",d->backup);
      lay->addWidget(d->backup);
    }
  }
}

bool ChoicePage::backupSelected() const
{
  return d->backup ? d->backup->isChecked() : false;
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
