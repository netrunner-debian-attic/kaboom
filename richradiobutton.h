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
#ifndef RICHRADIOBUTTON_H
#define RICHRADIOBUTTON_H

#include <QtGui/QAbstractButton>

class RichRadioButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit RichRadioButton(QWidget *parent = 0);
    explicit RichRadioButton(const QString & text, QWidget *parent = 0);
    virtual ~RichRadioButton();

    //not virtual, do not call the parent's implementation!
    void setText(const QString & text);

protected:
    //reimplemented because it's pure virtual in QAbstractButton
    virtual void paintEvent(QPaintEvent *event);

private:
    struct Private;
    Private * const d;
};

#endif
