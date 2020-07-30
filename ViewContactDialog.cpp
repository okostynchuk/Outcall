#include "ViewContactDialog.h"
#include "ui_ViewContactDialog.h"

#include "AsteriskManager.h"
#include "Global.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>

ViewContactDialog::ViewContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewContactDialog)
{
    ui->setupUi(this);

    userID = global::getSettingsValue("user_login", "settings").toString();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);

    connect(ui->comboBox,  SIGNAL(currentTextChanged(QString)), this, SLOT(daysChanged()));
    connect(ui->tabWidget_2, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));

    connect(ui->openAccessButton, &QPushButton::clicked, this, &ViewContactDialog::onOpenAccess);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &ViewContactDialog::onAddReminder);
    connect(ui->editButton, &QAbstractButton::clicked, this, &ViewContactDialog::onEdit);
    connect(ui->callButton, &QAbstractButton::clicked, this, &ViewContactDialog::onCall);
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewMissedNotes(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewRecievedNotes(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewPlacedNotes(const QModelIndex &)));

    my_number = global::getExtensionNumber("extensions");
    my_group = global::getGroupExtensionNumber("group_extensions");

    if (!MSSQLopened)
        ui->openAccessButton->hide();
}

ViewContactDialog::~ViewContactDialog()
{
    deleteNotesObjects();
    delete ui;
}

void ViewContactDialog::onAddReminder()
{
    addReminderDialog = new AddReminderDialog;
    addReminderDialog->setCallId(updateID);
    connect(addReminderDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addReminderDialog->show();
    addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::onOpenAccess() {
    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray password3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(password3));
    QString port_3 = global::getSettingsValue("port_3", "settings").toString();

    QSqlDatabase dbMSSQL = QSqlDatabase::addDatabase("QODBC", "Third");
    dbMSSQL.setDatabaseName("DRIVER={SQL Server Native Client 10.0};"
                            "Server="+hostName_3+","+port_3+";"
                            "Database="+databaseName_3+";"
                            "Uid="+userName_3+";"
                            "Pwd="+password_3);
    bool ok = dbMSSQL.open();

    QSqlQuery query(dbMSSQL);

    if (ok)
    {
        query.prepare("INSERT INTO CallTable (UserID, ClientID)"
                    "VALUES (?, ?)");
        query.addBindValue(userID);
        query.addBindValue(ui->VyborID->text().toInt());
        query.exec();

        ui->openAccessButton->setDisabled(true);

        dbMSSQL.close();
    }
    else
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе клиентов!"), QMessageBox::Ok);
    }
}

void ViewContactDialog::receiveData(bool updating)
{
    if (updating)
        emit sendData(true);
}

void ViewContactDialog::receiveNumber(QString &to)
{
    const QString from = my_number;
    const QString protocol = global::getSettingsValue(from, "extensions").toString();
    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

void ViewContactDialog::onCall()
{
    chooseNumber = new ChooseNumber;
    chooseNumber->setValuesNumber(updateID);
    connect(chooseNumber, SIGNAL(sendNumber(QString &)), this, SLOT(receiveNumber(QString &)));
    chooseNumber->show();
    chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::onEdit()
{
    destroy(true);
    editContactDialog = new EditContactDialog;
    editContactDialog->setValuesContacts(updateID);
    connect(editContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    editContactDialog->show();
    editContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::updateCalls()
{
    days = ui->comboBox->currentText();
    deleteNotesObjects();
    loadAllCalls();
    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}



void ViewContactDialog::setValuesContacts(QString &i)
{
    updateID = i;
    QSqlDatabase db;
    QSqlQuery query(db);
    QString sql = QString("SELECT entry_phone FROM entry_phone WHERE entry_id = %1").arg(updateID);
    query.prepare(sql);
    query.exec();
    int count = 1;
    while (query.next())
    {
        if (count == 1)
            firstNumber = query.value(0).toString();
        else if (count == 2)
            secondNumber = query.value(0).toString();
        else if (count == 3)
            thirdNumber = query.value(0).toString();
        else if (count == 4)
            fourthNumber = query.value(0).toString();
        else if (count == 5)
            fifthNumber = query.value(0).toString();
        count++;
    }
    query.prepare("SELECT entry_person_org_id FROM entry WHERE id = " + updateID);
    query.exec();
    QString orgID = NULL;
    while(query.next())
        orgID = query.value(0).toString();
    query.prepare("SELECT entry_org_name FROM entry WHERE id = " + orgID);
    query.exec();
    while(query.next())
        ui->Organization->setText(query.value(0).toString());
    sql = QString("SELECT distinct entry_person_fname, entry_person_mname, entry_person_lname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = %1").arg(updateID);
    query.prepare(sql);
    query.exec();
    query.next();
    QString entryFName = query.value(0).toString();
    QString entryMName = query.value(1).toString();
    QString entryLName = query.value(2).toString();
    QString entryCity = query.value(3).toString();
    QString entryAddress = query.value(4).toString();
    QString entryEmail = query.value(5).toString();
    QString entryVyborID = query.value(6).toString();
    QString entryComment = query.value(7).toString();
    ui->FirstNumber->setText(firstNumber);
    ui->SecondNumber->setText(secondNumber);
    ui->ThirdNumber->setText(thirdNumber);
    ui->FourthNumber->setText(fourthNumber);
    ui->FifthNumber->setText(fifthNumber);
    ui->FirstName->setText(entryFName);
    ui->Patronymic->setText(entryMName);
    ui->LastName->setText(entryLName);
    ui->City->setText(entryCity);
    ui->Address->setText(entryAddress);
    ui->Email->setText(entryEmail);
    ui->VyborID->setText(entryVyborID);
    ui->Comment->setText(entryComment);

    if(secondNumber != 0)
    {
        countNumbers++;
        if(thirdNumber != 0)
        {
            countNumbers++;
            if(fourthNumber != 0)
            {
                countNumbers++;
                if(fifthNumber != 0)
                {
                    countNumbers++;
                }
            }
        }
    }

    numbersList = (QStringList()
                   << ui->FirstNumber->text()
                   << ui->SecondNumber->text()
                   << ui->ThirdNumber->text()
                   << ui->FourthNumber->text()
                   << ui->FifthNumber->text());
    days = ui->comboBox->currentText();
    page="1";
    updateCount();
}

void ViewContactDialog::loadAllCalls()
{
    if (!widgets.isEmpty())
        deleteNotesObjects();
    if (!widgetsAllName.isEmpty())
        deleteNameObjects();
    if (!widgetsStatus.isEmpty())
        deleteStatusObjects();

    query4 = new QSqlQueryModel;

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        remainder = count % ui->comboBox_list->currentText().toInt();
        if (remainder)
            remainder = 1;
        else
            remainder = 0;
        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }
    if (go == "previous" && page != "1")
        page = QString::number(page.toInt() - 1);
    else if (go == "previousStart" && page != "1")
        page = "1";
    else if (go == "next" && page.toInt() < pages.toInt())
        page = QString::number(page.toInt() + 1);
    else if (go == "next" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "nextEnd" && page.toInt() < pages.toInt())
        page = pages;
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        page = ui->lineEdit_page->text();
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (go == "default" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT extfield1, src, dst, disposition, datetime, uniqueid, recordpath FROM cdr "
                          "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL'"
                          " OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                          "'"+ days +"' DAY) AND (";
    for (int i = 0; i < countNumbers; i++)
    {
        if(i == 0)
            queryString.append(" src = '"+numbersList[i]+"' OR dst = '"+numbersList[i]+"'");
        else
            queryString.append(" OR src = '"+numbersList[i]+"' OR dst = '"+numbersList[i]+"'");
    }

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append(") ORDER BY datetime DESC LIMIT 0,"
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
       queryString.append(") ORDER BY datetime DESC LIMIT "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
    }

    query4->setQuery(queryString, dbAsterisk);

    query4->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query4->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query4->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query4->insertColumn(4);
    query4->setHeaderData(4, Qt::Horizontal, QObject::tr("Статус"));
    query4->setHeaderData(5, Qt::Horizontal, QObject::tr("Дата и время"));
    query4->insertColumn(6);
    query4->setHeaderData(6, Qt::Horizontal, tr("Заметки"));

    ui->tableView_4->setModel(query4);

    ui->tableView_4->setColumnHidden(3,true);
    ui->tableView_4->setColumnHidden(7, true);
    ui->tableView_4->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {
        extfield1 = query4->data(query4->index(row_index, 0)).toString();
        src = query4->data(query4->index(row_index, 1)).toString();
        uniqueid = query4->data(query4->index(row_index, 7)).toString();
        dialogStatus = query4->data(query4->index(row_index, 3)).toString();

        ui->tableView_4->setIndexWidget(query4->index(row_index, 4), loadStatus());

        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query4->index(row_index, 0), loadName());

        QSqlDatabase db;
        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toInt() != 0)
            ui->tableView_4->setIndexWidget(query4->index(row_index, 6), loadNote());
    }

    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_4->resizeColumnsToContents();
    ui->tableView_4->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    queries.append(query4);
}

QWidget* ViewContactDialog::loadStatus()
{
    QHBoxLayout* statusLayout = new QHBoxLayout;
    QWidget* statusWgt = new QWidget;
    QLabel* statusLabel = new QLabel(statusWgt);

    if (dialogStatus == "NO ANSWER")
        statusLabel->setText(tr("Пропущенный "));
    else if (dialogStatus == "BUSY")
        statusLabel->setText(tr("Занято "));
    else if (dialogStatus == "CANCEL")
        statusLabel->setText(tr("Отколено "));
    else if (dialogStatus == "ANSWERED")
        statusLabel->setText(tr("Принятый "));

    statusLayout->addWidget(statusLabel);
    statusLayout->setContentsMargins(3, 0, 0, 0);
    statusWgt->setLayout(statusLayout);

    layoutsStatus.append(statusLayout);
    widgetsStatus.append(statusWgt);
    labelsStatus.append(statusLabel);
    return statusWgt;
}

void ViewContactDialog::deleteStatusObjects()
{
    for (int i = 0; i < layoutsStatus.size(); ++i)
        layoutsStatus[i]->deleteLater();

    for (int i = 0; i < widgetsStatus.size(); ++i)
        widgetsStatus[i]->deleteLater();

    qDeleteAll(labelsStatus);
    layoutsStatus.clear();
    widgetsStatus.clear();
    labelsStatus.clear();
}

QWidget* ViewContactDialog::loadName()
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);
    nameLabel->setText(src);

    nameLayout->addWidget(nameLabel);
    nameLayout->setContentsMargins(3, 0, 0, 0);
    nameWgt->setLayout(nameLayout);

    if (ui->tabWidget_2->currentIndex() == 0)
    {
        layoutsAllName.append(nameLayout);
        widgetsAllName.append(nameWgt);
        labelsAllName.append(nameLabel);
    }
    if (ui->tabWidget_2->currentIndex() == 1)
    {
        layoutsMissedName.append(nameLayout);
        widgetsMissedName.append(nameWgt);
        labelsMissedName.append(nameLabel);
    }
    if (ui->tabWidget_2->currentIndex() == 2)
    {
        layoutsReceivedName.append(nameLayout);
        widgetsReceivedName.append(nameWgt);
        labelsReceivedName.append(nameLabel);
    }
    if (ui->tabWidget_2->currentIndex() == 3)
    {
        layoutsPlacedName.append(nameLayout);
        widgetsPlacedName.append(nameWgt);
        labelsPlacedName.append(nameLabel);
    }

    return nameWgt;
}

void ViewContactDialog::deleteNameObjects()
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        for (int i = 0; i < layoutsAllName.size(); ++i)
            layoutsAllName[i]->deleteLater();

        for (int i = 0; i < widgetsAllName.size(); ++i)
            widgetsAllName[i]->deleteLater();

        qDeleteAll(labelsAllName);
        layoutsAllName.clear();
        widgetsAllName.clear();
        labelsAllName.clear();
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        for (int i = 0; i < layoutsMissedName.size(); ++i)
            layoutsMissedName[i]->deleteLater();

        for (int i = 0; i < widgetsMissedName.size(); ++i)
            widgetsMissedName[i]->deleteLater();

        qDeleteAll(labelsMissedName);
        layoutsMissedName.clear();
        widgetsMissedName.clear();
        labelsMissedName.clear();
    }
    if (ui->tabWidget->currentIndex() == 2)
    {
        for (int i = 0; i < layoutsReceivedName.size(); ++i)
            layoutsReceivedName[i]->deleteLater();

        for (int i = 0; i < widgetsReceivedName.size(); ++i)
            widgetsReceivedName[i]->deleteLater();

        qDeleteAll(labelsReceivedName);
        layoutsReceivedName.clear();
        widgetsReceivedName.clear();
        labelsReceivedName.clear();
    }
    if (ui->tabWidget->currentIndex() == 3)
    {
        for (int i = 0; i < layoutsPlacedName.size(); ++i)
            layoutsPlacedName[i]->deleteLater();

        for (int i = 0; i < widgetsPlacedName.size(); ++i)
            widgetsPlacedName[i]->deleteLater();

        qDeleteAll(labelsPlacedName);
        layoutsPlacedName.clear();
        widgetsPlacedName.clear();
        labelsPlacedName.clear();
    }
}

void ViewContactDialog::daysChanged()
{
     days = ui->comboBox->currentText();
     go = "default";

     updateCount();
}

void ViewContactDialog::tabSelected()
{
    go = "default";
    page = "1";

    updateCount();
}

void ViewContactDialog::updateCount()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);

    if (ui->tabWidget_2->currentIndex() == 0)
    {
        if(countNumbers == 1)
        {
            query.prepare("SELECT COUNT(*) FROM cdr "
                              "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                              "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                              "'"+ days +"' DAY) AND (dst = '"+numbersList[0]+"' OR src = '"+numbersList[0]+"')");
            query.exec();
            query.first();

        }
        if(countNumbers > 1)
        {
            QString queryString = "SELECT COUNT(*) FROM cdr "
                                  "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                                  "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                                  "'"+ days +"' DAY) AND ( dst = '"+numbersList[0]+"' OR src = '"+numbersList[0]+"'";
            for (int i = 1; i < countNumbers; i++)
            {
                queryString.append(" OR dst = '"+numbersList[i]+"' OR src = '"+numbersList[i]+"'");
                if (i == countNumbers-1)
                     queryString.append(")");
            }
            query.prepare(queryString);
            query.exec();
            query.first();

            count = query.value(0).toInt();
        }
        loadAllCalls();
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE (disposition = 'NO ANSWER'"
                      " OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                      "dst IN ()");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadMissedCalls();
    }
    else if (ui->tabWidget_2->currentIndex() == 2)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE disposition = 'ANSWERED' "
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                      "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$')");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadReceivedCalls();
    }
    else if (ui->tabWidget_2->currentIndex() == 3)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND src = '"+my_number+"'");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadPlacedCalls();
    }
}

void ViewContactDialog::loadMissedCalls()
{
    if (!widgetsMissed.isEmpty())
        deleteNotesObjects();
    if (!widgetsMissedName.isEmpty())
        deleteNameObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    query1 = new QSqlQueryModel;

    QSqlDatabase db;
    QSqlQuery query(db);

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        remainder = count % ui->comboBox_list->currentText().toInt();
        if (remainder)
            remainder = 1;
        else
            remainder = 0;
        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }
    if (go == "previous" && page != "1")
        page = QString::number(page.toInt() - 1);
    else if (go == "previousStart" && page != "1")
        page = "1";
    else if (go == "next" && page.toInt() < pages.toInt())
        page = QString::number(page.toInt() + 1);
    else if (go == "next" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "nextEnd" && page.toInt() < pages.toInt())
        page = pages;
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        page = ui->lineEdit_page->text();
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (go == "default" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE ("
                          "disposition = 'NO ANSWER' OR disposition = 'BUSY' "
                          "OR disposition = 'CANCEL') AND (";
    for (int i = 0; i < countNumbers; i++)
    {
            if(i == 0)
                queryString.append(" src = '"+numbersList[i]+"'");
            else
                queryString.append(" OR src = '"+numbersList[i]+"'");
    }
    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime ");
    if (ui->lineEdit_page->text() == "1")
        queryString.append("DESC LIMIT 0,"
                              + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("DESC LIMIT 0,"
                           + QString::number(ui->lineEdit_page->text().toInt()
                                             * ui->comboBox_list->currentText().toInt() -
                                             ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    query1->setQuery(queryString, dbAsterisk);
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->insertColumn(4);
    query1->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView->setModel(query1);
    ui->tableView->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        extfield1 = query1->data(query1->index(row_index, 0)).toString();
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView->setIndexWidget(query1->index(row_index, 4), loadNote());

        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query1->index(row_index, 0), loadName());
    }
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    queries.append(query1);
}

void ViewContactDialog::loadReceivedCalls()
{
    if (!widgetsReceived.isEmpty())
        deleteNotesObjects();
    if (!widgetsReceivedName.isEmpty())
        deleteNameObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    query2 = new QSqlQueryModel;

    QSqlDatabase db;
    QSqlQuery query(db);

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        remainder = count % ui->comboBox_list->currentText().toInt();
        if (remainder)
            remainder = 1;
        else
            remainder = 0;
        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }
    if (go == "previous" && page != "1")
        page = QString::number(page.toInt() - 1);
    else if (go == "previousStart" && page != "1")
        page = "1";
    else if (go == "next" && page.toInt() < pages.toInt())
        page = QString::number(page.toInt() + 1);
    else if (go == "next" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "nextEnd" && page.toInt() < pages.toInt())
        page = pages;
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        page = ui->lineEdit_page->text();
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (go == "default" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE "
                          "disposition = 'ANSWER' AND (";
    for (int i = 0; i < countNumbers; i++)
    {
        if (i == 0)
            queryString.append(" src = '"+numbersList[i]+"'");
        else
            queryString.append(" OR src = '"+numbersList[i]+"'");
    }
    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime ");
    if (ui->lineEdit_page->text() == "1")
        queryString.append("DESC LIMIT 0,"
                              + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("DESC LIMIT 0,"
                           + QString::number(ui->lineEdit_page->text().toInt()
                                             * ui->comboBox_list->currentText().toInt() -
                                             ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    query2->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query2->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query2->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query2->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query2->insertColumn(4);
    query2->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_2->setModel(query2);
    ui->tableView_2->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        extfield1 = query2->data(query2->index(row_index, 0)).toString();
        uniqueid = query2->data(query2->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_2->setIndexWidget(query2->index(row_index, 4), loadNote());

        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query2->index(row_index, 0), loadName());
    }
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->resizeRowsToContents();
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView_2->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    queries.append(query2);
}

void ViewContactDialog::loadPlacedCalls()
{
    if (!widgetsPlaced.isEmpty())
        deleteNotesObjects();
    if (!widgetsPlacedName.isEmpty())
        deleteNameObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    query3 = new QSqlQueryModel;

    QSqlDatabase db;
    QSqlQuery query(db);

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        remainder = count % ui->comboBox_list->currentText().toInt();
        if (remainder)
            remainder = 1;
        else
            remainder = 0;
        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }
    if (go == "previous" && page != "1")
        page = QString::number(page.toInt() - 1);
    else if (go == "previousStart" && page != "1")
        page = "1";
    else if (go == "next" && page.toInt() < pages.toInt())
        page = QString::number(page.toInt() + 1);
    else if (go == "next" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "nextEnd" && page.toInt() < pages.toInt())
        page = pages;
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        page = ui->lineEdit_page->text();
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (go == "default" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);


    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE "
                          "AND (";
    for (int i = 0; i < countNumbers; i++)
    {
        if (i == 0)
            queryString.append(" dst = '"+numbersList[i]+"'");
        else
            queryString.append(" OR dst = '"+numbersList[i]+"'");
    }
    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime ");
    if (ui->lineEdit_page->text() == "1")
        queryString.append("DESC LIMIT 0,"
                              + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("DESC LIMIT 0,"
                           + QString::number(ui->lineEdit_page->text().toInt()
                                             * ui->comboBox_list->currentText().toInt() -
                                             ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    query3->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query3->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query3->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query3->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query3->insertColumn(4);
    query3->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_3->setModel(query3);
    ui->tableView_3->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        extfield1 = query3->data(query3->index(row_index, 0)).toString();
        uniqueid = query3->data(query3->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_3->setIndexWidget(query3->index(row_index, 4), loadNote());

        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query3->index(row_index, 0), loadName());
    }
    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_3->resizeRowsToContents();
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView_3->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    queries.append(query3);
}

QWidget* ViewContactDialog::loadNote()
{
    QWidget* wgt = new QWidget;
    QLabel *note = new QLabel(wgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid =" + uniqueid + " ORDER BY datetime DESC");
    query.exec();
    query.first();
    note->setText(query.value(0).toString());
    note->setWordWrap(true);

    if(ui->tabWidget_2->currentIndex() == 0)
    {
        widgets.append(wgt);
        notes.append(note);
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        widgetsMissed.append(wgt);
        notesMissed.append(note);
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        widgetsReceived.append(wgt);
        notesReceived.append(note);
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        widgetsReceived.append(wgt);
        notesReceived.append(note);
    }
    return wgt;
}

void ViewContactDialog::deleteNotesObjects()
{
    if(ui->tabWidget_2->currentIndex() == 0)
    {
        for (int i = 0; i < widgets.size(); ++i)
            widgets[i]->deleteLater();
        qDeleteAll(notes);
        widgets.clear();
        notes.clear();
    }
    if(ui->tabWidget_2->currentIndex() == 1)
    {
        for (int i = 0; i < widgetsMissed.size(); ++i)
            widgetsMissed[i]->deleteLater();
        qDeleteAll(notesMissed);
        widgetsMissed.clear();
        notesMissed.clear();
    }
    if(ui->tabWidget_2->currentIndex() == 2)
    {
        for (int i = 0; i < widgetsReceived.size(); ++i)
            widgetsReceived[i]->deleteLater();
        qDeleteAll(notesReceived);
        widgetsReceived.clear();
        notesReceived.clear();
    }
    if(ui->tabWidget_2->currentIndex() == 3)
    {
        for (int i = 0; i < widgetsPlaced.size(); ++i)
            widgetsPlaced[i]->deleteLater();
        qDeleteAll(notesPlaced);
        widgetsPlaced.clear();
        notesPlaced.clear();
    }

    for (int i = 0; i < queries.size(); ++i)
        queries[i]->deleteLater();
    queries.clear();
}

void ViewContactDialog::viewMissedNotes(const QModelIndex &index) {
    uniqueid = query1->data(query1->index(index.row(), 5)).toString();
    state_call = "save_disable";
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::viewRecievedNotes(const QModelIndex &index) {
    uniqueid = query2->data(query2->index(index.row(), 5)).toString();
    state_call = "save_disable";
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::viewPlacedNotes(const QModelIndex &index) {
    uniqueid = query3->data(query3->index(index.row(), 5)).toString();
    state_call = "save_disable";
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::onUpdate()
{
    if(ui->tabWidget_2->currentIndex() == 0)
        loadAllCalls();
    if(ui->tabWidget_2->currentIndex() == 1)
        loadMissedCalls();
    if(ui->tabWidget_2->currentIndex() == 2)
        loadReceivedCalls();
    if(ui->tabWidget_2->currentIndex() == 3)
        loadPlacedCalls();
}

void ViewContactDialog::on_previousButton_clicked()
{
    go = "previous";
    onUpdate();
}

void ViewContactDialog::on_nextButton_clicked()
{
    go = "next";
    onUpdate();
}

void ViewContactDialog::on_previousStartButton_clicked()
{
    go = "previousStart";
    onUpdate();
}

void ViewContactDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";
    onUpdate();;
}

void ViewContactDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";
    onUpdate();
}
