#ifndef GLOBAL_H
#define GLOBAL_H

#define APP_NAME            "Outcall"
#define ORGANIZATION_NAME   "Team Nonames"
#define APP_VERSION         "1.0.0"

#include <QVariant>
#include <QMessageBox>

extern QString g_appDirPath;
extern QString g_appSettingsFolderPath;

extern bool g_dbsOpened;
extern bool g_ordersDbOpened;

int MsgBoxInformation(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

int MsgBoxError(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

int MsgBoxWarning(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

int MsgBoxQuestion(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

namespace global
{
    void setSettingsValue(const QString& key, const QVariant& value, const QString& group = "");

    QVariant getSettingsValue(const QString& key, const QString& group = "", const QVariant& value = QVariant());

    void removeSettingsKey(const QString& key, const QString& group = "");

    bool containsSettingsKey(const QString& key, const QString& group = "");

    QStringList getSettingKeys(const QString& group = "");

    QString getExtensionNumber(const QString& group = "");

    QString getGroupExtensionNumber(const QString& group = "");
}

#endif // GLOBAL_H
