#include "RemindersDialog.h"
#include "ui_RemindersDialog.h"
#include "RemindersThread.h"

#include <QDebug>
#include <QThread>

RemindersDialog::RemindersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemindersDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    ui->calendarWidget->setGridVisible(true);
    ui->timeEdit->setTime(QTime::currentTime());

    connect(ui->saveButton, &QAbstractButton::clicked, this, &RemindersDialog::onSave);
}

RemindersDialog::~RemindersDialog()
{
    delete ui;
}

void RemindersDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);

    QThread* thread = new QThread;
    RemindersThread* worker = new RemindersThread();
    worker->moveToThread(thread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();


    QDateTime dateTime = QDateTime::QDateTime(date, time);
    QString note = ui->textEdit->toPlainText();
//    qDebug() << date;
//    qDebug() << time;
//    qDebug() << note;
//    qDebug() << dateTime;
}
