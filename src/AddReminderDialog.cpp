/*
 * Класс служит для создания напоминания.
 */

#include "AddReminderDialog.h"
#include "ui_AddReminderDialog.h"

#include <QDebug>
#include <QMessageBox>

AddReminderDialog::AddReminderDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddReminderDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);    

    m_employee.append(g_personalNumberName);

    ui->employee->setText(g_personalNumberName);

    QString language = global::getSettingsValue("language", "settings").toString();

    if (language == "Русский")
        ui->calendarWidget->setLocale(QLocale::Russian);
    else if (language == "Українська")
        ui->calendarWidget->setLocale(QLocale::Ukrainian);
    else if (language == "English")
        ui->calendarWidget->setLocale(QLocale::English);

    ui->calendarWidget->setGridVisible(true);
    ui->calendarWidget->setMinimumDate(QDate::currentDate());
    ui->calendarWidget->setSelectedDate(QDate::currentDate());

    ui->timeEdit->setTime(QTime::currentTime());

    connect(ui->textEdit, &QTextEdit::textChanged, this, &AddReminderDialog::onTextChanged);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddReminderDialog::onSave);
    connect(ui->chooseEmployeeButton, &QAbstractButton::clicked, this, &AddReminderDialog::onChooseEmployee);
}

AddReminderDialog::~AddReminderDialog()
{
    delete ui;
}

/**
 * Получает список выбранных сотрудников из классов
 * ChooseEmployee и InternalContactsDialog.
 */
void AddReminderDialog::receiveEmployee(const QStringList& employee)
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
void AddReminderDialog::onChooseEmployee()
{
    if (!m_chooseEmployee.isNull())
        m_chooseEmployee->close();

    m_chooseEmployee = new ChooseEmployee;
    m_chooseEmployee->setValues(m_employee);
    connect(m_chooseEmployee, &ChooseEmployee::sendEmployee, this, &AddReminderDialog::receiveEmployee);
    connect(this, &AddReminderDialog::getPos, m_chooseEmployee, &ChooseEmployee::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    m_chooseEmployee->show();
    m_chooseEmployee->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void AddReminderDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime(date, time);
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

    QSqlQuery query(m_db);

    if (m_callId.isEmpty())
    {
        if (ui->employee->text() != g_personalNumberName)
        {
            query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, false, false, true)");
            query.addBindValue(g_personalNumberName);
            query.addBindValue(m_employee.first());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.exec();

            if (m_employee.length() > 1)
            {
                qint32 id = query.lastInsertId().toInt();

                query.prepare("UPDATE reminders SET group_id = ? WHERE id = ?");
                query.addBindValue(id);
                query.addBindValue(id);
                query.exec();

                for (qint32 i = 1; i < m_employee.length(); ++i)
                {
                    query.prepare("INSERT INTO reminders (group_id, phone_from, phone_to, datetime, content, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
                    query.addBindValue(id);
                    query.addBindValue(g_personalNumberName);
                    query.addBindValue(m_employee.at(i));
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.exec();
                }
            }
        }
        else
        {
            query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, active) VALUES(?, ?, ?, ?, true)");
            query.addBindValue(g_personalNumberName);
            query.addBindValue(ui->employee->text());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.exec();
        }
    }
    else
    {
        if (ui->employee->text() != g_personalNumberName)
        {
            query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, call_id, viewed, completed, active) VALUES(?, ?, ?, ?, ?, false, false, true)");
            query.addBindValue(g_personalNumberName);
            query.addBindValue(m_employee.first());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.addBindValue(m_callId);
            query.exec();

            if (m_employee.length() > 1)
            {
                qint32 id = query.lastInsertId().toInt();

                query.prepare("UPDATE reminders SET group_id = ? WHERE id = ?");
                query.addBindValue(id);
                query.addBindValue(id);
                query.exec();

                for (qint32 i = 1; i < m_employee.length(); ++i)
                {
                    query.prepare("INSERT INTO reminders (group_id, phone_from, phone_to, datetime, content, call_id, viewed, completed, active) VALUES(?, ?, ?, ?, ?, ?, false, false, true)");
                    query.addBindValue(id);
                    query.addBindValue(g_personalNumberName);
                    query.addBindValue(m_employee.at(i));
                    query.addBindValue(dateTime);
                    query.addBindValue(note);
                    query.addBindValue(m_callId);
                    query.exec();
                }
            }
        }
        else
        {
            query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, call_id, active) VALUES(?, ?, ?, ?, ?, true)");
            query.addBindValue(g_personalNumberName);
            query.addBindValue(ui->employee->text());
            query.addBindValue(dateTime);
            query.addBindValue(note);
            query.addBindValue(m_callId);
            query.exec();
        }
    }

    if (!m_chooseEmployee.isNull())
        m_chooseEmployee->close();

    emit sendData(true);

    close();

    if (ui->employee->text() != g_personalNumberName)
        MsgBoxInformation(tr("Напоминание успешно отправлено!"));
    else
        MsgBoxInformation(tr("Напоминание успешно добавлено!"));
}

/**
 * Получает уникальный id звонка из класса PopupWindow
 * или id контакта из классов ViewContactDialog и ViewOrgContactDialog.
 */
void AddReminderDialog::setCallId(const QString& callId)
{
    m_callId = callId;
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void AddReminderDialog::onTextChanged()
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
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиш Esc и Enter.
 */
void AddReminderDialog::keyPressEvent(QKeyEvent* event)
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
 * Выполняет обработку закрытия окна.
 */
void AddReminderDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    if (!m_chooseEmployee.isNull())
        m_chooseEmployee->close();
}

/**
 * Выполняет прибавление к текущему времени 5 минут.
 */
void AddReminderDialog::on_add5MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(300));
}

/**
 * Выполняет прибавление к текущему времени 10 минут.
 */
void AddReminderDialog::on_add10MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(600));
}

/**
 * Выполняет прибавление к текущему времени 30 минут.
 */
void AddReminderDialog::on_add30MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(1800));
}

/**
 * Выполняет прибавление к текущему времени 60 минут.
 */
void AddReminderDialog::on_add60MinButton_clicked()
{
    ui->timeEdit->setTime(ui->timeEdit->time().addSecs(3600));
}
