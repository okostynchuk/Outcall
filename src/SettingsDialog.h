#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "AsteriskManager.h"
#include "QSimpleUpdater.h"

#include <QDialog>
#include <QFile>
#include <QTreeWidgetItem>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

signals:
    void restart(bool hide);

public:
    explicit SettingsDialog(QWidget* parent = 0);
    ~SettingsDialog();

private slots:
    void loadLanguages();
    void loadSettings();
    void saveSettings();
    void applySettings();

    void checkForUpdates();
    void checkAsteriskState(const AsteriskManager::AsteriskState& state);
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onEditButtonClicked();
    void onAddGroupButtonClicked();
    void onRemoveGroupButtonClicked();
    void onEditGroupButtonClicked();

    void on_applyButton_clicked();
    void on_cancelButton_clicked();

    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::SettingsDialog* ui;

    QSimpleUpdater* m_updater;

    QByteArray m_geometry;
};

#endif // SETTINGSDIALOG_H
