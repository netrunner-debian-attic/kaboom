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
#include "progresswidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QApplication>

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_label = new QLabel(this);
    layout->addWidget(m_label);

    m_progressBar = new QProgressBar(this);
    layout->addWidget(m_progressBar);
}

void ProgressWidget::setLabelText(const QString & text)
{
    m_label->setText(text);
}

bool ProgressWidget::wasCanceled() const
{
    return false;
}

void ProgressWidget::setMaximum(int max)
{
    m_progressBar->setMaximum(max);
}

void ProgressWidget::setValue(int value)
{
    m_progressBar->setValue(value);
}

void ProgressWidget::processEvents()
{
    qApp->processEvents();
}
