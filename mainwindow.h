#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setTextInformation(int charCount) const;

public slots:
    void on_textEdit_textChanged();

private:
    QSharedPointer<Ui::MainWindow> ui;
};

#endif // MAINWINDOW_H
