#ifndef REMINDERSTHREAD_H
#define REMINDERSTHREAD_H

#include <QObject>
#include <QTime>
#include <QDate>

class RemindersThread : public QObject
{
    Q_OBJECT

public:
    RemindersThread(QString receivedNumber, QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes);
    ~RemindersThread();

public slots:
    void process();
    void receiveNewValues(QList<QDateTime> receivedDateTimes, QList<QString> receivedNotes);

signals:
    void notify(QString);
    void finished();

private:
    QString my_number;
    QList<QDateTime> dateTimes;
    QList<QString> notes;
};

#endif // REMINDERSTHREAD_H
