#include "AddNoteDialog.h"
#include "ui_AddNoteDialog.h"

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

void AddNoteDialog::setCallId(QString &uniqueid, QString &state)
{
    callState = state;
    callId = uniqueid;
}

void AddNoteDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    if(callState == "add")
    {
        query.prepare("INSERT INTO calls (uniqueid, note)"
                      "VALUES(?, ?)");
        query.addBindValue(callId);
        query.addBindValue(ui->textEdit->toPlainText());
        query.exec();
        ui->label->setText("<span style=\"color: green;\">Запись успешно добавлена!</span>");
    }
    else if (callState == "edit")
    {
        query.prepare("UPDATE calls SET note = ? WHERE uniqueid = ?");
        query.addBindValue(ui->textEdit->toPlainText());
        query.addBindValue(callId);
        query.exec();
        ui->label->setText("<span style=\"color: green;\">Запись успешно изменена!</span>");
    }

}
