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
    QString note = ui->textEdit->toPlainText().simplified();

    if (dateTime < QDateTime::currentDateTime())
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Указано прошедшее время!"), QMessageBox::Ok);
        return;
    }

    if (ui->textEdit->toPlainText().simplified().isEmpty())
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Содержание напоминания не может быть пустым!"), QMessageBox::Ok);
        return;
    }

    QSqlDatabase db;
    QSqlQuery query(db);

    QRegExp reg("([0-9]+)(.+)");
    reg.indexIn(ui->comboBox->currentText());

    query.prepare("UPDATE reminders SET phone_from = ?, phone_to = ?, datetime = ?, content = ?, active = true WHERE id = ?");
    query.addBindValue(my_number);
    query.addBindValue(reg.cap(1));
    query.addBindValue(dateTime);
    query.addBindValue(note);
    query.addBindValue(id);;
    query.exec();

    emit sendData(true);
    close();

    if (reg.cap(1) != my_number)
        QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Напоминание успешно отправлено!"), QMessageBox::Ok);
    else
        QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Напоминание успешно изменено!"), QMessageBox::Ok);

    destroy(true);
}

void EditReminderDialog::setValuesReminders(QString receivedNumber, QString receivedSelectedNumber, QString receivedId, QDateTime receivedDateTime, QString receivedNote)
{
    my_number = receivedNumber;
    selectedNumber = receivedSelectedNumber;
    id = receivedId;
    oldDateTime = receivedDateTime;
    oldNote = receivedNote;

    ui->dateTimeEdit->setDateTime(oldDateTime);
    ui->textEdit->setText(oldNote);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);

    ui->comboBox->addItem(g_pAsteriskManager->extensionNumbers.value(selectedNumber));
    ui->comboBox->addItems(g_pAsteriskManager->extensionNumbers.values());
}

void EditReminderDialog::onTextChanged()
{
    if(ui->textEdit->toPlainText().simplified().length() > 255)
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
