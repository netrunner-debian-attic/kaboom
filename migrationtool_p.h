#include "migrationtool.h"

#ifndef MIGRATIONTOOL_P_H
#define MIGRATIONTOOL_P_H
#include "intropage.h"
#include "choicepage.h"
#include "warningpage.h"
#include "migrationpage.h"
class MigrationToolPrivate: public QObject
{
  Q_OBJECT

  public:
    MigrationToolPrivate(MigrationTool *parent);
    MigrationTool *q;
    IntroPage *intro;
    ChoicePage *choice;
    WarningPage *warning;
    MigrationPage *migration;
};


#endif // MIGRATIONTOOL_P_H