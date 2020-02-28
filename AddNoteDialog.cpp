#include "AddNoteDialog.h"
#include "ui_AddNoteDialog.h"
#include "CallHistoryDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>

AddNoteDialog::AddNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNoteDialog)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddNoteDialog::onSave);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

}

AddNoteDialog::~AddNoteDialog()
{
    delete ui;
}

void AddNoteDialog::setCallId(QString &uniqueid, QString &state_call)
{
    QString note;
    callId = uniqueid;
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT note FROM calls WHERE uniqueid = "+callId);
    query.exec();
    while(query.next())
        note= query.value(0).toString();
    QTextCursor  cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setText(note);
    state = state_call;
}

void AddNoteDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT EXISTS(SELECT uniqueid FROM calls WHERE uniqueid = '"+callId+"')");
    query.exec();
    query.first();

    if(ui->textEdit->toPlainText().isEmpty() && query.value(0) == 0)
        return;

    query.prepare("INSERT  INTO calls (uniqueid, note) VALUES(?, ?) ON DUPLICATE KEY UPDATE note = ?");
    query.addBindValue(callId);
    query.addBindValue(ui->textEdit->toPlainText());
    query.addBindValue(ui->textEdit->toPlainText());
    query.exec();

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
    close();
    QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Заметка успешно добавлена!"), QMessageBox::Ok);
    destroy(true);
}
