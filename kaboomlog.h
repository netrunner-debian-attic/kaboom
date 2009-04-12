/*
    Copyright (C) 2009  George Kiagiadakis <gkiagia@users.sourceforge.net>

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
#ifndef KABOOMLOG_H
#define KABOOMLOG_H

#include <QtCore/QtGlobal>
class QFile;

class KaboomLog
{
public:
    static void init(const QString & fileName);
    static void cleanup();

private:
    static QFile *m_logFile;
    static void logMessageOutput(QtMsgType type, const char *msg);
};

#endif
