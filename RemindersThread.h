#ifndef REMINDERSTHREAD_H
#define REMINDERSTHREAD_H

#include <QObject>

class RemindersThread : public QObject
{
    Q_OBJECT

public:
    //explicit RemindersThread(QObject *parent = 0);
    RemindersThread();
    ~RemindersThread();

public slots:
    void process(QString);
    void receiveData(bool, QString);

signals:
    void finished();
    void sendData(bool, QString);

private:

};

#endif // REMINDERSTHREAD_H
