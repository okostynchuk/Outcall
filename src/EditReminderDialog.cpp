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
    m_employee = employee;

    if (m_employee.length() == 1)
        ui->employee->setText(m_employee.first());
    else
        ui->employee->setText(tr("Группа") + " (" + QString::number(m_employee.length()) + ")");
}

/**
 * Выполняет открытие окна для выбора списка сотрудников,
 * которые получат напоминание.
 */
void EditReminderDialog::onChooseEmployee()
{
    if (m_employee.isEmpty())
        m_employee = m_employeeInitial;

    if (!m_chooseEmployee.isNull())
        m_chooseEmployee->close();

    m_chooseEmployee = new ChooseEmployee;
    m_chooseEmployee->setValues(m_employee);
    connect(m_chooseEmployee, &ChooseEmployee::sendEmployee, this, &EditReminderDialog::receiveEmployee);
    connect(this, &EditReminderDialog::getPos, m_chooseEmployee, &ChooseEmployee::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    m_chooseEmployee->show();
    m_chooseEmployee->setAttribute(Qt::WA_DeleteOnClose);
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

    if (m_employee.isEmpty())
        m_employee = m_employeeInitial;

    if (ui->chooseEmployeeButton->isHidden() && m_employee.first() == g_personalNumberName && dateTime == m_oldDateTime && note == m_oldNote && m_employee == m_employeeInitial)
    {
        MsgBoxError(tr("Для сохранения требуется внести изменения!"));

        return;
    }

    QSqlQuery query(m_db);

    if (ui->chooseEmployeeButton->isHidden())
    {
        if (m_groupId == "0")
        {
            query.prepare("UPDATE reminders SET datetime = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(dateTime);
            query.addBindValue(m_id);
            query.exec();
        }
        else if (m_groupId != "0")
        {
            query.prepare("UPDATE reminders SET group_id = NULL, datetime = ?, completed = false, active = true WHERE id = ?");
            query.addBindValue(dateTime);
            query.addBindValue(m_id);
            query.exec();

            if (m_employee.length() == 2)
            {
                query.prepare("UPDATE reminders SET group_id = NULL WHERE group_id = ?");
                query.addBindValue(m_groupId);
                query.exec();
            }
        }
    }
    else
    {
        if (m_employeeInitial.length() == 1 && m_employee.length() > 1)
        {
            query.prepare("UPDATE reminders SET group_id = ? WHERE id = ?");
            query.addBindValue(m_id);
            query.addBindValue(m_id);
            query.exec();

            query.prepare("SELECT group_id FROM reminders WHERE id = ?");
            query.addBindValue(m_id);
            query.exec();
            query.first();

            m_groupId = query.value(0).toString();
        }

        if (ui->employee->text() != g_personalNumberName)
        {
            if (m_employee == m_employeeInitial)
            {
                if (m_employee.length() == 1)
                {
                    query.prepare("UPDATE reminders SET datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE id = ?");
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(m_id);
                    query.exec();
                }
                else
                {
                    for (qint32 i = 0; i < m_employee.length(); ++i)
                    {
                        query.prepare("UPDATE reminders SET datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE phone_to = ? AND group_id = ?");
                        query.addBindValue(dateTime);
                        query.addBindValue(note);
                        query.addBindValue(m_employee.at(i));
                        query.addBindValue(m_groupId);
                        query.exec();
                    }
                }
            }
            else
            {
                if (m_employeeInitial.length() == 1)
                {
                    if (!m_employee.contains(m_employeeInitial.first()))
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE id = ?");
                        query.addBindValue(m_id);
                        query.exec();
                    }
                }
                else
                {
                    for (qint32 i = 0; i < m_employeeInitial.length(); ++i)
                    {
                        if (m_employee.contains(m_employeeInitial.at(i)))
                            continue;

                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE phone_to = ? AND group_id = ?");
                        query.addBindValue(m_employeeInitial.at(i));
                        query.addBindValue(m_groupId);
                        query.exec();
                    }
                }

                if (m_employee.length() == 1)
                {
                    if (m_employeeInitial.contains(m_employee.first()))
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, phone_to = ?, datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE group_id = ?");
                        query.addBindValue(m_employee.first());
                        query.addBindValue(dateTime);
                        query.addBindValue(note);
                        query.addBindValue(m_groupId);
                        query.exec();
                    }
                    else
                    {
                        query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, false, false, true)");
                        query.addBindValue(g_personalNumberName);
                        query.addBindValue(m_employee.first());
                        query.addBindValue(dateTime);
                        query.addBindValue(note);
                        query.exec();
                    }
                }
                else
                {
                    for (qint32 i = 0; i < m_employee.length(); ++i)
                    {
                        if (m_employeeInitial.contains(m_employee.at(i)))
                        {
                            query.prepare("UPDATE reminders SET datetime = ?, content = ?, viewed = false, completed = false, active = true WHERE phone_to = ? AND group_id = ?");
                            query.addBindValue(dateTime);
                            query.addBindValue(note);
                            query.addBindValue(m_employee.at(i));
                            query.addBindValue(m_groupId);
                            query.exec();
                        }
                        else
                        {
                            query.prepare("INSERT INTO reminders (group_id, phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
                            query.addBindValue(m_groupId);
                            query.addBindValue(g_personalNumberName);
                            query.addBindValue(m_employee.at(i));
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
            if (m_employee == m_employeeInitial)
            {
                query.prepare("UPDATE reminders SET datetime = ?, content = ?, completed = false, active = true WHERE id = ?");
                query.addBindValue(dateTime);
                query.addBindValue(note);
                query.addBindValue(m_id);
                query.exec();
            }
            else
            {
                if (m_employeeInitial.length() == 1)
                {
                    if (!m_employee.contains(m_employeeInitial.first()))
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE id = ?");
                        query.addBindValue(m_id);
                        query.exec();
                    }
                }
                else
                {
                    for (qint32 i = 0; i < m_employeeInitial.length(); ++i)
                    {
                        query.prepare("UPDATE reminders SET group_id = NULL, active = false WHERE phone_to = ? AND group_id = ?");
                        query.addBindValue(m_employeeInitial.at(i));
                        query.addBindValue(m_groupId);
                        query.exec();
                    }
                }

                if (m_employeeInitial.contains(m_employee.first()))
                {
                    query.prepare("UPDATE reminders SET phone_to = ?, datetime = ?, content = ?, completed = false, active = true WHERE id = ?");
                    query.addBindValue(m_employee.first());
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(m_id);
                    query.exec();
                }
                else
                {
                    query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, completed, active) VALUES(?, ?, ?, ?, false, true)");
                    query.addBindValue(m_employee.first());
                    query.addBindValue(m_employee.first());
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.exec();
                }
            }
        }
    }

    if (!m_chooseEmployee.isNull())
        m_chooseEmployee->close();

    emit sendData(true);

    close();

    MsgBoxInformation(tr("Напоминание успешно изменено!"));
}

/**
 * Получает данные существующего напоминания из классов
 * PopupReminder и RemindersDialog.
 */
void EditReminderDialog::setValues(const QString& id, const QString& groupId, const QDateTime& dateTime, const QString& note)
{
    m_id = id;
    m_groupId = groupId;
    m_oldDateTime = dateTime;
    m_oldNote = note;

    ui->dateTimeEdit->setDateTime(m_oldDateTime);
    ui->textEdit->setText(m_oldNote);

    QSqlQuery query(m_db);

    if (m_groupId == "0")
    {
        query.prepare("SELECT phone_from, phone_to FROM reminders WHERE id = ?");
        query.addBindValue(m_id);
        query.exec();
        query.first();

        QString phone_from = query.value(0).toString();
        QString phone_to = query.value(1).toString();

        m_employeeInitial.append(phone_to);

        ui->employee->setText(phone_to);

        if (phone_from != phone_to && phone_from != g_personalNumberName)
        {
            ui->chooseEmployeeButton->hide();

            ui->textEdit->setReadOnly(true);
            ui->textEdit->setStyleSheet("QTextEdit { background-color: #fffff0; }");
        }
    }
    else
    {
        query.prepare("SELECT phone_to FROM reminders WHERE group_id = ?");
        query.addBindValue(m_groupId);
        query.exec();

        while (query.next())
            m_employeeInitial.append(query.value(0).toString());

        query.prepare("SELECT phone_from FROM reminders WHERE id = ?");
        query.addBindValue(m_groupId);
        query.exec();
        query.next();

        if (query.value(0).toString() != g_personalNumberName)
        {
            ui->chooseEmployeeButton->hide();

            ui->textEdit->setReadOnly(true);
            ui->textEdit->setStyleSheet("QTextEdit {background-color: #fffff0;}");

            ui->employee->setText(g_personalNumberName);
        }
        else
            ui->employee->setText(tr("Группа") + " (" + QString::number(m_employeeInitial.length()) + ")");
    }
}

/**
 * Выполняет обработку закрытия окна.
 */
void EditReminderDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    if (!m_chooseEmployee.isNull())
        m_chooseEmployee->close();
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void EditReminderDialog::onTextChanged()
{
    qint32 maxTextLength = 255;

    if (ui->textEdit->toPlainText().length() > maxTextLength)
    {
        qint32 diff = ui->textEdit->toPlainText().length() - maxTextLength;

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
    if (m_textCursor.isNull())
    {
        m_textCursor = ui->textEdit->textCursor();
        m_textCursor.movePosition(QTextCursor::End);
    }
    else
        m_textCursor = ui->textEdit->textCursor();
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
                ui->textEdit->setTextCursor(m_textCursor);

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
