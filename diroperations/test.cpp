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
#include "test.h"
#include "progresswidget.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <cstdlib>

Test::Test()
    :QDialog()
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    m_source = new QLineEdit(this);
    lay->addWidget(m_source);
    m_dest = new QLineEdit(this);
    lay->addWidget(m_dest);

    QPushButton *m_button = new QPushButton("copy", this);
    connect(m_button, SIGNAL(clicked()), this, SLOT(docopy()));
    lay->addWidget(m_button);

    m_progressWidget = new ProgressWidget(this);
    lay->addWidget(m_progressWidget);
}

void Test::docopy()
{
    try {
        DirOperations::recursiveCpDir(m_source->text(), m_dest->text(), true, m_progressWidget);
    } catch (const DirOperations::Exception & e) {
        qDebug() << e.what();
        std::exit(1);
    }
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Test t;
    t.show();
    return a.exec();
}

#include "test.moc"
