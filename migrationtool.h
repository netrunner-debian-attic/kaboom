#include <QtGui>
#ifndef MIGRATIONTOOL_H
#define MIGRATIONTOOL_H

class MigrationToolPrivate;
class MigrationTool : public QWizard
{
  public:
    enum Selection
    {
      Migrate, /*do nothing, let kconf_update do its magic*/
      Merge, /*merge .kde4 into .kde and let kconf_update do its magic*/
      Clean, /*clean .kde*/
      Move, /*move .kde4 to .kde*/
    };
    enum 
    {
      Intro, /* tell about migration */
      Choice, /* give the choices */
      Migration, /*do the actual migration - including start button*/
      Warning, /*warn if no backup and descructive choice made*/
    };
    MigrationTool(QWidget *parent=0);
    int nextId() const;
  private:
    MigrationToolPrivate *d;
    
};

#endif // MAINWINDOW_