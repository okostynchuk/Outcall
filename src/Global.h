#ifndef GLOBAL_H
#define GLOBAL_H

#include "AsteriskManager.h"

#include <QVariant>
#include <QMessageBox>
#include <QSqlDatabase>

#define APP_NAME            "Outcall"
#define ORGANIZATION_NAME   "Team Nonames"
#define APP_VERSION         "1.0.5"

extern AsteriskManager* g_asteriskManager;

extern bool g_mainDbsOpened;
extern bool g_ordersDbOpened;

extern const QString g_personalNumber;
extern const QString g_personalNumberName;
extern const QString g_groupNumber;
extern const QString g_language;
extern QStringList g_regionsList;

QString QueryStringGetGroups();

qint32 MsgBoxInformation(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

qint32 MsgBoxError(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

qint32 MsgBoxWarning(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

qint32 MsgBoxQuestion(const QString& text, const QString& title = APP_NAME, QWidget* parent = nullptr);

namespace global
{
    void getRegionsList();

    void setSettingsValue(const QString& key, const QVariant& value, const QString& group = "");

    QVariant getSettingsValue(const QString& key, const QString& group = "", const QVariant& value = QVariant());

    void removeSettingsKey(const QString& key, const QString& group = "");

    bool containsSettingsKey(const QString& key, const QString& group = "");

    QStringList getSettingKeys(const QString& group = "");

    QString getExtensionNumber(const QString& group = "");

    QString getGroupExtensionNumber(const QString& group = "");
}

#endif // GLOBAL_H
