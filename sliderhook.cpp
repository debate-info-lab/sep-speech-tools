#include "sliderhook.h"

#include <QEvent>
#include <QMouseEvent>
#include <QSlider>

SliderHook::SliderHook(QSlider *target, QObject *parent) :
    QObject(parent),
    target(target)
{
}

bool SliderHook::eventFilter(QObject *obj, QEvent *event)
{
    // check target
    if ( obj != this->target ) {
        return false;
    }

    // check event
    if ( event->type() != QEvent::MouseButtonPress ) {
        return false;
    }

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    if ( mouseEvent->button() != Qt::LeftButton ) {
        return false;
    }

    // from <http://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump>
    int maximum = this->target->maximum();
    int minimum = this->target->minimum();
    if (this->target->orientation() == Qt::Vertical) {
        int height = this->target->height();
        int y = mouseEvent->y();
        this->target->setValue(minimum + ((maximum-minimum) * (height-y)) / height);
    } else {
        int width = this->target->width();
        int x = mouseEvent->x();
        this->target->setValue(minimum + ((maximum-minimum) * x) / width);
    }
    return true;
}
