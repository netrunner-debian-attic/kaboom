/*
    Copyright (C) 2009  George Kiagiadakis <gkiagia@users.sourceforge.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TEST_H
#define TEST_H

#include <QDialog>
class QLineEdit;
class ProgressWidget;

class Test : public QDialog
{
    Q_OBJECT
public:
    Test();

public slots:
    void docopy();

private:
    QLineEdit *m_source, *m_dest;
    ProgressWidget *m_progressWidget;
};

#endif
