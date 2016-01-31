#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>
#include <QSharedPointer>

class QSettings;

namespace Ui {
class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(const QSharedPointer<QSettings> &settings, QWidget *parent = 0);
    ~PreferenceDialog();

    QString getSysDictDir() const;
    QString getUserDictFile() const;

    void setSysDictDir(const QString &sysDictDir);
    void setUserDictFile(const QString &userDictFile);

    void saveSettings();

public slots:
    void on_toolButtonPathSysDict_clicked();
    void on_toolButtonPathUserDict_clicked();
    void on_lineEditPathSysDict_textChanged();
    void on_lineEditPathUserDict_textChanged();

private:
    bool isSysDictDir(const QString &dirPath) const;
    bool isUserDictFile(const QString &filePath) const;

    QSharedPointer<Ui::PreferenceDialog> ui;
    QSharedPointer<QSettings> settings;

    QPalette defaultPalette;
    QPalette wrongPalette;

};

#endif // PREFERENCEDIALOG_H
