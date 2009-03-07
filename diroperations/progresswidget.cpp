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
#include <climits>

ProgressWidget::ProgressWidget(QWidget *parent)
    : QWidget(parent), m_overflow(false), m_max(0)
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

void ProgressWidget::setMaximum(quint64 max)
{
    m_overflow = (max > INT_MAX);
    m_max = max;
    m_progressBar->setMaximum(m_overflow ? 100 : static_cast<int>(max));
}

void ProgressWidget::setValue(quint64 value)
{
    if ( m_overflow )
        value = static_cast<int>((double(value) / double(m_max)) * 100);
    m_progressBar->setValue(static_cast<int>(value));
}

