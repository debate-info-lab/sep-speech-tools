#include "autocursor.h"

#include <QWidget>

AutoCursor::AutoCursor(QWidget *widget, Qt::CursorShape shape) :
    widget(widget),
    before(widget->cursor()),
    after(shape)
{
    this->widget->setCursor(this->after);
}

AutoCursor::~AutoCursor()
{
    this->widget->setCursor(this->before);
}
