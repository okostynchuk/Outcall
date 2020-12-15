/*
 * Класс служит для изменения существующего напоминания.
 */

#include "EditReminderDialog.h"
#include "ui_EditReminderDialog.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>

EditReminderDialog::EditReminderDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditReminderDialog)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    connect(ui->textEdit, &QTextEdit::textChanged, this, &EditReminderDialog::onTextChanged);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &EditReminderDialog::onCursorPosChanged);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditReminderDialog::onSave);
    connect(ui->chooseEmployeeButton, &QAbstractButton::clicked, this, &EditReminderDialog::onChooseEmployee);
}

EditReminderDialog::~EditReminderDialog()
{
    delete ui;
}

/**
 * Получает список выбранных сотрудников из класса ChooseEmployee.
 */
void EditReminderDialog::receiveEmployee(const QStringList& employee)
{
    this->employee = employee;

    if (this->employee.length() == 1)
        ui->employee->setText(this->employee.first());
    else
        ui->employee->setText(tr("Группа") + " (" + QString::number(this->employee.length()) + ")");
}

/**
 * Выполняет открытие окна для выбора списка сотрудников,
 * которые получат напоминание.
 */
void EditReminderDialog::onChooseEmployee()
{
    if (employee.isEmpty())
        employee = employeeInitial;

    if (!chooseEmployee.isNull())
        chooseEmployee->close();

    chooseEmployee = new ChooseEmployee;
    chooseEmployee->setValues(employee);
    connect(chooseEmployee, &ChooseEmployee::sendEmployee, this, &EditReminderDialog::receiveEmployee);
    connect(this, &EditReminderDialog::getPos, chooseEmployee, &ChooseEmployee::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    chooseEmployee->show();
    chooseEmployee->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void EditReminderDialog::onSave()
{
    QDateTime dateTime = ui->dateTimeEdit->dateTime();
    QString note = ui->textEdit->toPlainText().trimmed();

    if (dateTime < QDateTime::currentDateTime())
    {
        MsgBoxError(tr("Указано прошедшее время!"));

        return;
    }

    if (note.isEmpty())
    {
        MsgBoxError(tr("Содержание напоминания не может быть пустым!"));

        return;
    }

    if (employee.isEmpty())
        employee = employeeInitial;

    if (ui->chooseEmployeeButton->isHidden() && employee.first() == my_number && dateTime == oldDateTime && note == oldNote && employee == employeeInitial)
    {
        MsgBoxError(tr("Для сохранения требуется внести изменения!"));

        return;
    }

    QSqlQuery query(db);

    if (ui->chooseEmployeeButton->isHidden())
    {
        if (group_id == "0")
        {
            query.prepare("UPDATE reminders SET datetime = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(dateTime);
            query.addBindValue(id);
            query.exec();
        }
        else if (group_id != "0")
        {
            query.prepare("UPDATE reminders SET group_id = NULL, datetime = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(dateTime);
            query.addBindValue(id);
            query.exec();

            if (employee.length() == 2)
            {
                query.prepare("UPDATE reminders SET group_id = NULL WHERE group_id = ?");
                query.addBindValue(group_id);
                query.exec();
            }
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
                    for (qint32 i = 0; i < employee.length(); ++i)
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
                if (employeeInitial.length() == 1)
                {
                    if (!employee.contains(employeeInitial.first()))
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE id = ?");
                        query.addBindValue(id);
                        query.exec();
                    }
                }
                else
                {
                    for (qint32 i = 0; i < employeeInitial.length(); ++i)
                    {
                        if (employee.contains(employeeInitial.at(i)))
                            continue;

                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE phone_to = ? AND group_id = ?");
                        query.addBindValue(employeeInitial.at(i));
                        query.addBindValue(group_id);
                        query.exec();
                    }
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
                    for (qint32 i = 0; i < employee.length(); ++i)
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
                if (employeeInitial.length() == 1)
                {
                    if (!employee.contains(employeeInitial.first()))
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE id = ?");
                        query.addBindValue(id);
                        query.exec();
                    }
                }
                else
                {
                    for (qint32 i = 0; i < employeeInitial.length(); ++i)
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE phone_to = ? AND group_id = ?");
                        query.addBindValue(employeeInitial.at(i));
                        query.addBindValue(group_id);
                        query.exec();
                    }
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
                    query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, completed, active) VALUES(?, ?, ?, ?, false, true)");
                    query.addBindValue(employee.first());
                    query.addBindValue(employee.first());
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.exec();
                }
            }
        }
    }

    if (!chooseEmployee.isNull())
        chooseEmployee->close();

    emit sendData(true);

    close();

    MsgBoxInformation(tr("Напоминание успешно изменено!"));
}

/**
 * Получает данные существующего напоминания из классов
 * PopupReminder и RemindersDialog.
 */
void EditReminderDialog::setValues(const QString& receivedId, const QString& receivedGroupId, const QDateTime& receivedDateTime, const QString& receivedNote)
{
    id = receivedId;
    group_id = receivedGroupId;
    oldDateTime = receivedDateTime;
    oldNote = receivedNote;

    ui->dateTimeEdit->setDateTime(oldDateTime);
    ui->textEdit->setText(oldNote);

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
            ui->chooseEmployeeButton->hide();

            ui->textEdit->setReadOnly(true);
            ui->textEdit->setStyleSheet("QTextEdit { background-color: #fffff0; }");
        }
    }
    else
    {
        query.prepare("SELECT phone_to FROM reminders WHERE group_id = ?");
        query.addBindValue(group_id);
        query.exec();

        while (query.next())
            employeeInitial.append(query.value(0).toString());

        query.prepare("SELECT phone_from FROM reminders WHERE id = ?");
        query.addBindValue(group_id);
        query.exec();
        query.next();

        if (query.value(0).toString() != my_number)
        {
            ui->chooseEmployeeButton->hide();

            ui->textEdit->setReadOnly(true);
            ui->textEdit->setStyleSheet("QTextEdit {background-color: #fffff0;}");

            ui->employee->setText(my_number);
        }
        else
            ui->employee->setText(tr("Группа") + " (" + QString::number(employeeInitial.length()) + ")");
    }
}

/**
 * Выполняет обработку закрытия окна.
 */
void EditReminderDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    if (!chooseEmployee.isNull())
        chooseEmployee->close();
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void EditReminderDialog::onTextChanged()
{
    int m_maxDescriptionLength = 255;

    if (ui->textEdit->toPlainText().length() > m_maxDescriptionLength)
    {
        int diff = ui->textEdit->toPlainText().length() - m_maxDescriptionLength;

        QString newStr = ui->textEdit->toPlainText();
        newStr.chop(diff);

        ui->textEdit->setText(newStr);

        QTextCursor cursor(ui->textEdit->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

        ui->textEdit->setTextCursor(cursor);
    }
}

/**
 * Выполняет сохранение позиции текстового курсора.
 */
void EditReminderDialog::onCursorPosChanged()
{
    if (textCursor.isNull())
    {
        textCursor = ui->textEdit->textCursor();
        textCursor.movePosition(QTextCursor::End);
    }
    else
        textCursor = ui->textEdit->textCursor();
}

/**
 * Выполняет обработку совершения операций с привязанным объектом.
 */
bool EditReminderDialog::eventFilter(QObject*, QEvent* event)
{
    if (event && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);

        if (keyEvent && (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab))
        {
            if (ui->textEdit->hasFocus())
                ui->textEdit->setTextCursor(textCursor);

            return true;
        }
    }

    return false;
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиш Esc и Enter.
 */
void EditReminderDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else if (event->key() == Qt::Key_Return)
    {
        if (ui->textEdit->hasFocus())
            return;
        else
            onSave();
    }
    else
        QDialog::keyPressEvent(event);
}

/**
 * Выполняет прибавление к текущему времени 5 минут.
 */
void EditReminderDialog::on_add5MinButton_clicked()
{
    ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(300));
}

/**
 * Выполняет прибавление к текущему времени 10 минут.
 */
void EditReminderDialog::on_add10MinButton_clicked()
{
     ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(600));
}

/**
 * Выполняет прибавление к текущему времени 30 минут.
 */
void EditReminderDialog::on_add30MinButton_clicked()
{
     ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(1800));
}

/**
 * Выполняет прибавление к текущему времени 60 минут.
 */
void EditReminderDialog::on_add60MinButton_clicked()
{
     ui->dateTimeEdit->setTime(ui->dateTimeEdit->time().addSecs(3600));
}
