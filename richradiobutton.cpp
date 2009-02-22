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
#include "richradiobutton.h"

#include <QtCore/QEvent>
#include <QtGui/QRadioButton>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QStyle>
#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionButton>

//#include <QtCore/QDebug>

class EventEater : public QObject
{
public:
    EventEater(QObject *parent) : QObject(parent) {}
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
};

bool EventEater::eventFilter(QObject *watched, QEvent *event)
{
    switch(event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::HoverMove:
            event->ignore();
            return true;
        default:
            return QObject::eventFilter(watched, event);
    }
}


class PrivateRadioButton : public QRadioButton
{
public:
    PrivateRadioButton(RichRadioButton *parent);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    RichRadioButton *q;
};

PrivateRadioButton::PrivateRadioButton(RichRadioButton *parent)
    : QRadioButton(parent), q(parent)
{
    setMouseTracking(false); //disable it, since QRadioButton enables it...
}

void PrivateRadioButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStylePainter p(this);
    QStyleOptionButton opt;
    opt.initFrom(q);
    opt.rect = rect();
    opt.state |= (q->isChecked()) ? QStyle::State_On : QStyle::State_Off;
    p.drawControl(QStyle::CE_RadioButton, opt);
}


struct RichRadioButton::Private
{
    PrivateRadioButton *m_button;
    QLabel *m_label;

    void init(RichRadioButton *q);
};

void RichRadioButton::Private::init(RichRadioButton *q)
{
    QHBoxLayout *layout = new QHBoxLayout(q);
    m_button = new PrivateRadioButton(q);
    m_button->setFocusPolicy(Qt::NoFocus);
    m_label = new QLabel(q);
    m_label->setWordWrap(q);

    layout->addWidget(m_button);
    layout->addWidget(m_label);

    m_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //install event filter to reject keyboard & mouse events from child objects
    EventEater *eventEater = new EventEater(q);
    m_button->installEventFilter(eventEater);
    m_label->installEventFilter(eventEater);

    q->setCheckable(true);
    q->setAutoExclusive(true);
}


RichRadioButton::RichRadioButton(QWidget *parent)
    : QAbstractButton(parent), d(new Private)
{
    d->init(this);
}

RichRadioButton::RichRadioButton(const QString & text, QWidget *parent)
    : QAbstractButton(parent), d(new Private)
{
    d->init(this);
    setText(text);
}

RichRadioButton::~RichRadioButton()
{
    delete d;
}

void RichRadioButton::setText(const QString & text)
{
    d->m_label->setText(text);
    QAbstractButton::setText(text);
}

void RichRadioButton::paintEvent(QPaintEvent *event)
{
    d->m_button->update(); //this is for reacting in hover events.
    QWidget::paintEvent(event);
}