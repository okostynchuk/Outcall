#ifndef REMINDERSTHREADMANAGER_H
#define REMINDERSTHREADMANAGER_H

#include "Global.h"

#include <QObject>
#include <QTime>
#include <QDate>

class RemindersThreadManager : public QObject
{
    Q_OBJECT

signals:
    void notify(QString id, QDateTime dateTime, QString note);
    void finished();

public slots:
    void process();
    void setValues(const QList<QString>& ids, const QList<QDateTime>& dateTimes, const QList<QString>& notes);

public:
    RemindersThreadManager(const QList<QString>& ids, const QList<QDateTime>& dateTimes, const QList<QString>& notes);
    ~RemindersThreadManager();

private:
    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QString my_number;
};

#endif // REMINDERSTHREADMANAGER_H
