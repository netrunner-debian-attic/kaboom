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
