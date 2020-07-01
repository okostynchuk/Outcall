#include "RemindersThread.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QSqlQuery>

RemindersThread::RemindersThread(QString receivedNumber, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes)
{
    my_number = receivedNumber;
    dateTimes = receivedDateTimes;
    notes = receivedNotes;
}

RemindersThread::~RemindersThread()
{

}

void RemindersThread::receiveNewValues(QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes)
{
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
                QSqlDatabase db;
                QSqlQuery query(db);

                query.prepare("UPDATE reminders SET active = false WHERE phone = ? AND datetime = ? AND content = ?");
                query.addBindValue(my_number);
                query.addBindValue(dateTimes.at(i));
                query.addBindValue(notes.at(i));
                query.exec();

                emit notify(notes.at(i));

                dateTimes.removeAt(i);
                notes.removeAt(i);
            }

            QThread::currentThread()->msleep(10);
        }
    }
}

