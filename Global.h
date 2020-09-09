#ifndef GLOBAL_H
#define GLOBAL_H

#define LOG_ERROR				0
#define LOG_WARNING				1
#define LOG_INFORMATION			2
#define APP_NAME                "OutCALL"
#define ORGANIZATION_NAME       "Team Nonames"
#define APP_VERSION             "3.0.2"

#include <QVariant>
#include <QMessageBox>

extern QString g_AppDirPath;
extern QString g_AppSettingsFolderPath;
extern QString g_CurrentAppVersion;
extern bool opened;
extern bool MSSQLopened;

QMessageBox::StandardButton MsgBoxInformation(const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                     const QString &title = APP_NAME, QWidget *parent = 0,
                                                     QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton MsgBoxError(const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                     const QString &title = APP_NAME, QWidget *parent = 0,
                                                     QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

QMessageBox::StandardButton MsgBoxWarning(const QString &text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                                                     const QString &title = APP_NAME, QWidget *parent = 0,
                                                     QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

namespace global
{
    void setSettingsValue(const QString key, const QVariant value, const QString group = "");

    QVariant getSettingsValue(const QString key, const QString group = "", const QVariant value = QVariant());

    void removeSettingsKey(const QString key, const QString group = "");

    bool containsSettingsKey(const QString key, const QString group = "");

    QStringList getSettingKeys(const QString group);

    QString getExtensionNumber(const QString group);

    QString getGroupExtensionNumber(const QString group);
}

#endif // GLOBAL_H
