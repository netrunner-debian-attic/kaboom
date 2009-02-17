/***************************************************************************
 *   Copyright (C) 2007 by George Kiagiadakis                              *
 *   gkiagia@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#include <QDir>
#include <QFileInfo>

/*! Deletes directory \a dir and all of its contents. Works like "rm -r". */
bool recursiveRmDir(QDir dir)
{
    int i;
    QFileInfoList list = dir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden
                                            | QDir::System | QDir::CaseSensitive, QDir::DirsLast );
    for (i=0; i < list.count() ; i++)
    {
        if ( list[i].isDir() and not list[i].isSymLink() ) {
            if ( !recursiveRmDir( QDir(list[i].absoluteFilePath()) ) )
                return false;
        } else {
            if ( !dir.remove(list[i].fileName()) )
                return false;
        }
    }
    QString dirname = dir.dirName();
    dir.cdUp();
    return dir.rmdir(dirname);
}
