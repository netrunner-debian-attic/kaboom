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
#include "migrationtool.h"
#include "kaboomsettings.h"
#include <QApplication>
#include <QFile>
#include <QTranslator>

int main(int argc, char* argv[])
{
  KaboomSettings settings(argc, argv);
  int exitvalue;

  settings.dump(); // dump settings for debugging purposes

  if(settings.stampExists())
  {
    return 0;
  }
  if(!settings.kdehomeDir().exists() && !settings.kde4homeDir().exists())
  {
      settings.touchStamp();
      return 0;
  }

  QApplication app(argc,argv);

  QString locale = QLocale::system().name();

  QTranslator translator;
  if(!translator.load(QString("kaboom_") + locale, "/usr/share/kaboom"))
  {
      qDebug() << "loading translation failed";
  }
  app.installTranslator(&translator);

  MigrationTool main;
  exitvalue = main.exec();
  if(exitvalue == QDialog::Accepted)
  {
      // disable for debugging
      // settings.touchStamp();
      exitvalue = 0; // OK
  } else if (exitvalue == QDialog::Rejected) {
      if (!main.migrationError().isEmpty()) {
          qDebug() << "Migration failed with error:" << main.migrationError();
          exitvalue = 2; // Failed
      } else {
          exitvalue = 1; // Canceled by the user
      }
  }

  return exitvalue;
}
