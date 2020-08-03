#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QFile>

class QTcpSocket;
class AddExtensionDialog;
class QTreeWidgetItem;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

signals:
    void restart(bool);

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

private slots:
    void closeEvent(QCloseEvent *event);

private:
    Ui::SettingsDialog *ui;
    QTcpSocket *m_tcpSocket;
    AddExtensionDialog *m_addExtensionDialog;
    QFile f;
    QString userName;
    QString path;
    QString exten;
    QString group_exten;
    QStringList m_countries;
};

#endif // SETTINGSDIALOG_H
