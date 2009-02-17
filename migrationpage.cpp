#include "migrationpage.h"
#include "choicepage.h"
#include "migrationpage_p.h"


MigrationPagePrivate::MigrationPagePrivate(MigrationPage* parent)
{
  q=parent;
  complete=false;
  backup=true;
  selection=MigrationTool::Migrate;
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