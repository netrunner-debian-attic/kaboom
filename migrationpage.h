#ifndef MIGRATIONPAGE_H
#define MIGRATIONPAGE_H

#include "migrationtool.h"
#include <QtGui>

class MigrationPagePrivate;
class MigrationPage : public QWizardPage
{
    Q_OBJECT
  public:
    MigrationPage(QWidget *parent=0);
    void setMigrationType(MigrationTool::Selection selection);
    void setBackup(bool b); 
    bool isComplete() const;
    void initializePage();
  signals:
    void completeChanged();

  private:
    MigrationPagePrivate *d;
    friend class MigrationPagePrivate;

};


#endif //MIGRATIONPAGE_H