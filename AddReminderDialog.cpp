#include "AddReminderDialog.h"
#include "ui_AddReminderDialog.h"

#include <QKeyEvent>
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

    ui->textEdit->installEventFilter(this);

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->saveButton, &QPushButton::clicked, this, &AddReminderDialog::onSave);
    connect(ui->chooseEmployeeButton, &QPushButton::clicked, this, &AddReminderDialog::onChooseEmployee);

    ui->calendarWidget->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
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
    chooseEmployee = new ChooseEmployee;
    chooseEmployee->setValuesReminders(employee);
    connect(chooseEmployee, SIGNAL(sendEmployee(QStringList)), this, SLOT(receiveEmployee(QStringList)));
    chooseEmployee->show();
    chooseEmployee->setAttribute(Qt::WA_DeleteOnClose);
}

void AddReminderDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime(date, time);
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
    if (ui->textEdit->toPlainText().simplified().length() > 255)
        ui->textEdit->textCursor().deletePreviousChar();
}

bool AddReminderDialog::eventFilter(QObject *object, QEvent *event)
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
