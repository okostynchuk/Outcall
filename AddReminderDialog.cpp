#include "AddReminderDialog.h"
#include "ui_AddReminderDialog.h"

#include <QSqlQuery>
#include <QDebug>

AddReminderDialog::AddReminderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddReminderDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    employee.append(my_number);

    ui->employee->setText(my_number);

    QString languages = global::getSettingsValue("language", "settings").toString();

    if (languages == "Русский (по умолчанию)")
        ui->calendarWidget->setLocale(QLocale::Russian);
    else if (languages == "Українська")
        ui->calendarWidget->setLocale(QLocale::Ukrainian);
    else if (languages == "English")
        ui->calendarWidget->setLocale(QLocale::English);

    ui->calendarWidget->setGridVisible(true);
    ui->calendarWidget->setMinimumDate(QDate::currentDate());
    ui->calendarWidget->setSelectedDate(QDate::currentDate());

    ui->timeEdit->setTime(QTime::currentTime());

    connect(ui->textEdit, &QTextEdit::textChanged, this, &AddReminderDialog::onTextChanged);
    connect(ui->saveButton, &QPushButton::clicked, this, &AddReminderDialog::onSave);
    connect(ui->chooseEmployeeButton, &QPushButton::clicked, this, &AddReminderDialog::onChooseEmployee);
}

AddReminderDialog::~AddReminderDialog()
{
    delete ui;
}

void AddReminderDialog::receiveEmployee(QStringList employee)
{
    this->employee = employee;

    if (employee.length() == 1)
        ui->employee->setText(employee.first());
    else
        ui->employee->setText(tr("Группа") + " (" + QString::number(employee.length()) + ")");
}

void AddReminderDialog::onChooseEmployee()
{
    if (!chooseEmployee.isNull())
        chooseEmployee.data()->close();

    chooseEmployee = new ChooseEmployee;
    chooseEmployee.data()->setValuesReminders(employee);
    connect(chooseEmployee.data(), &ChooseEmployee::sendEmployee, this, &AddReminderDialog::receiveEmployee);
    chooseEmployee.data()->show();
    chooseEmployee.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void AddReminderDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime(date, time);
    QString note = ui->textEdit->toPlainText().trimmed();

    if (dateTime < QDateTime::currentDateTime())
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Указано прошедшее время!"), QMessageBox::Ok);

        return;
    }

    if (note.isEmpty())
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Содержание напоминания не может быть пустым!"), QMessageBox::Ok);

        return;
    }

    QSqlDatabase db;
    QSqlQuery query(db);

    if (callId.isEmpty())
    {
        if (ui->employee->text() != my_number)
        {
            if (employee.length() == 1)
            {
                query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, false, false, true)");
                query.addBindValue(my_number);
                query.addBindValue(employee.first());
                query.addBindValue(dateTime);
                query.addBindValue(note);
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

                int id = query.lastInsertId().toInt();

                query.prepare("UPDATE reminders SET group_id = ? WHERE id = ?");
                query.addBindValue(id);
                query.addBindValue(id);
                query.exec();

                for (int i = 1; i < employee.length(); ++i)
                {
                    query.prepare("INSERT INTO reminders (group_id, phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
                    query.addBindValue(id);
                    query.addBindValue(my_number);
                    query.addBindValue(employee.at(i));
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.exec();
                }
            }
        }
        else
        {
            query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, active) VALUES(?, ?, ?, ?, true)");
            query.addBindValue(my_number);
            query.addBindValue(ui->employee->text());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.exec();
        }
    }
    else
    {
        if (ui->employee->text() != my_number)
        {
            if (employee.length() == 1)
            {
                query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, call_id, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
                query.addBindValue(my_number);
                query.addBindValue(employee.first());
                query.addBindValue(dateTime);
                query.addBindValue(note);
                query.addBindValue(callId);
                query.exec();
            }
            else
            {
                query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, call_id, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
                query.addBindValue(my_number);
                query.addBindValue(employee.first());
                query.addBindValue(dateTime);
                query.addBindValue(note);
                query.addBindValue(callId);
                query.exec();

                int id = query.lastInsertId().toInt();

                query.prepare("UPDATE reminders SET group_id = ? WHERE id = ?");
                query.addBindValue(id);
                query.addBindValue(id);
                query.exec();

                for (int i = 1; i < employee.length(); ++i)
                {
                    query.prepare("INSERT INTO reminders (group_id, phone_from, phone_to, datetime, content, call_id, viewed, completed, active) VALUES(?, ?, ?, ?, ?, ?, false, false, true)");
                    query.addBindValue(id);
                    query.addBindValue(my_number);
                    query.addBindValue(employee.at(i));
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(callId);
                    query.exec();
                }
            }
        }
        else
        {
            query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, call_id, active) VALUES(?, ?, ?, ?, ?, true)");
            query.addBindValue(my_number);
            query.addBindValue(ui->employee->text());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.addBindValue(callId);
            query.exec();
        }
    }

    if (!chooseEmployee.isNull())
        chooseEmployee.data()->close();

    emit sendData(true);

    close();

    if (ui->employee->text() != my_number)
        QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Напоминание успешно отправлено!"), QMessageBox::Ok);
    else
        QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Напоминание успешно добавлено!"), QMessageBox::Ok);
}

void AddReminderDialog::setCallId(QString receivedCallId)
{
    callId = receivedCallId;
}

void AddReminderDialog::onTextChanged()
{
    if (ui->textEdit->toPlainText().trimmed().length() > 255)
        ui->textEdit->textCursor().deletePreviousChar();
}

void AddReminderDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        if (ui->textEdit->hasFocus())
            return;
        else
            onSave();
    }
    else
        QDialog::keyPressEvent(event);
}

void AddReminderDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    if (!chooseEmployee.isNull())
        chooseEmployee.data()->close();
}

void AddReminderDialog::setEmployee(QString employee)
{
    ui->employee->setText(employee);

    this->employee.append(employee);
}

void AddReminderDialog::on_add5MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(300));
}

void AddReminderDialog::on_add10MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(600));
}

void AddReminderDialog::on_add30MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(1800));
}

void AddReminderDialog::on_add60MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(3600));
}
