#include "RemindersThread.h"

#include <QTime>
#include <QDate>
#include <QDebug>

//RemindersThread::RemindersThread(QObject *parent) : QObject(parent)
//{

//}

RemindersThread::RemindersThread()
{
    //connect(this, SIGNAL(sendData(bool, QString)), this, SLOT(receiveData(bool, QString)));
}

RemindersThread::~RemindersThread()
{

}

void RemindersThread::receiveData(bool opa, QString time)
{
    if (opa)
    {
        if (time > QTime::currentTime().toString())
        {
            emit sendData(true, time);
        }
        else
        {
            qDebug("123");
            emit finished();
        }
    }
}

void RemindersThread::process(QString time)
{
    //emit sendData(true, time);
}

