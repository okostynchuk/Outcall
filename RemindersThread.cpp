#include "RemindersThread.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>

RemindersThread::RemindersThread(QString receivedNumber, QList<QString> receivedIds, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes)
{
    my_number = receivedNumber;
    ids = receivedIds;
    dateTimes = receivedDateTimes;
    notes = receivedNotes;

    dbReminders = QSqlDatabase::addDatabase("QMYSQL", "Reminders");
    QString hostName_1 = global::getSettingsValue("hostName_1", "settings").toString();
    QString databaseName_1 = global::getSettingsValue("databaseName_1", "settings").toString();
    QString userName_1 = global::getSettingsValue("userName_1", "settings").toString();
    QByteArray password1 = global::getSettingsValue("password_1", "settings").toByteArray();
    QString password_1 = QString(QByteArray::fromBase64(password1));
    QString port_1 = global::getSettingsValue("port_1", "settings").toString();

    dbReminders.setHostName(hostName_1);
    dbReminders.setDatabaseName(databaseName_1);
    dbReminders.setUserName(userName_1);
    dbReminders.setPassword(password_1);
    dbReminders.setPort(port_1.toUInt());
    dbReminders.open();
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

                QSqlQuery query(dbReminders);

                query.prepare("UPDATE reminders SET active = false WHERE id = ? AND phone_to = ?");
                query.addBindValue(ids.at(i));
                query.addBindValue(my_number);
                query.exec();

                ids.removeAt(i);
                dateTimes.removeAt(i);
                notes.removeAt(i);
            }

            QThread::currentThread()->msleep(100);
        }
    }
}

