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
#include "kaboomlog.h"
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <iostream>

QFile *KaboomLog::m_logFile = 0;

void KaboomLog::init(const QString & fileName)
{
    m_logFile = new QFile(fileName);
    if ( !m_logFile->open(QIODevice::WriteOnly) ) {
        delete m_logFile;
        qCritical() << "Could not open log file" << fileName;
    } else {
        qInstallMsgHandler(logMessageOutput);
    }
}

void KaboomLog::cleanup()
{
    if ( m_logFile != 0 ) {
        qInstallMsgHandler(0); //restore the default message handler
        m_logFile->close();
        delete m_logFile;
        m_logFile = 0;
    }
}

void KaboomLog::logMessageOutput(QtMsgType type, const char *msg)
{
    m_logFile->write(msg);
    m_logFile->write("\n");
    std::cerr << msg << std::endl;
    if ( type == QtFatalMsg ) {
        cleanup();
        abort();
    }
}
