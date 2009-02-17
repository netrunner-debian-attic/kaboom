#ifndef CHOICEPAGE_H
#define CHOICEPAGE_H
#include <QtGui>
#include "migrationtool.h"

class ChoicePagePrivate;
class ChoicePage : public QWizardPage
{
  public:
    ChoicePage(QWidget *parent=0);
    bool backupSelected() const;
    MigrationTool::Selection selected() const;
  private:
    ChoicePagePrivate *d;



};


#endif // CHOICEPAGE_H