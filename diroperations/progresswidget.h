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
#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include "diroperations.h"
#include <QWidget>
class QLabel;
class QProgressBar;

class ProgressWidget : public QWidget, public DirOperations::ProgressDialogInterface
{
public:
    ProgressWidget(QWidget *parent = 0);

    virtual void setLabelText(const QString & text);
    virtual bool wasCanceled() const;
    virtual void setMaximum(int max);
    virtual void setValue(int value);
    virtual void processEvents();

private:
    QLabel *m_label;
    QProgressBar *m_progressBar;
};

#endif
