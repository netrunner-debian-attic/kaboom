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
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <cstring>

#include "kaboomsettings.h"

#define DEFAULT_KDEDIR ".kde"
#define DEFAULT_KDE4DIR ".kde4"
#define DEFAULT_KDE3BACKUPDIR "kde3-backup"

KaboomSettings* KaboomSettings::s_instance = 0;

KaboomSettings& KaboomSettings::instance()
{
    return *s_instance;
}

void KaboomSettings::initDefaults()
{
    const QString& homedir = QDir::homePath();
    s_instance = this;

    setKdehomePath(KdeHome, homedir + DEFAULT_KDEDIR);
    setKdehomePath(Kde4Home, homedir + DEFAULT_KDE4DIR);
    setKdehomePath(Kde3Backup, homedir + DEFAULT_KDE3BACKUPDIR);
}

KaboomSettings::KaboomSettings()
{
    initDefaults();
}

KaboomSettings::KaboomSettings(int argc, char** argv)
{
    initDefaults();

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--kdehome") == 0 && i+1 < argc) {
            setKdehomePath(KdeHome, QString::fromLocal8Bit(argv[++i]));
        } else if (strcmp(argv[i], "--kde4home") == 0 && i+1 < argc) {
            setKdehomePath(Kde4Home, QString::fromLocal8Bit(argv[++i]));
        } else if (strcmp(argv[i], "--kde3backup") == 0 && i+1 < argc) {
            setKdehomePath(Kde3Backup, QString::fromLocal8Bit(argv[++i]));
        } else if (strcmp(argv[i], "--help") == 0) {
            // TODO: show help
        }
    }
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
    qDebug() << "kaboom stamp -" << "exists?: " << ((stampExists()) ? "YES" : "NO");
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

bool KaboomSettings::stampExists() const
{
    return (QFile::exists(QDir::homePath()+"/.local/kdebian3to4"));
}

void KaboomSettings::touchStamp()
{
    QFile(QDir::homePath()+"/.local/kdebian3to4").open(QIODevice::WriteOnly);
}
