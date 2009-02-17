#include "migrationtool.h"
#include "migrationtool_p.h"


MigrationToolPrivate::MigrationToolPrivate(MigrationTool* parent ) : QObject(parent)
{
  q=parent;
};

MigrationTool::MigrationTool(QWidget *parent) : QWizard(parent)
{
  d=new MigrationToolPrivate(this);
  d->intro = new IntroPage(this);
  d->choice = new ChoicePage(this);
  d->warning = new WarningPage(this);
  d->migration = new MigrationPage(this);
  
  setOptions(QWizard::NoBackButtonOnStartPage|QWizard::NoCancelButton|options());  
  
  setPage(Intro,d->intro);
  setPage(Choice, d->choice);
  setPage(Warning, d->warning); 
  setPage(Migration,d->migration);

  setStartId(MigrationTool::Intro);

}
int MigrationTool::nextId() const
{
    qDebug() << "nextId()";
    switch(currentId())
    {
      case Intro:
	return Choice;
	break;
      case Choice:
	if(!d->choice->backupSelected() && d->choice->selected()==Clean)
	{
	    return Warning;
	}
	return Migration;
	break;
      case Warning:
	return Migration;
      case Migration:
	return -1;
	break;
      default:
	abort();
	break;
    }
}