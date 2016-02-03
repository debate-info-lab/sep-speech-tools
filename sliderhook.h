#ifndef SLIDERHOOK_H
#define SLIDERHOOK_H

#include <QObject>

class QSlider;

class SliderHook : public QObject
{
    Q_OBJECT
public:
    explicit SliderHook(QSlider *target, QObject *parent = 0);

    bool eventFilter(QObject *obj, QEvent *event);

private:
    QSlider *target;

};

#endif // SLIDERHOOK_H
