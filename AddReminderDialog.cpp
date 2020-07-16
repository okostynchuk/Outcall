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

    my_number = global::getExtensionNumber("extensions");

    ui->comboBox->addItem(g_pAsteriskManager->extensionNumbers.value(my_number));
    ui->comboBox->addItems(g_pAsteriskManager->extensionNumbers.values());

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
    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddReminderDialog::onSave);

    ui->calendarWidget->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
}

AddReminderDialog::~AddReminderDialog()
{
    delete ui;
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

    QRegExp reg("([0-9]+)(.+)");
    reg.indexIn(ui->comboBox->currentText());

    if (callId.isEmpty())
    {
        query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, active) VALUES(?, ?, ?, ?, ?)");
        query.addBindValue(my_number);
        query.addBindValue(reg.cap(1));
        query.addBindValue(dateTime);
        query.addBindValue(note);
        query.addBindValue(true);
        query.exec();
    }
    else
    {
        query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, active, call_id) VALUES(?, ?, ?, ?, ?, ?)");
        query.addBindValue(my_number);
        query.addBindValue(reg.cap(1));
        query.addBindValue(dateTime);
        query.addBindValue(note);
        query.addBindValue(true);
        query.addBindValue(callId);
        query.exec();
    }

    emit sendData(true);
    close();

    if (reg.cap(1) != my_number)
        QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Напоминание успешно отправлено!"), QMessageBox::Ok);
    else
        QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Напоминание успешно добавлено!"), QMessageBox::Ok);

    destroy(true);
}

void AddReminderDialog::setCallId(QString receivedCallId)
{
    callId = receivedCallId;
}

void AddReminderDialog::onTextChanged()
{
    if(ui->textEdit->toPlainText().simplified().length() > 255)
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
