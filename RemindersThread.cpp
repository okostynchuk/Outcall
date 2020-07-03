#include "RemindersThread.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QSqlQuery>

RemindersThread::RemindersThread(QString receivedNumber, QList<QString> receivedIds, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes)
{
    my_number = receivedNumber;
    ids = receivedIds;
    dateTimes = receivedDateTimes;
    notes = receivedNotes;
}

RemindersThread::~RemindersThread()
{

}

void RemindersThread::receiveNewValues(QList<QString> receivedIds, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes)
{
    ids = receivedIds;
    dateTimes = receivedDateTimes;
    notes = receivedNotes;
}

void RemindersThread::process()
{
    forever
    {
        if (QThread::currentThread()->isInterruptionRequested())
        {
            emit finished();
            return;
        }

        for (int i = 0; i < dateTimes.count(); i++)
        {
            if (dateTimes.at(i) <= QDateTime::currentDateTime())
            {
                emit notify(ids.at(i), dateTimes.at(i), notes.at(i));

                QSqlDatabase db;
                QSqlQuery query(db);

                query.prepare("UPDATE reminders SET active = false WHERE id = ? AND phone = ? AND datetime = ? AND content = ?");
                query.addBindValue(ids.at(i));
                query.addBindValue(my_number);
                query.addBindValue(dateTimes.at(i));
                query.addBindValue(notes.at(i));
                query.exec();

                ids.removeAt(i);
                dateTimes.removeAt(i);
                notes.removeAt(i);
            }

            QThread::currentThread()->msleep(100);
        }
    }
}

