/*
    Copyright (C) 2009 Modestas Vainius <modestas@vainius.eu>

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
#ifndef KABOOMSETTINGS_H
#define KABOOMSETTINGS_H

#include <QtCore/QString>
#include <QtCore/QDir>

#define KDEHOMES_COUNT      3

class KaboomSettings {

  public:
    enum KdeHomeType {
        KdeHome = 0x0,
        Kde4Home = 0x1,
        Kde3Backup = 0x2,
    };

    explicit KaboomSettings(int argc, char** argv);
    explicit KaboomSettings();

    static KaboomSettings& instance();

    const QDir & kdehomeDir(KdeHomeType type=KdeHome) const;
    QString kdehomePrettyPath(KdeHomeType type=KdeHome) const;
    void setKdehomePath(KdeHomeType type, const QString & path);

    /* Convenience methods */
    const QDir& kde4homeDir() const { return kdehomeDir(Kde4Home); }
    QString kde4homePrettyPath() const { return kdehomePrettyPath(Kde4Home); }
    const QDir& kde3backupDir() const { return kdehomeDir(Kde3Backup); }

    /* Stamp handling */
    const QFile& stampFile() const;
    bool stampExists() const;
    void touchStamp();

    /* Debugging */
    void dump();

  private:
    static KaboomSettings* s_instance;

    QDir m_kdehomes[KDEHOMES_COUNT];
    QString m_prettyKdehomes[KDEHOMES_COUNT];
    QFile m_stampFile;
    void initDefaults();
};

#endif
