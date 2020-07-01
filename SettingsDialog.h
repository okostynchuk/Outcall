#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <AddExtensionDialog.h>

#include <QDialog>
#include <QFile>

class QAbstractButton;
class QTcpSocket;
class AddExtensionDialog;
class QTreeWidgetItem;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    void saveSettings();
    void loadSettings();
    void show();
    QString getExtension();
    QString getGroupExtension();

public slots:
    void checkExten();
    void checkGroupExten();

protected:
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onEditButtonClicked();
    void onAddGroupButtonClicked();
    void onRemoveGroupButtonClicked();
    void onEditGroupButtonClicked();
    void applySettings();
    void selectLanguage();
    void loadLanguages();

protected slots:
    void on_applyButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::SettingsDialog *ui;
    QTcpSocket *m_tcpSocket;
    AddExtensionDialog *m_addExtensionDialog;
    QFile f = nullptr;
    QString userName;
    QString path;
    QString exten;
    QString group_exten;
    QStringList m_countries;
};

#endif // SETTINGSDIALOG_H
