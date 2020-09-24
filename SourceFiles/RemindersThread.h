#ifndef REMINDERSTHREAD_H
#define REMINDERSTHREAD_H

#include "Global.h"

#include <QObject>
#include <QTime>
#include <QDate>

class RemindersThread : public QObject
{
    Q_OBJECT

signals:
    void notify(QString, QDateTime, QString);
    void finished();

public slots:
    void process();
    void receiveNewValues(QList<QString> receivedIds, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes);

public:
    RemindersThread(QString receivedNumber, QList<QString> receivedIds, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes);
    ~RemindersThread();

private:
    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QString my_number;
};

#endif // REMINDERSTHREAD_H
