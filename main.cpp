#include <QtGui>
#include <migrationtool.h>

int main(int argc, char* argv[])
{
  QApplication app(argc,argv);
  if(QFile::exists(QDir::homePath()+"/.local/kdebian3to4"))
  {
    return 0;
  }
  MigrationTool main;
  main.show();
  return app.exec();

}