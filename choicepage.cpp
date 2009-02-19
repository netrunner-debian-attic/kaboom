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
    ChoicePagePrivate()
        : buttons(NULL), backup(NULL) {}
    QButtonGroup *buttons;
    QCheckBox *backup;
};

ChoicePage::ChoicePage(QWidget *parent) : QWizardPage(parent)
{
  d = new ChoicePagePrivate;
  QVBoxLayout *lay = new QVBoxLayout(this);
  setTitle("Kaboom - MigrationTool");
  bool haskde4dir = QFile::exists(QDir::homePath()+KDE4DIR);
  bool haskdedir = QFile::exists(QDir::homePath()+KDEDIR);
  d->buttons = new QButtonGroup(this);
  QLabel *text = new QLabel(tr("Please select the option on how you want to migrate your settings"),this);
  lay->addWidget(text);
  if(haskdedir)
  {
    QRadioButton *migrate = new QRadioButton("Migrate settings from KDE3 to KDE4 (recommended)",this);
    d->buttons->addButton(migrate,MigrationTool::Migrate);
    lay->addWidget(migrate);
    migrate->setChecked(true);
  }
  if(haskde4dir)
  {
    QRadioButton *move = new QRadioButton("Move settings from KDE 4 dir and <b>replace</b> settings from KDE 3");
    d->buttons->addButton(move,MigrationTool::Move);
    lay->addWidget(move);
    if(haskdedir)
    {
      QRadioButton *merge = new QRadioButton("Merge settings from KDE3 and KDE4 (experimental)");
      d->buttons->addButton(merge,MigrationTool::Merge);
      lay->addWidget(merge);
    }
    else
    {
      move->setChecked(true);
    }
  }
  QRadioButton *clean = new QRadioButton("Start with a fresh KDE. This option will <b>remove</b> data and settings such as contacts, local stored mails, accounts in KMail and Kopete, bookmarks and other such data",this);
  d->buttons->addButton(clean,MigrationTool::Clean);
  lay->addWidget(clean);
  if(haskdedir) //if no kdedir, nothing to backup.
  {
    d->backup = new QCheckBox("Backup existing settings from KDE3 into .kde3-backup. (Highly recommended)");
    d->backup->setChecked(true);
    registerField("backup",d->backup);
    lay->addWidget(d->backup);
  }
}

bool ChoicePage::backupSelected() const
{
  return d->backup != NULL ? d->backup->isChecked() : false;
}

MigrationTool::Selection ChoicePage::selected() const
{
  int selection = d->buttons->checkedId();
  Q_ASSERT(selection >= 0 && selection < 4); //update if more options are added
  return (MigrationTool::Selection) selection;
}
