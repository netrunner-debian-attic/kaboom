/*
    Copyright (C) 2009 Modestas Vainius <modestas@vainius.eu>
                  2009 Sune Vuorela <sune@vuorela.dk>

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
#include "kaboomsettings.h"
#include "kaboomlog.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <unistd.h>

#define DEFAULT_KDEDIR "/.kde"
#define DEFAULT_KDE4DIR "/.kde4"
#define DEFAULT_KDE3BACKUPDIR "/kde3-backup"
#define KABOOM_STAMP "/.local/kaboom"
#define KABOOM_LOG "/.kaboom.log"

KaboomSettings* KaboomSettings::s_instance = 0;

KaboomSettings& KaboomSettings::instance()
{
    return *s_instance;
}

void KaboomSettings::initDefaults()
{
    const QString homedir = QDir::homePath();
    s_instance = this;

    setKdehomePath(KdeHome, homedir + DEFAULT_KDEDIR);
    setKdehomePath(Kde4Home, homedir + DEFAULT_KDE4DIR);
    setKdehomePath(Kde3Backup, homedir + DEFAULT_KDE3BACKUPDIR);
    m_stampFile.setFileName(homedir + KABOOM_STAMP);
    m_logFile = homedir + KABOOM_LOG;
}

KaboomSettings::KaboomSettings()
{
    initDefaults();
    KaboomLog::init(m_logFile);
}

KaboomSettings::KaboomSettings(int argc, char** argv)
{
    initDefaults();

    for (int i = 1; i < argc; i++) {
        if (qstrcmp(argv[i], "--kdehome") == 0 && i+1 < argc) {
            setKdehomePath(KdeHome, QFile::decodeName(argv[++i]));
        } else if (qstrcmp(argv[i], "--kde4home") == 0 && i+1 < argc) {
            setKdehomePath(Kde4Home, QFile::decodeName(argv[++i]));
        } else if (qstrcmp(argv[i], "--kde3backup") == 0 && i+1 < argc) {
            setKdehomePath(Kde3Backup, QFile::decodeName(argv[++i]));
        } else if (qstrcmp(argv[i], "--stamp") == 0 && i+1 < argc) {
            m_stampFile.setFileName(QFile::decodeName(argv[++i]));
        } else if (qstrcmp(argv[i], "--log") == 0) {
            m_logFile = QFile::decodeName(argv[++i]);
        } else if (qstrcmp(argv[i], "--help") == 0) {
            // TODO: show help
        }
    }

    KaboomLog::init(m_logFile);
}

KaboomSettings::~KaboomSettings()
{
    KaboomLog::cleanup();
}

void KaboomSettings::dump()
{
    qDebug() << "---- Kaboom Settings Dump ----";
    qDebug() << "kdehome -" << kdehomeDir().path() << "- exists?:" <<
        ((kdehomeDir().exists()) ? "YES" : "NO");
    qDebug() << "kde4home -" << kde4homeDir().path() << "- exists?:" <<
        ((kde4homeDir().exists()) ? "YES" : "NO");
    qDebug() << "kde3backup -" << kde3backupDir().path() << "- exists?:" <<
        ((kde3backupDir().exists()) ? "YES" : "NO");
    qDebug() << "kaboom stamp -" << stampFile().fileName() << "- exists?: " <<
        ((stampExists()) ? "YES" : "NO");
    qDebug() << "kaboom log -" << m_logFile;
    qDebug() << "---- -------------------- ----";
}

void KaboomSettings::setKdehomePath(KdeHomeType type, const QString & path)
{
    QDir dir(path);
    m_kdehomes[type] = dir;
    m_prettyKdehomes[type] = path;

    /* Determine pretty path (replace $HOME with ~) */
    QStringList paths;
    QString homepath = QDir::homePath();

    paths.append(dir.absolutePath());
    paths.append(dir.canonicalPath());
    foreach (QString path, paths) {
        if (path.indexOf(homepath) == 0) {
            path.replace(0, homepath.length(), "~");
            m_prettyKdehomes[type] = path;
            break;
        }
    }
}

const QDir& KaboomSettings::kdehomeDir(KdeHomeType type) const
{
    return m_kdehomes[type];
}

QString KaboomSettings::kdehomePrettyPath(KdeHomeType type) const
{
    return m_prettyKdehomes[type];
}

const QFile& KaboomSettings::stampFile() const
{
    return m_stampFile;
}

bool KaboomSettings::stampExists() const
{
    return m_stampFile.exists();;
}

void KaboomSettings::touchStamp()
{
    if (m_stampFile.open(QIODevice::WriteOnly)) {
        fsync(m_stampFile.handle());
        m_stampFile.close();
    }
}
