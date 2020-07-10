#include "RemindersDialog.h"
#include "ui_RemindersDialog.h"
#include "PopupReminder.h"
#include "Global.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QKeyEvent>
#include <QSqlQuery>

#define TIME_TO_UPDATE 5000 // msec

RemindersDialog::RemindersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemindersDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);

    languages = global::getSettingsValue("language", "settings").toString();
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

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onUpdate()));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditReminder(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditReminder(const QModelIndex &)));
    connect(&timer, &QTimer::timeout, this, &RemindersDialog::onTimer);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &RemindersDialog::onSave);

    ui->tableView->setStyleSheet  ("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_2->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->calendarWidget->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();

    ui->tabWidget->setCurrentIndex(0);

    loadActualReminders();

    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT id, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND active IS TRUE");
    query.exec();
    while (query.next())
    {
        ids.append(query.value(0).value<QString>());
        dateTimes.append(query.value(1).value<QDateTime>());
        notes.append(query.value(2).value<QString>());
    }

    remindersThread = new QThread;
    remindersThreadManager = new RemindersThread(my_number, ids, dateTimes, notes);
    remindersThreadManager->moveToThread(remindersThread);
    connect(remindersThread, SIGNAL(started()), remindersThreadManager, SLOT(process()));
    connect(remindersThreadManager, SIGNAL(notify(QString, QDateTime, QString)), this, SLOT(onNotify(QString, QDateTime, QString)));
    connect(remindersThreadManager, SIGNAL(finished()), remindersThread, SLOT(quit()));
    connect(remindersThreadManager, SIGNAL(finished()), remindersThreadManager, SLOT(deleteLater()));
    connect(remindersThread, SIGNAL(finished()), remindersThread, SLOT(deleteLater()));
    remindersThread->start();

    timer.setInterval(TIME_TO_UPDATE);
    timer.start();
}

RemindersDialog::~RemindersDialog()
{
    remindersThread->requestInterruption();
    delete settingsDialog;
    delete ui;
}

void RemindersDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    ui->calendarWidget->setMinimumDate(QDate::currentDate());
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
    ui->timeEdit->setTime(QTime::currentTime());

    ui->comboBox->clear();
    ui->comboBox->addItem(g_pAsteriskManager->extensionNumbers.value(my_number));
    ui->comboBox->addItems(g_pAsteriskManager->extensionNumbers.values());

    onUpdate();
}

void RemindersDialog::onTimer()
{
    onUpdate();
    sendNewValues();
}

void RemindersDialog::deleteObjects()
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        for (int i = 0; i < widgetsActual.size(); ++i)
            widgetsActual[i]->deleteLater();

        for (int i = 0; i < layoutsActual.size(); ++i)
            layoutsActual[i]->deleteLater();

        for (int i = 0; i < boxesActual.size(); ++i)
            boxesActual[i]->deleteLater();

        for (int i = 0; i < queriesActual.size(); ++i)
            queriesActual[i]->deleteLater();

        queriesActual.clear();
        widgetsActual.clear();
        layoutsActual.clear();
        boxesActual.clear();
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        for (int i = 0; i < widgetsPast.size(); ++i)
            widgetsPast[i]->deleteLater();

        for (int i = 0; i < layoutsPast.size(); ++i)
            layoutsPast[i]->deleteLater();

        for (int i = 0; i < boxesPast.size(); ++i)
            boxesPast[i]->deleteLater();

        for (int i = 0; i < queriesPast.size(); ++i)
            queriesPast[i]->deleteLater();

        queriesPast.clear();
        widgetsPast.clear();
        layoutsPast.clear();
        boxesPast.clear();
    }
}

void RemindersDialog::sendNewValues()
{
    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT id, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND active IS TRUE");
    query.exec();
    while (query.next())
    {
        ids.append(query.value(0).value<QString>());
        dateTimes.append(query.value(1).value<QDateTime>());
        notes.append(query.value(2).value<QString>());
    }

    remindersThreadManager->receiveNewValues(ids, dateTimes, notes);
}

void RemindersDialog::onTextChanged()
{
    if(ui->textEdit->toPlainText().simplified().length() > 255)
        ui->textEdit->textCursor().deletePreviousChar();
}

void RemindersDialog::receiveData(bool updating)
{
    if (updating)
    {
        onUpdate();
        sendNewValues();
    }
}

void RemindersDialog::loadActualReminders()
{
    if (!widgetsActual.isEmpty())
        deleteObjects();

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET active = false WHERE phone_to = '" + my_number + "' AND datetime < ?");
    query.addBindValue(QDateTime::currentDateTime());
    query.exec();

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;

    query1->setQuery("SELECT id, phone_to, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND datetime > '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' ORDER BY datetime DESC");
    query2->setQuery("SELECT active FROM reminders WHERE phone_to = '" + my_number + "' AND datetime > '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' ORDER BY datetime DESC");

    query1->insertColumn(2);
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Активное"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Содержание"));

    ui->tableView->setModel(query1);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query1->index(row_index, 2), addCheckBox(row_index));

    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(1, true);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());   
    ui->tableView->setWordWrap(true);
    ui->tableView->resizeRowsToContents();
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    queriesActual.append(query1);
    queriesActual.append(query2);
}

void RemindersDialog::loadPastReminders()
{
    if (!widgetsPast.isEmpty())
        deleteObjects();

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET active = false WHERE phone_to = '" + my_number + "' AND datetime < ?");
    query.addBindValue(QDateTime::currentDateTime());
    query.exec();

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;

    query1->setQuery("SELECT id, phone_to, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND active IS FALSE AND datetime < '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' ORDER BY datetime DESC");
    query2->setQuery("SELECT active FROM reminders WHERE phone_to = '" + my_number + "' AND active IS FALSE AND datetime < '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' ORDER BY datetime DESC");

    query1->insertColumn(2);
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Активное"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Содержание"));

    ui->tableView_2->setModel(query1);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
        ui->tableView_2->setIndexWidget(query1->index(row_index, 2), addCheckBox(row_index));

    ui->tableView_2->setColumnHidden(0, true);
    ui->tableView_2->setColumnHidden(1, true);
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->setWordWrap(true);
    ui->tableView_2->resizeRowsToContents();
    ui->tableView_2->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);

    queriesPast.append(query1);
    queriesPast.append(query2);
}

void RemindersDialog::onEditReminder(const QModelIndex &index)
{
    QString id = query1->data(query1->index(index.row(), 0)).toString();
    QDateTime dateTime = query1->data(query1->index(index.row(), 3)).toDateTime();
    QString note = query1->data(query1->index(index.row(), 4)).toString();

    editReminderDialog = new EditReminderDialog;
    editReminderDialog->setValuesReminders(my_number, id, dateTime, note);
    connect(editReminderDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    editReminderDialog->show();
    editReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void RemindersDialog::changeState()
{
    QCheckBox* checkBox = sender()->property("checkBox").value<QCheckBox*>();
    QString id = sender()->property("id").value<QString>();
    QDateTime dateTime = sender()->property("dateTime").value<QDateTime>();

    if (dateTime < QDateTime::currentDateTime())
    {
        checkBox->setChecked(false);

        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Указано прошедшее время!"), QMessageBox::Ok);
    }
    else
    {
        QSqlDatabase db;
        QSqlQuery query(db);

        if (checkBox->isChecked())
        {
            checkBox->setChecked(false);

            query.prepare("UPDATE reminders SET active = false WHERE id = ? AND phone_to = '" + my_number + "' AND active IS TRUE");
            query.addBindValue(id);
            query.exec();
        }
        else
        {
            checkBox->setChecked(true);

            query.prepare("UPDATE reminders SET active = true WHERE id = ? AND phone_to = '" + my_number + "' AND active IS FALSE");
            query.addBindValue(id);
            query.exec();
        }

        onUpdate();
        sendNewValues();
    }
}

QWidget* RemindersDialog::addCheckBox(int row_index)
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QCheckBox* checkBox = new QCheckBox(wgt);

    layout->addWidget(checkBox);

    if (query2->data(query2->index(row_index, 0)) == true)
        checkBox->setChecked(true);
    else
        checkBox->setChecked(false);

    if (languages == "Русский (по умолчанию)")
        layout->setContentsMargins(25, 0, 0, 0);
    else if (languages == "Українська")
        layout->setContentsMargins(22, 0, 0, 0);
    else if (languages == "English")
        layout->setContentsMargins(15, 0, 0, 0);

    wgt->setLayout(layout);

    if (ui->tabWidget->currentIndex() == 0)
    {
        widgetsActual.append(wgt);
        layoutsActual.append(layout);
        boxesActual.append(checkBox);
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        widgetsPast.append(wgt);
        layoutsPast.append(layout);
        boxesPast.append(checkBox);
    }

    QString id = query1->data(query1->index(row_index, 0)).toString();
    QDateTime dateTime = query1->data(query1->index(row_index, 3)).toDateTime();

    connect(checkBox, SIGNAL(pressed()), this, SLOT(changeState()));
    checkBox->setProperty("checkBox", qVariantFromValue(checkBox));
    checkBox->setProperty("id", qVariantFromValue(id));
    checkBox->setProperty("dateTime", qVariantFromValue(dateTime));

    return wgt;
}

void RemindersDialog::onUpdate()
{
    if (ui->tabWidget->currentIndex() == 0)
        loadActualReminders();
    if (ui->tabWidget->currentIndex() == 1)
        loadPastReminders();
}

void RemindersDialog::onNotify(QString reminderId, QDateTime reminderDateTime, QString reminderNote)
{
    PopupReminder::showReminder(this, my_number, reminderId, reminderDateTime, reminderNote);

    onUpdate();
}

void RemindersDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime::QDateTime(date, time);
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

    query.prepare("INSERT INTO reminders (phone_from, phone_to, datetime, content, active) VALUES(?, ?, ?, ?, ?)");
    query.addBindValue(my_number);
    query.addBindValue(reg.cap(1));
    query.addBindValue(dateTime);
    query.addBindValue(note);
    query.addBindValue(true);
    query.exec();

    onUpdate();
    sendNewValues();

    if (reg.cap(1) != my_number)
        QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Напоминание успешно отправлено!"), QMessageBox::Ok);
}

bool RemindersDialog::eventFilter(QObject *object, QEvent *event)
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
