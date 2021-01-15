/*
 * Класс служит для объявления и инициализации глобальных переменных и настроек.
 */

#include "Global.h"

#include <QSettings>

AsteriskManager* g_asteriskManager = nullptr;

bool g_mainDbsOpened = false;
bool g_ordersDbOpened = false;

const QString g_personalNumber = global::getExtensionNumber("extensions");
const QString g_personalNumberName = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();
const QString g_groupNumber = global::getGroupExtensionNumber("group_extensions");

QString QueryStringGetGroups()
{
    QString language = global::getSettingsValue("language", "settings").toString();

    QString queryString;

    if (language == "Русский")
        queryString = "SELECT number, name_ru FROM groups";
    else if (language == "Українська")
        queryString = "SELECT number, name_ukr FROM groups";
    else if (language == "English")
        queryString = "SELECT number, name_en FROM groups";

    return queryString;
}

/**
 * Возвращает окно сообщения с информацией.
 */
qint32 MsgBoxInformation(const QString& text, const QString& title, QWidget* parent)
{
    QMessageBox msgBox;
    msgBox.setParent(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/images/information.png"));
    return msgBox.exec();
}

/**
 * Возвращает окно сообщения с ошибкой.
 */
qint32 MsgBoxError(const QString& text, const QString& title, QWidget* parent)
{
    QMessageBox msgBox;
    msgBox.setParent(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/images/error.png"));
    return msgBox.exec();
}

/**
 * Возвращает окно сообщения с предупреждением.
 */
qint32 MsgBoxWarning(const QString& text, const QString& title, QWidget* parent)
{
    QMessageBox msgBox;
    msgBox.setParent(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
    return msgBox.exec();
}

/**
 * Возвращает окно сообщения с вопросом.
 */
qint32 MsgBoxQuestion(const QString& text, const QString& title, QWidget* parent)
{
    QMessageBox msgBox;
    msgBox.setParent(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIconPixmap(QPixmap(":/images/question.png"));
    return msgBox.exec();
}

/**
 * Выполняет установку настройки в реестре.
 */
void global::setSettingsValue(const QString& key, const QVariant& value, const QString& group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    settings.setValue(key, value);
}

/**
 * Возвращает настройку из реестра.
 */
QVariant global::getSettingsValue(const QString& key, const QString& group, const QVariant& defaultValue)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    return settings.value(key, defaultValue);
}

/**
 * Выполняет удаление ключа настройки в реестре.
 */
void global::removeSettingsKey(const QString& key, const QString& group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    settings.remove(key);
}

/**
 * Выполняет проверку на наличие ключа настройки в реестре.
 */
bool global::containsSettingsKey(const QString& key, const QString& group)
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
QStringList global::getSettingKeys(const QString& group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    return settings.childKeys();
}

/**
 * Возвращает номер пользователя из реестра.
 */
QString global::getExtensionNumber(const QString& group)
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
QString global::getGroupExtensionNumber(const QString& group)
{
    QSettings settings(ORGANIZATION_NAME, APP_NAME);

    if (!group.isEmpty())
        settings.beginGroup(group);

    if (!settings.childKeys().isEmpty())
        return settings.childKeys().first();
    else
        return NULL;
}
