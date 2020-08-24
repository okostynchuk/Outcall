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

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->textEdit->installEventFilter(this);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->saveButton, &QPushButton::clicked, this, &EditReminderDialog::onSave);
    connect(ui->chooseEmployeeButton, &QPushButton::clicked, this, &EditReminderDialog::onChooseEmployee);
}

EditReminderDialog::~EditReminderDialog()
{
    delete ui;
}

void EditReminderDialog::receiveEmployee(QStringList employee)
{
    this->employee = employee;

    if (employee.length() == 1)
        ui->employee->setText(employee.first());
    else
        ui->employee->setText(tr("Группа") + " (" + QString::number(employee.length()) + ")");
}

void EditReminderDialog::onChooseEmployee()
{
    if (employee.isEmpty())
        employee = employeeInitial;

    chooseEmployee = new ChooseEmployee;
    chooseEmployee->setValuesReminders(employee);
    connect(chooseEmployee, SIGNAL(sendEmployee(QStringList)), this, SLOT(receiveEmployee(QStringList)));
    chooseEmployee->show();
    chooseEmployee->setAttribute(Qt::WA_DeleteOnClose);
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

    if (employee.isEmpty())
        employee = employeeInitial;

    QSqlDatabase db;
    QSqlQuery query(db);

    if (!ui->chooseEmployeeButton->isEnabled())
    {
        if (group_id == "0" || (group_id != "0" && dateTime == oldDateTime))
        {
            query.prepare("UPDATE reminders SET datetime = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(dateTime);
            query.addBindValue(id);
            query.exec();
        }
        else if (group_id != "0" && dateTime != oldDateTime)
        {
            query.prepare("UPDATE reminders SET group_id = NULL, datetime = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(dateTime);
            query.addBindValue(id);
            query.exec();
        }
    }
    else
    {
        if (employeeInitial.length() == 1 && employee.length() > 1)
        {
            query.prepare("UPDATE reminders SET group_id = ? WHERE id = ?");
            query.addBindValue(id);
            query.addBindValue(id);
            query.exec();

            query.prepare("SELECT group_id FROM reminders WHERE id = ?");
            query.addBindValue(id);
            query.exec();
            query.first();

            group_id = query.value(0).toString();
        }

        if (ui->employee->text() != my_number)
        {
            if (employee == employeeInitial)
            {
                if (employee.length() == 1)
                {
                    query.prepare("UPDATE reminders SET datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE id = ?");
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(id);
                    query.exec();
                }
                else
                {
                    for (int i = 0; i < employee.length(); i++)
                    {
                        query.prepare("UPDATE reminders SET datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE phone_to = ? AND group_id = ?");
                        query.addBindValue(dateTime);
                        query.addBindValue(note);
                        query.addBindValue(employee.at(i));
                        query.addBindValue(group_id);
                        query.exec();
                    }
                }
            }
            else
            {
                for (int i = 0; i < employeeInitial.length(); ++i)
                {
                    if (employee.contains(employeeInitial.at(i)))
                        continue;

                    query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE phone_to = ? AND group_id = ?");
                    query.addBindValue(employeeInitial.at(i));
                    query.addBindValue(group_id);
                    query.exec();
                }

                if (employee.length() == 1)
                {
                    if (employeeInitial.contains(employee.first()))
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, phone_to = ?, datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE group_id = ?");
                        query.addBindValue(employee.first());
                        query.addBindValue(dateTime);
                        query.addBindValue(note);
                        query.addBindValue(group_id);
                        query.exec();
                    }
                    else
                    {
                        query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, false, false, true)");
                        query.addBindValue(my_number);
                        query.addBindValue(employee.first());
                        query.addBindValue(dateTime);
                        query.addBindValue(note);
                        query.exec();
                    }
                }
                else
                {
                    for (int i = 0; i < employee.length(); ++i)
                    {
                        if (employeeInitial.contains(employee.at(i)))
                        {
                            query.prepare("UPDATE reminders SET datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE phone_to = ? AND group_id = ?");
                            query.addBindValue(dateTime);
                            query.addBindValue(note);
                            query.addBindValue(employee.at(i));
                            query.addBindValue(group_id);
                            query.exec();
                        }
                        else
                        {
                            query.prepare("INSERT INTO reminders (group_id, phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
                            query.addBindValue(group_id);
                            query.addBindValue(my_number);
                            query.addBindValue(employee.at(i));
                            query.addBindValue(dateTime);
                            query.addBindValue(note);
                            query.exec();
                        }
                    }
                }
            }
        }
        else
        {
            if (employee == employeeInitial)
            {
                query.prepare("UPDATE reminders SET datetime = ?, content = ?, completed = false, active = true WHERE id = ?");
                query.addBindValue(dateTime);
                query.addBindValue(note);
                query.addBindValue(id);
                query.exec();
            }
            else
            {
                for (int i = 0; i < employeeInitial.length(); ++i)
                {
                    query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE phone_to = ? AND group_id = ?");
                    query.addBindValue(employeeInitial.at(i));
                    query.addBindValue(group_id);
                    query.exec();
                }

                if (employeeInitial.contains(employee.first()))
                {
                    query.prepare("UPDATE reminders SET phone_to = ?, datetime = ?, content = ?, completed = false, active = true WHERE id = ?");
                    query.addBindValue(employee.first());
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(id);
                    query.exec();
                }
                else
                {
                    query.prepare("INSERT INTO reminders (phone_to, datetime, content, completed, active) VALUES(?, ?, ?, false, true)");
                    query.addBindValue(employee.first());
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(id);
                    query.exec();
                }
            }
        }
    }

    emit sendData(true);

    close();

    QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Напоминание успешно изменено!"), QMessageBox::Ok);
}

void EditReminderDialog::setValuesReminders(QString receivedId, QString receivedGroupId, QDateTime receivedDateTime, QString receivedNote)
{
    id = receivedId;
    group_id = receivedGroupId;
    oldDateTime = receivedDateTime;
    oldNote = receivedNote;

    ui->dateTimeEdit->setDateTime(oldDateTime);
    ui->textEdit->setText(oldNote);

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);

    QSqlDatabase db;
    QSqlQuery query(db);

    if (group_id == "0")
    {
        query.prepare("SELECT phone_from, phone_to FROM reminders WHERE id = ?");
        query.addBindValue(id);
        query.exec();
        query.first();

        QString phone_from = query.value(0).toString();
        QString phone_to = query.value(1).toString();

        employeeInitial.append(phone_to);

        ui->employee->setText(phone_to);

        if (phone_from != phone_to && phone_from != my_number)
        {
            ui->chooseEmployeeButton->setDisabled(true);

            ui->textEdit->setReadOnly(true);
            ui->textEdit->setStyleSheet("QTextEdit {background-color: #fffff0;}");
        }
    }
    else
    {
        query.prepare("SELECT phone_to FROM reminders WHERE group_id = ?");
        query.addBindValue(group_id);
        query.exec();

        while (query.next())
            employeeInitial.append(query.value(0).toString());

        ui->employee->setText(tr("Группа") + " (" + QString::number(employeeInitial.length()) + ")");
    }
}

void EditReminderDialog::onTextChanged()
{
    if (ui->textEdit->toPlainText().simplified().length() > 255)
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
    if (event->key() == Qt::Key_Escape)
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
