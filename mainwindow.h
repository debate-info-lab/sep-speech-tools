#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class QSettings;

namespace MeCab {
class Tagger;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initializeTagger(QString dictPath, QString userDictPath=QString());
    void setTextInformation(int charCount, int speechCount) const;

public slots:
    void on_textEdit_textChanged();

private:
    QSharedPointer<Ui::MainWindow> ui;
    QSharedPointer<QSettings> settings;

    QSharedPointer<MeCab::Tagger> tagger;

};

#endif // MAINWINDOW_H
