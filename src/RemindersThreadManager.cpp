/*
 * Класс служит для отлавливания момента,
 * когда должно появиться окно напоминания.
 */

#include "RemindersThreadManager.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>

RemindersThreadManager::RemindersThreadManager(const QList<QString>& ids, const QList<QDateTime>& dateTimes, const QList<QString>& notes)
{
    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    this->ids = ids;
    this->dateTimes = dateTimes;
    this->notes = notes;
}

RemindersThreadManager::~RemindersThreadManager()
{}

/**
 * Получает данные актуальных напоминаний из класса RemindersDialog.
 */
void RemindersThreadManager::setValues(const QList<QString>& ids, const QList<QDateTime>& dateTimes, const QList<QString>& notes)
{
    this->ids = ids;
    this->dateTimes = dateTimes;
    this->notes = notes;
}

/**
 * Выполняет сравнения текущего времени с временем актуальных напоминаний.
 * Когда приходит время напоминания, происходит отправка его данных в класс RemindersDialog.
 */
void RemindersThreadManager::process()
{
    forever
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            emit finished();

            return;
        }

        for (qint32 i = 0; i < dateTimes.count(); ++i)
        {
            if (dateTimes.at(i) <= QDateTime::currentDateTime())
            {
                emit notify(ids.at(i), dateTimes.at(i), notes.at(i));

                ids.removeAt(i);
                dateTimes.removeAt(i);
                notes.removeAt(i);
            }
        }

        QThread::currentThread()->msleep(100);
    }
}
