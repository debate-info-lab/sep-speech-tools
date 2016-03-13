#include "preferencedialog.h"
#include "ui_preferencedialog.h"

#include <QFileDialog>
#include <QSettings>

PreferenceDialog::PreferenceDialog(const QSharedPointer<QSettings> &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog),
    settings(settings),
    defaultPalette(),
    wrongPalette()
{
    ui->setupUi(this);

    this->ui->lineEditPathSysDict->setText(this->getSysDictDir());
    this->ui->lineEditPathUserDict->setText(this->getUserDictFile());

    this->defaultPalette.setColor(QPalette::Text, Qt::black);
    this->defaultPalette.setColor(QPalette::Base, Qt::white);

    this->wrongPalette.setColor(QPalette::Text, Qt::red);
    this->wrongPalette.setColor(QPalette::Base, Qt::white);

    this->on_lineEditPathSysDict_textChanged();
    this->on_lineEditPathUserDict_textChanged();
}

PreferenceDialog::~PreferenceDialog()
{

}

QString PreferenceDialog::getSysDictDir() const
{
    // default dir
    QDir appDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_MAC)
    appDir.cdUp(); // MacOS
    appDir.cdUp(); // Contents
    appDir.cdUp(); // *.app
#endif
    // get dir
    QDir dictDir(appDir.filePath("dict"));
    return this->settings->value("dict/system", dictDir.absolutePath()).toString();
}

QString PreferenceDialog::getUserDictFile() const
{
    return this->settings->value("dict/user", QString()).toString();
}

void PreferenceDialog::setSysDictDir(const QString &sysDictDir)
{
    this->settings->setValue("dict/system", sysDictDir);
}

void PreferenceDialog::setUserDictFile(const QString &userDictFile)
{
    this->settings->setValue("dict/user", userDictFile);
}

void PreferenceDialog::saveSettings()
{
    this->setSysDictDir(this->ui->lineEditPathSysDict->text());
    this->setUserDictFile(this->ui->lineEditPathUserDict->text());
}

void PreferenceDialog::on_toolButtonPathSysDict_clicked()
{
    QString newPath = QFileDialog::getExistingDirectory(this, tr("Select system dict directory"), this->getSysDictDir());
    if ( newPath.isEmpty() ) {
        return;
    }
    this->ui->lineEditPathSysDict->setText(newPath);
}

void PreferenceDialog::on_toolButtonPathUserDict_clicked()
{
    QString newPath = QFileDialog::getOpenFileName(this, tr("Select user dict file"), this->getUserDictFile());
    if ( newPath.isEmpty() ) {
        return;
    }
    this->ui->lineEditPathUserDict->setText(newPath);
}

void PreferenceDialog::on_lineEditPathSysDict_textChanged()
{
    if ( this->isSysDictDir(this->ui->lineEditPathSysDict->text()) ) {
        this->ui->lineEditPathSysDict->setPalette(this->defaultPalette);
    } else {
        this->ui->lineEditPathSysDict->setPalette(this->wrongPalette);
    }
}

void PreferenceDialog::on_lineEditPathUserDict_textChanged()
{
    if ( this->isUserDictFile(this->ui->lineEditPathUserDict->text()) ) {
        this->ui->lineEditPathUserDict->setPalette(this->defaultPalette);
    } else {
        this->ui->lineEditPathUserDict->setPalette(this->wrongPalette);
    }
}

bool PreferenceDialog::isSysDictDir(const QString &dirPath) const
{
    QDir dir(dirPath);
    return ( dir.exists() &&
             QFileInfo(dir.filePath("char.bin")).exists() &&
             QFileInfo(dir.filePath("dicrc")).exists() &&
             QFileInfo(dir.filePath("left-id.def")).exists() &&
             QFileInfo(dir.filePath("matrix.bin")).exists() &&
             QFileInfo(dir.filePath("pos-id.def")).exists() &&
             QFileInfo(dir.filePath("rewrite.def")).exists() &&
             QFileInfo(dir.filePath("right-id.def")).exists() &&
             QFileInfo(dir.filePath("sys.dic")).exists() &&
             QFileInfo(dir.filePath("unk.dic")).exists() );
}

bool PreferenceDialog::isUserDictFile(const QString &filePath) const
{
    QFileInfo info(filePath);
    return ( info.exists() &&
             info.suffix() == "dic" );
}
