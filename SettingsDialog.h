#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFile>

#include <AddExtensionDialog.h>

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

public slots:
    void checkExten();

protected:
    void handleButtonBox(QAbstractButton * button);
    void okPressed();
    void applyPressed();
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onEditButtonClicked();
    void applySettings();

protected slots:
    void onComboBoxSelected();

private:
    Ui::SettingsDialog *ui;
    QTcpSocket *m_tcpSocket;
    AddExtensionDialog *m_addExtensionDialog;
    QFile f = nullptr;
    QString userName;
    QString path;
    QString exten;

};

#endif // SETTINGSDIALOG_H
