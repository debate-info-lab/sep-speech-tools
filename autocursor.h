#ifndef AUTOCURSOR_H
#define AUTOCURSOR_H

#include <QCursor>

class QWidget;

class AutoCursor
{
public:
    AutoCursor(QWidget *widget, Qt::CursorShape shape);
    virtual ~AutoCursor();

private:
    QWidget *widget;
    Qt::CursorShape shape;
    QCursor before;
    QCursor after;

};

template <Qt::CursorShape SHAPE>
class AutoCursorTmpl : public AutoCursor
{
public:
    AutoCursorTmpl(QWidget *widget) :
        AutoCursor(widget, SHAPE)
    {}

};

typedef AutoCursorTmpl<Qt::BusyCursor> BusyAutoCursor;

#endif // AUTOCURSOR_H
