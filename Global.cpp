#include "Global.h"

#include <Windows.h>
#include <TCHAR.H>
#include <shlobj.h>

#include <QSettings>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QApplication>

QString g_AppDirPath = "";
QString g_AppSettingsFolderPath = "";
bool opened = false;
bool MSSQLopened = false;

QMessageBox::StandardButton MsgBoxInformation(const QString &text, QMessageBox::StandardButtons buttons, const QString &title, QWidget *parent,
        QMessageBox::StandardButton defaultButton) {
        return QMessageBox::information(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MsgBoxError(const QString &text, QMessageBox::StandardButtons buttons, const QString &title, QWidget *parent,
         QMessageBox::StandardButton defaultButton) {
         return QMessageBox::critical(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MsgBoxWarning(const QString &text, QMessageBox::StandardButtons buttons, const QString &title, QWidget *parent,
         QMessageBox::StandardButton defaultButton) {
         return QMessageBox::warning(parent, title, text, buttons, defaultButton);
}

void global::setSettingsValue(QString key, QVariant value, QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    if (!group.isEmpty())
        settings.beginGroup(group);
    settings.setValue(key, value);
}

QVariant global::getSettingsValue(const QString key, const QString group, const QVariant defaultValue)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    if (!group.isEmpty())
        settings.beginGroup(group);
    return settings.value(key, defaultValue);
}

void global::removeSettingsKey(const QString key, const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    if (!group.isEmpty())
        settings.beginGroup(group);
    settings.remove(key);
}

bool global::containsSettingsKey(const QString key, const QString group)
{
    if (key.isEmpty())
        return false;

    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    if (!group.isEmpty())
        settings.beginGroup(group);
    return settings.contains(key);
}

QStringList global::getSettingKeys(const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    if (!group.isEmpty())
        settings.beginGroup(group);
    return settings.childKeys();
}

QString global::getExtensionNumber(const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    if (!group.isEmpty())
        settings.beginGroup(group);
    return settings.childKeys().first();
}
