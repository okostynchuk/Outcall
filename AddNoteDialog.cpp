#include "AddNoteDialog.h"
#include "ui_AddNoteDialog.h"
#include "CallHistoryDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

AddNoteDialog::AddNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNoteDialog)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddNoteDialog::onSave);

}

AddNoteDialog::~AddNoteDialog()
{
    delete ui;
}

void AddNoteDialog::setCallId(QString &uniqueid, QString &state_call)
{
    callId = uniqueid;
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT note FROM calls WHERE uniqueid = "+callId);
    query.exec();
    query.first();
    QString note = query.value(0).toString();
    ui->textEdit->setText(note);
    state = state_call;
}

void AddNoteDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("INSERT  INTO calls (uniqueid, note) VALUES(?, ?) ON DUPLICATE KEY UPDATE note = ?");
    query.addBindValue(callId);
    query.addBindValue(ui->textEdit->toPlainText());
    query.addBindValue(ui->textEdit->toPlainText());
    query.exec();
    ui->label->setText("<span style=\"color: green;\">Запись успешно добавлена!</span>");
    if(state == "missed")
    {
       emit sendDataToMissed();
    }
    if(state == "received")
    {
       emit sendDataToReceived();
    }
    if(state == "placed")
    {
        emit sendDataToPlaced();
    }
}
