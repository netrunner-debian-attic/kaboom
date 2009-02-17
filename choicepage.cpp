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

class ChoicePagePrivate
{
  public:
    QLabel *text;
    QRadioButton *clean;
    QRadioButton *migrate;
    QRadioButton *move;
    QRadioButton *merge;
    QButtonGroup *buttons;
    QCheckBox *backup;
    bool haskde4dir;
};

ChoicePage::ChoicePage(QWidget *parent) : QWizardPage(parent)
{
  d = new ChoicePagePrivate;
  QVBoxLayout *lay = new QVBoxLayout(this);
  setTitle("Kaboom - MigrationTool");
  d->haskde4dir = QFile::exists(QDir::homePath()+"/"+".kde4");
  d->buttons = new QButtonGroup(this);
  d->text = new QLabel(tr("Please select the option on how you want to migrate your settings"),this);
  lay->addWidget(d->text);
  d->clean = new QRadioButton("Start with a fresh KDE. This option will <b>remove</b> data and settings such as contacts, local stored mails, accounts in KMail and Kopete, bookmarks and other such data",this);
  d->migrate = new QRadioButton("Migrate settings from KDE3 to KDE4 (recommended)",this);
  d->buttons->addButton(d->clean,MigrationTool::Clean);
  d->buttons->addButton(d->migrate,MigrationTool::Migrate);
  d->migrate->setChecked(true);
  lay->addWidget(d->migrate);
  lay->addWidget(d->clean);
  if(d->haskde4dir)
  {
    d->move = new QRadioButton("Move settings from KDE 4 dir and <b>replace</b> settings from KDE 3");
    d->merge = new QRadioButton("Merge settings from KDE3 and KDE4 (experimental)");
    d->buttons->addButton(d->move,MigrationTool::Move);
    d->buttons->addButton(d->merge,MigrationTool::Merge);
    lay->addWidget(d->move);
    lay->addWidget(d->merge);
  }
  d->backup = new QCheckBox("Backup existing settings from KDE3 into .kde3-backup");
  d->backup->setChecked(true);
  registerField("backup",d->backup);
 // registerField("choice",d->buttons);
  lay->addWidget(d->backup);
}

bool ChoicePage::backupSelected() const
{
  return d->backup->isChecked();
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
