/*
 * Класс служит для объявления и инициализации глобальных переменных и настроек.
 */

#include "Global.h"

#include <Windows.h>
#include <TCHAR.H>
#include <shlobj.h>

#include <QSettings>

QString g_AppDirPath = "";
QString g_AppSettingsFolderPath = "";
QString g_CurrentAppVersion = APP_VERSION;

bool opened = false;
bool MSSQLopened = false;

QMessageBox::StandardButton MsgBoxInformation(const QString &text, QMessageBox::StandardButtons buttons, const QString &title, QWidget *parent, QMessageBox::StandardButton defaultButton)
{
    return QMessageBox::information(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MsgBoxError(const QString &text, QMessageBox::StandardButtons buttons, const QString &title, QWidget *parent, QMessageBox::StandardButton defaultButton)
{
    return QMessageBox::critical(parent, title, text, buttons, defaultButton);
}

QMessageBox::StandardButton MsgBoxWarning(const QString &text, QMessageBox::StandardButtons buttons, const QString &title, QWidget *parent, QMessageBox::StandardButton defaultButton)
{
    return QMessageBox::warning(parent, title, text, buttons, defaultButton);
}

/**
 * Выполняет установку настройки в реестре.
 */
void global::setSettingsValue(QString key, QVariant value, QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    settings.setValue(key, value);
}

/**
 * Возвращает настройку из реестра.
 */
QVariant global::getSettingsValue(const QString key, const QString group, const QVariant defaultValue)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    return settings.value(key, defaultValue);
}

/**
 * Выполняет удаление ключа настройки в реестре.
 */
void global::removeSettingsKey(const QString key, const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    settings.remove(key);
}

/**
 * Выполняет проверку на наличие ключа настройки в реестре.
 */
bool global::containsSettingsKey(const QString key, const QString group)
{
    if (key.isEmpty())
        return false;

    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    return settings.contains(key);
}

/**
 * Возвращает ключи настройки из реестра.
 */
QStringList global::getSettingKeys(const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    return settings.childKeys();
}

/**
 * Возвращает номер пользователя из реестра.
 */
QString global::getExtensionNumber(const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    if (!settings.childKeys().isEmpty())
        return settings.childKeys().first();
    else
        return NULL;
}

/**
 * Возвращает номер группы из реестра.
 */
QString global::getGroupExtensionNumber(const QString group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    if (!settings.childKeys().isEmpty())
        return settings.childKeys().first();
    else
        return NULL;
}
