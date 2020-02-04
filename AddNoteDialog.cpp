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
   // qDebug()<<uniqueid;
}

AddNoteDialog::~AddNoteDialog()
{
    delete ui;
}

void AddNoteDialog::setCallId(QString &uniqueid)
{
    callId = uniqueid;
}

void AddNoteDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("INSERT INTO calls (uniqueid, note)"
                  "VALUES(?, ?)");
    query.addBindValue(callId);
    query.addBindValue(ui->textEdit->toPlainText());
    query.exec();
    ui->label->setText("<span style=\"color: green;\">Запись успешно добавлена!</span>");
}
