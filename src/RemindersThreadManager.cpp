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

    m_ids = ids;
    m_dateTimes = dateTimes;
    m_notes = notes;
}

RemindersThreadManager::~RemindersThreadManager()
{}

/**
 * Получает данные актуальных напоминаний из класса RemindersDialog.
 */
void RemindersThreadManager::setValues(const QList<QString>& ids, const QList<QDateTime>& dateTimes, const QList<QString>& notes)
{
    m_ids = ids;
    m_dateTimes = dateTimes;
    m_notes = notes;
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

        for (qint32 i = 0; i < m_dateTimes.count(); ++i)
        {
            if (m_dateTimes.at(i) <= QDateTime::currentDateTime())
            {
                emit notify(m_ids.at(i), m_dateTimes.at(i), m_notes.at(i));

                m_ids.removeAt(i);
                m_dateTimes.removeAt(i);
                m_notes.removeAt(i);
            }
        }

        QThread::currentThread()->msleep(100);
    }
}
