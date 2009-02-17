#ifndef MIGRATIONPAGE_P_H
#define MIGRATIONPAGE_P_H

#include <QtCore>
#include "migrationpage.h"
#include "migrationtool.h"

class MigrationPagePrivate : public QObject
{
  Q_OBJECT
  public:
    MigrationPagePrivate(MigrationPage *parent);
    MigrationTool::Selection selection;
    MigrationPage *q;
    bool backup;
    bool complete;
  public slots:
    void doMagic();
};

#endif // MigrationPagePrivate