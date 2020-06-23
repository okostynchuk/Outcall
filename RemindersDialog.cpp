#include "RemindersDialog.h"
#include "ui_RemindersDialog.h"

#include <QDebug>

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
    connect(this, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    while (!this->isActiveWindow())
        emit sendData(true);
}

RemindersDialog::~RemindersDialog()
{
    delete ui;
}

void RemindersDialog::receiveData(bool opa)
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime::QDateTime(date, time);
    if (opa)
    {
//        if (QDateTime::currentDateTime() != dateTime)
//        {
//            qDebug("123");
//            emit sendData(true);
//        }
        if (time > QTime::currentTime())
        {
            qDebug("123");
            emit sendData(true);
        }
    }
}

void RemindersDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime::QDateTime(date, time);
    QString note = ui->textEdit->toPlainText();
//    qDebug() << date;
//    qDebug() << time;
//    qDebug() << note;
//    qDebug() << dateTime;
}
