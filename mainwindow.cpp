#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
}

MainWindow::~MainWindow()
{}

void MainWindow::setTextInformation(int charCount) const
{
    QString message = tr("%1 chars").arg(charCount);
    this->ui->statusBar->showMessage(message, 0);
}

void MainWindow::on_textEdit_textChanged()
{
    QString text = this->ui->textEdit->toPlainText();

    QString serialized = text.remove(QRegExp("\\s+"));
    this->setTextInformation(serialized.size());
}
