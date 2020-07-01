#include "RemindersDialog.h"
#include "ui_RemindersDialog.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QKeyEvent>
#include <QSqlQuery>

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

    ui->calendarWidget->setGridVisible(true);
    ui->timeEdit->setTime(QTime::currentTime());
    ui->textEdit->installEventFilter(this);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onUpdate()));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditReminder(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onEditReminder(const QModelIndex &)));
    connect(ui->saveButton, &QAbstractButton::clicked, this, &RemindersDialog::onSave);

    ui->tableView->setStyleSheet  ("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_2->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();

    ui->tabWidget->setCurrentIndex(0);

    loadActiveReminders();

    QSqlDatabase db;
    QSqlQuery query(db);

    QList<QDateTime> dateTimes;
    QList<QString> notes;

    query.prepare("SELECT datetime, content FROM reminders WHERE phone = '" + my_number + "' AND active IS TRUE");
    query.exec();
    while (query.next())
    {
        dateTimes.append(query.value(0).value<QDateTime>());
        notes.append(query.value(1).value<QString>());
    }

    remindersThread = new QThread;
    remindersThreadManager = new RemindersThread(my_number, dateTimes, notes);
    remindersThreadManager->moveToThread(remindersThread);
    connect(remindersThread, SIGNAL(started()), remindersThreadManager, SLOT(process()));
    connect(remindersThreadManager, SIGNAL(notify(QString)), this, SLOT(onNotify(QString)));
    connect(remindersThreadManager, SIGNAL(finished()), remindersThread, SLOT(quit()));
    connect(remindersThreadManager, SIGNAL(finished()), remindersThreadManager, SLOT(deleteLater()));
    connect(remindersThread, SIGNAL(finished()), remindersThread, SLOT(deleteLater()));
    remindersThread->start();
}

RemindersDialog::~RemindersDialog()
{
    remindersThread->requestInterruption();
    delete settingsDialog;
    delete ui;
}

void RemindersDialog::deleteObjects()
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        for (int i = 0; i < widgetsActive.size(); ++i)
            widgetsActive[i]->deleteLater();

        for (int i = 0; i < layoutsActive.size(); ++i)
            layoutsActive[i]->deleteLater();

        for (int i = 0; i < boxesActive.size(); ++i)
            boxesActive[i]->deleteLater();

        for (int i = 0; i < queriesActive.size(); ++i)
            queriesActive[i]->deleteLater();

        queriesActive.clear();
        widgetsActive.clear();
        layoutsActive.clear();
        boxesActive.clear();
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        for (int i = 0; i < widgetsInactive.size(); ++i)
            widgetsInactive[i]->deleteLater();

        for (int i = 0; i < layoutsInactive.size(); ++i)
            layoutsInactive[i]->deleteLater();

        for (int i = 0; i < boxesInactive.size(); ++i)
            boxesInactive[i]->deleteLater();

        for (int i = 0; i < queriesInactive.size(); ++i)
            queriesInactive[i]->deleteLater();

        queriesInactive.clear();
        widgetsInactive.clear();
        layoutsInactive.clear();
        boxesInactive.clear();
    }
}

void RemindersDialog::sendNewValues()
{
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT datetime, content FROM reminders WHERE phone = '" + my_number + "' AND active IS TRUE");
    query.exec();
    while (query.next())
    {
        dateTimes.append(query.value(0).value<QDateTime>());
        notes.append(query.value(1).value<QString>());
    }

    remindersThreadManager->receiveNewValues(dateTimes, notes);
}

void RemindersDialog::onTextChanged()
{
    if(ui->textEdit->toPlainText().length() > 255)
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

void RemindersDialog::loadActiveReminders()
{
    if (!widgetsActive.isEmpty())
        deleteObjects();

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET active = false WHERE phone = '" + my_number + "' AND datetime < ? AND active IS TRUE");
    query.addBindValue(QDateTime::currentDateTime());
    query.exec();

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;

    query1->setQuery("SELECT id, datetime, content FROM reminders WHERE phone = '" + my_number + "' AND active IS TRUE ORDER BY datetime DESC");
    query2->setQuery("SELECT active FROM reminders WHERE phone = '" + my_number + "' AND active IS TRUE ORDER BY datetime DESC");

    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Активное"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Содержание"));

    ui->tableView->setModel(query1);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query1->index(row_index, 1), addCheckBox(row_index));

    ui->tableView->setColumnHidden(0, true);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());   
    ui->tableView->setWordWrap(true);
    ui->tableView->resizeRowsToContents();
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    queriesActive.append(query1);
    queriesActive.append(query2);
}

void RemindersDialog::loadInactiveReminders()
{
    if (!widgetsInactive.isEmpty())
        deleteObjects();

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET active = false WHERE phone = '" + my_number + "' AND datetime < ? AND active IS TRUE");
    query.addBindValue(QDateTime::currentDateTime());
    query.exec();

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;

    query1->setQuery("SELECT id, datetime, content FROM reminders WHERE phone = '" + my_number + "' AND active IS FALSE ORDER BY datetime DESC");
    query2->setQuery("SELECT active FROM reminders WHERE phone = '" + my_number + "' AND active IS FALSE ORDER BY datetime DESC");

    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Активное"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Содержание"));

    ui->tableView_2->setModel(query1);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
        ui->tableView_2->setIndexWidget(query1->index(row_index, 1), addCheckBox(row_index));

    ui->tableView_2->setColumnHidden(0, true);
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->setWordWrap(true);
    ui->tableView_2->resizeRowsToContents();
    ui->tableView_2->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);

    queriesInactive.append(query1);
    queriesInactive.append(query2);
}

void RemindersDialog::onEditReminder(const QModelIndex &index)
{
    QString id = query1->data(query1->index(index.row(), 0)).toString();
    QDateTime dateTime = query1->data(query1->index(index.row(), 2)).toDateTime();
    QString note = query1->data(query1->index(index.row(), 3)).toString();

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

            query.prepare("UPDATE reminders SET active = false WHERE id = ? AND phone = '" + my_number + "' AND datetime = ? AND active IS TRUE");
            query.addBindValue(id);
            query.addBindValue(dateTime);
            query.exec();
        }
        else
        {
            query.prepare("SELECT EXISTS (SELECT datetime FROM reminders WHERE phone = '" + my_number + "' AND datetime = ? AND active IS TRUE)");
            query.addBindValue(dateTime);
            query.exec();
            query.next();
            if (query.value(0) != 0)
            {
                QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Напоминание на заданное время уже существует и активно!"), QMessageBox::Ok);
                return;
            }

            checkBox->setChecked(true);

            query.prepare("UPDATE reminders SET active = true WHERE id = ? AND phone = '" + my_number + "' AND datetime = ? AND active IS FALSE");
            query.addBindValue(id);
            query.addBindValue(dateTime);
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

    layout->setContentsMargins(25, 0, 0, 0);

    wgt->setLayout(layout);

    if (ui->tabWidget->currentIndex() == 0)
    {
        widgetsActive.append(wgt);
        layoutsActive.append(layout);
        boxesActive.append(checkBox);
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        widgetsInactive.append(wgt);
        layoutsInactive.append(layout);
        boxesInactive.append(checkBox);
    }

    QString id = query1->data(query1->index(row_index, 0)).toString();
    QDateTime dateTime = query1->data(query1->index(row_index, 2)).toDateTime();

    connect(checkBox, SIGNAL(pressed()), this, SLOT(changeState()));
    checkBox->setProperty("checkBox", qVariantFromValue(checkBox));
    checkBox->setProperty("id", qVariantFromValue(id));
    checkBox->setProperty("dateTime", qVariantFromValue(dateTime));

    return wgt;
}

void RemindersDialog::onUpdate()
{
    if (ui->tabWidget->currentIndex() == 0)
        loadActiveReminders();
    if (ui->tabWidget->currentIndex() == 1)
        loadInactiveReminders();
}

void RemindersDialog::onNotify(QString reminderNote)
{
    QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("%1!").arg(reminderNote), QMessageBox::Ok);

    onUpdate();
}

void RemindersDialog::onSave()
{
    QDate date = ui->calendarWidget->selectedDate();
    QTime time(ui->timeEdit->time().hour(), ui->timeEdit->time().minute(), 0);
    QDateTime dateTime = QDateTime::QDateTime(date, time);
    QString note = ui->textEdit->toPlainText();

    if (dateTime < QDateTime::currentDateTime())
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Указано прошедшее время!"), QMessageBox::Ok);
        return;
    }

    if (ui->textEdit->toPlainText().isEmpty())
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Содержание напоминания не может быть пустым!"), QMessageBox::Ok);
        return;
    }

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT EXISTS (SELECT datetime FROM reminders WHERE phone = '" + my_number + "' AND datetime = ? AND active IS TRUE)");
    query.addBindValue(dateTime);
    query.exec();
    query.next();
    if (query.value(0) != 0)
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Напоминание на заданное время уже существует и активно!"), QMessageBox::Ok);
        return;
    }

    query.prepare("INSERT INTO reminders (phone, datetime, content, active) VALUES(?, ?, ?, ?)");
    query.addBindValue(my_number);
    query.addBindValue(dateTime);
    query.addBindValue(note);
    query.addBindValue(true);
    query.exec();

    onUpdate();
    sendNewValues();
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
