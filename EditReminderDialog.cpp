#include "EditReminderDialog.h"
#include "ui_EditReminderDialog.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>

EditReminderDialog::EditReminderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditReminderDialog)
{
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditReminderDialog::onSave);
}

EditReminderDialog::~EditReminderDialog()
{
    delete ui;
}

void EditReminderDialog::onSave()
{
    QDateTime dateTime = ui->dateTimeEdit->dateTime();
    QString note = ui->textEdit->toPlainText();

    if (dateTime < QDateTime::currentDateTime())
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Указано прошедшее время!"), QMessageBox::Ok);
        return;
    }

    if (ui->textEdit->toPlainText().isEmpty())
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Содержание напоминания не может быть пустым!"), QMessageBox::Ok);
        return;
    }

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT EXISTS (SELECT datetime FROM reminders WHERE phone = '" + my_number + "' AND datetime = ? AND datetime <> ? AND active IS TRUE)");
    query.addBindValue(dateTime);
    query.addBindValue(oldDateTime);
    query.exec();
    query.next();
    if (query.value(0) != 0)
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Напоминание на заданное время уже существует и активно!"), QMessageBox::Ok);
        return;
    }

    query.prepare("UPDATE reminders SET datetime = ?, content = ?, active = true WHERE id = ? AND phone = ? AND datetime = ? AND content = ?");
    query.addBindValue(dateTime);
    query.addBindValue(note);
    query.addBindValue(id);
    query.addBindValue(my_number);
    query.addBindValue(oldDateTime);
    query.addBindValue(oldNote);
    query.exec();

    emit sendData(true);
    close();
    QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Напоминание успешно изменено!"), QMessageBox::Ok);
    destroy(true);
}

void EditReminderDialog::setValuesReminders(QString receivedNumber, QString receivedId, QDateTime receivedDateTime, QString receivedNote)
{
    my_number = receivedNumber;
    id = receivedId;
    oldDateTime = receivedDateTime;
    oldNote = receivedNote;

    ui->dateTimeEdit->setDateTime(oldDateTime);
    ui->textEdit->setText(oldNote);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
}

void EditReminderDialog::onTextChanged()
{
    if(ui->textEdit->toPlainText().length() > 255)
        ui->textEdit->textCursor().deletePreviousChar();
}

bool EditReminderDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object->objectName() == "textEdit")
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return)
            {
                object->setObjectName("textEdit2");
                return true;
            }
        }
    }
    else if (object->objectName() == "textEdit2")
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return)
            {
                object->setObjectName("textEdit");
                return true;
            }
        }
    }
    return false;
}
