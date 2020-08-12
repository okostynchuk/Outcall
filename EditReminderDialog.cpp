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

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->saveButton, &QPushButton::clicked, this, &EditReminderDialog::onSave);
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
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Указано прошедшее время!"), QMessageBox::Ok);

        return;
    }

    if (ui->textEdit->toPlainText().simplified().isEmpty())
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Содержание напоминания не может быть пустым!"), QMessageBox::Ok);

        return;
    }

    QSqlDatabase db;
    QSqlQuery query(db);

    if (!ui->comboBox->isEnabled())
    {
        query.prepare("UPDATE reminders SET datetime = ?, completed = false, active = true WHERE id = ?");
        query.addBindValue(dateTime);
        query.addBindValue(id);
        query.exec();
    }
    else
    {
        if (ui->comboBox->currentText() != my_number)
        {
            query.prepare("UPDATE reminders SET phone_to = ?, datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE id = ?");
            query.addBindValue(ui->comboBox->currentText());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.addBindValue(id);
            query.exec();
        }
        else
        {
            query.prepare("UPDATE reminders SET phone_to = ?, datetime = ?, content = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(my_number);
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.addBindValue(id);
            query.exec();
        }
    }

    emit sendData(true);

    close();

    QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Напоминание успешно изменено!"), QMessageBox::Ok);
}

void EditReminderDialog::setValuesReminders(QString receivedId, QDateTime receivedDateTime, QString receivedNote)
{
    id = receivedId;
    oldDateTime = receivedDateTime;
    oldNote = receivedNote;

    ui->dateTimeEdit->setDateTime(oldDateTime);
    ui->textEdit->setText(oldNote);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT phone_from, phone_to FROM reminders WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    query.first();

    phone_from = query.value(0).toString();
    phone_to = query.value(1).toString();

    ui->comboBox->addItem(phone_to);
    ui->comboBox->addItems(g_pAsteriskManager->extensionNumbers.values());

    if (phone_from != phone_to && phone_from != my_number)
    {
        ui->comboBox->setDisabled(true);
        ui->textEdit->setReadOnly(true);
    }
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

void EditReminderDialog::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);

    emit sendData(false);
}

void EditReminderDialog::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        emit sendData(false);

        QDialog::close();
    }
    else
        QWidget::keyPressEvent(event);
}

void EditReminderDialog::on_add5MinButton_clicked()
{
    ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(300));
}

void EditReminderDialog::on_add10MinButton_clicked()
{
     ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(600));
}

void EditReminderDialog::on_add30MinButton_clicked()
{
     ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(1800));
}

void EditReminderDialog::on_add60MinButton_clicked()
{
     ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(3600));
}
