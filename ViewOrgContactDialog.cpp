#include "ViewOrgContactDialog.h"
#include "ui_ViewOrgContactDialog.h"

#include "AsteriskManager.h"
#include "Global.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

ViewOrgContactDialog::ViewOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewOrgContactDialog)
{
    ui->setupUi(this);

    userID = global::getSettingsValue("user_login", "settings").toString();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    m_horiz_header = ui->tableView->horizontalHeader();

    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_4->verticalHeader()->setSectionsClickable(false);
    ui->tableView_4->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_5->verticalHeader()->setSectionsClickable(false);
    ui->tableView_5->horizontalHeader()->setSectionsClickable(false);

    connect(ui->openAccessButton,  &QPushButton::clicked, this, &ViewOrgContactDialog::onOpenAccess);
    connect(ui->addReminderButton, &QPushButton::clicked, this, &ViewOrgContactDialog::onAddReminder);
    connect(ui->callButton, &QPushButton::clicked, this, &ViewOrgContactDialog::onCall);
    connect(ui->editButton, &QPushButton::clicked, this, &ViewOrgContactDialog::onEdit);

    connect(ui->tabWidget_3,SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
    connect(m_horiz_header, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));
    connect(ui->comboBox_2, SIGNAL(currentTextChanged(QString)), this, SLOT(daysChanged()));
    connect(ui->playAudio,         &QPushButton::clicked, this, &ViewOrgContactDialog::onPlayAudio);
    connect(ui->playAudioPhone,    &QPushButton::clicked, this, &ViewOrgContactDialog::onPlayAudioPhone);

    connect(ui->tableView,   SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewAllNotes(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewMissedNotes(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewRecievedNotes(const QModelIndex &)));
    connect(ui->tableView_5, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewPlacedNotes(const QModelIndex &)));

    connect(ui->tableView_2, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getDataAll(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getDataMissed()));
    connect(ui->tableView_4, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getDataReceived(const QModelIndex &)));
    connect(ui->tableView_5, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getDataPlaced(const QModelIndex &)));

    onComboBoxSelected();

    my_number = global::getExtensionNumber("extensions");

    if (!MSSQLopened)
        ui->openAccessButton->hide();

    ui->comboBox_list->setVisible(false);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    delete ui;
}

void ViewOrgContactDialog::onAddReminder()
{
    addReminderDialog = new AddReminderDialog;
    addReminderDialog->setCallId(updateID);
    connect(addReminderDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addReminderDialog->show();
    addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::onOpenAccess() {
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

void ViewOrgContactDialog::receiveDataPerson(bool updating)
{
    if (updating)
    {
        emit sendData(true);

        onUpdate();
    }
}

void ViewOrgContactDialog::receiveDataOrg(bool updating)
{
    if (updating)
    {
        emit sendData(true);

        destroy(true);
    }
    else
        show();
}

void ViewOrgContactDialog::receiveNumber(QString &to)
{
    const QString from = my_number;
    const QString protocol = global::getSettingsValue(from, "extensions").toString();

    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

void ViewOrgContactDialog::onCall()
{
    chooseNumber = new ChooseNumber;
    chooseNumber->setValuesNumber(updateID);
    connect(chooseNumber, SIGNAL(sendNumber(QString &)), this, SLOT(receiveNumber(QString &)));
    chooseNumber->show();
    chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::showCard(const QModelIndex &index)
{   
    QString id = query_model->data(query_model->index(index.row(), 0)).toString();

    viewContactDialog = new ViewContactDialog;
    viewContactDialog->setValuesContacts(id);
    connect(viewContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveDataPerson(bool)));
    viewContactDialog->show();
    viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::onEdit()
{
    hide();

    editOrgContactDialog = new EditOrgContactDialog;
    editOrgContactDialog->setOrgValuesContacts(updateID);
    connect(editOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveDataOrg(bool)));
    editOrgContactDialog->show();
    editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}


void ViewOrgContactDialog::onUpdate()
{
    if (update == "default" && filter == false)
    {
        query_model = new QSqlQueryModel;
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_name ASC");
        m_horiz_header->setSortIndicatorShown(false);
    }

    query_model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query_model->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО"));
    query_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Заметка"));

    ui->tableView->setModel(query_model);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void ViewOrgContactDialog::onUpdateCalls()
{
    if (ui->tabWidget_3->currentIndex() == 0)
        loadAllCalls();
    else if (ui->tabWidget_3->currentIndex() == 1)
        loadMissedCalls();
    else if (ui->tabWidget_3->currentIndex() == 2)
        loadReceivedCalls();
    else if (ui->tabWidget_3->currentIndex() == 3)
        loadPlacedCalls();
}

void ViewOrgContactDialog::onComboBoxSelected()
{
    ui->comboBox->addItem(tr("Поиск по ФИО"));
    ui->comboBox->addItem(tr("Поиск по номеру телефона"));
    ui->comboBox->addItem(tr("Поиск по заметке"));
}

void ViewOrgContactDialog::onSectionClicked(int logicalIndex)
{
    if (logicalIndex != 0 && logicalIndex != 1 && sort == "id")
    {
        if (update == "sortIDASC")
        {
            m_horiz_header->setSortIndicator(0, Qt::AscendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
        }
        else if (update == "sortIDDESC")
        {
            m_horiz_header->setSortIndicator(0, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
        }
        else if (update == "default")
            m_horiz_header->setSortIndicatorShown(false);
        return;
    }
    else if (logicalIndex == 0)
    {
        query_model = new QSqlQueryModel;
        if (sort == "name")
            update = "default";
        sort = "id";
        if (update == "default")
        {
            update = "sortIDASC";
            m_horiz_header->setSortIndicator(0, Qt::AscendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
            if (filter == false)
            {
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_id");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                }
                else if (entry_phone != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                }
                else if (entry_comment != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                }
            }
            onUpdate();
        }
        else if (update == "sortIDASC")
        {
            update = "sortIDDESC";
            m_horiz_header->setSortIndicator(0, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
            if (filter == false)
            {
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_id DESC");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                }
                else if (entry_phone != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                }
                else if (entry_comment != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                }
            }
            onUpdate();
        }
        else if (update == "sortIDDESC")
        {
            update = "default";
            m_horiz_header->setSortIndicatorShown(false);
            if (filter == true)
            {
                if (entry_name != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                }
                else if (entry_phone != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                }
                else if (entry_comment != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
                }
            }
            onUpdate();
        }
    }
    else if (logicalIndex != 0 && logicalIndex != 1 && sort == "name")
    {
        if (update == "sortASC")
        {
            m_horiz_header->setSortIndicator(1, Qt::AscendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
        }
        else if (update == "sortDESC")
        {
            m_horiz_header->setSortIndicator(1, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
        }
        else if (update == "default")
            m_horiz_header->setSortIndicatorShown(false);
        return;
    }
    else if (logicalIndex == 1)
    {
        query_model = new QSqlQueryModel;
        if (sort == "id")
            update = "default";
        sort = "name";
        if (update == "default")
        {
            update = "sortASC";
            m_horiz_header->setSortIndicator(1, Qt::AscendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
            if (filter == false)
            {
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_name");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                }
                else if (entry_phone != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                }
                else if (entry_comment != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                }
            }
            onUpdate();
        }
        else if (update == "sortASC")
        {
            update = "sortDESC";
            m_horiz_header->setSortIndicator(1, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
            if (filter == false)
            {
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_name DESC");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                }
                else if (entry_phone != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                }
                else if (entry_comment != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                }
            }
            onUpdate();
        }
        else if (update == "sortDESC")
        {
            update = "default";
            m_horiz_header->setSortIndicatorShown(false);
            if (filter == true)
            {
                if (entry_name != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                }
                else if (entry_phone != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                }
                else if (entry_comment != "NULL")
                {
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
                }
            }
            onUpdate();
        }
    }
}

void ViewOrgContactDialog::setOrgValuesContacts(QString &i)
{
    updateID = i;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM entry_phone WHERE entry_id = " + updateID);
    query.exec();
    query.first();

    countNumbers = query.value(0).toInt();

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + updateID);
    query.exec();
    query.next();

    for (int i = 0; i < countNumbers; i++)
    {
        if (i == 0)
            ui->FirstNumber->setText(query.value(0).toString());
        if (i == 1)
            ui->SecondNumber->setText(query.value(0).toString());
        if (i == 2)
            ui->ThirdNumber->setText(query.value(0).toString());
        if (i == 3)
            ui->FourthNumber->setText(query.value(0).toString());
        if (i == 4)
            ui->FifthNumber->setText(query.value(0).toString());
        query.next();
    }

    numbersList = (QStringList()
                       << ui->FirstNumber->text()
                       << ui->SecondNumber->text()
                       << ui->ThirdNumber->text()
                       << ui->FourthNumber->text()
                       << ui->FifthNumber->text());

    query.prepare("SELECT DISTINCT entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = " + updateID);
    query.exec();
    query.next();

    QString entryORGName = query.value(0).toString();
    QString entryCity = query.value(1).toString();
    QString entryAddress = query.value(2).toString();
    QString entryEmail = query.value(3).toString();
    QString entryVyborID = query.value(4).toString();
    QString entryComment = query.value(5).toString();

    ui->OrgName->setText(entryORGName);
    ui->City->setText(entryCity);
    ui->Address->setText(entryAddress);
    ui->Email->setText(entryEmail);
    ui->VyborID->setText(entryVyborID);
    ui->Comment->setText(entryComment);

    query_model = new QSqlQueryModel;

    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_name ASC");

    query_model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query_model->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО"));
    query_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Заметка"));

    ui->tableView->setModel(query_model);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    ui->tableView->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    update = "default";
    filter = false;

    days = ui->comboBox_2->currentText();

    page = "1";

    updateCount();
}

void ViewOrgContactDialog::setOrgValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}


void ViewOrgContactDialog::loadAllCalls()
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

    ui->tableView_2->setModel(query4);

    ui->tableView_2->setColumnHidden(3,true);
    ui->tableView_2->setColumnHidden(7, true);
    ui->tableView_2->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        extfield1 = query4->data(query4->index(row_index, 0)).toString();
        src = query4->data(query4->index(row_index, 1)).toString();
        uniqueid = query4->data(query4->index(row_index, 7)).toString();
        dialogStatus = query4->data(query4->index(row_index, 3)).toString();

        ui->tableView_2->setIndexWidget(query4->index(row_index, 4), loadStatus());

        if (extfield1.isEmpty())
            ui->tableView_2->setIndexWidget(query4->index(row_index, 0), loadName());

        QSqlDatabase db;
        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toInt() != 0)
            ui->tableView_2->setIndexWidget(query4->index(row_index, 6), loadNote());
    }

    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

QWidget* ViewOrgContactDialog::loadStatus()
{
    QHBoxLayout* statusLayout = new QHBoxLayout;
    QWidget* statusWgt = new QWidget;
    QLabel* statusLabel = new QLabel(statusWgt);

    if (dialogStatus == "NO ANSWER")
        statusLabel->setText(tr("Пропущенный "));
    else if (dialogStatus == "BUSY")
        statusLabel->setText(tr("Занято "));
    else if (dialogStatus == "CANCEL")
        statusLabel->setText(tr("Отклонено "));
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

void ViewOrgContactDialog::deleteStatusObjects()
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

QWidget* ViewOrgContactDialog::loadName()
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);
    nameLabel->setText(src);

    nameLayout->addWidget(nameLabel);
    nameLayout->setContentsMargins(3, 0, 0, 0);
    nameWgt->setLayout(nameLayout);

    if (ui->tabWidget_3->currentIndex() == 0)
    {
        layoutsAllName.append(nameLayout);
        widgetsAllName.append(nameWgt);
        labelsAllName.append(nameLabel);
    }
    if (ui->tabWidget_3->currentIndex() == 1)
    {
        layoutsMissedName.append(nameLayout);
        widgetsMissedName.append(nameWgt);
        labelsMissedName.append(nameLabel);
    }
    if (ui->tabWidget_3->currentIndex() == 2)
    {
        layoutsReceivedName.append(nameLayout);
        widgetsReceivedName.append(nameWgt);
        labelsReceivedName.append(nameLabel);
    }
    if (ui->tabWidget_3->currentIndex() == 3)
    {
        layoutsPlacedName.append(nameLayout);
        widgetsPlacedName.append(nameWgt);
        labelsPlacedName.append(nameLabel);
    }

    return nameWgt;
}

void ViewOrgContactDialog::deleteNameObjects()
{
    if (ui->tabWidget_3->currentIndex() == 0)
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
    if (ui->tabWidget_3->currentIndex() == 1)
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
    if (ui->tabWidget_3->currentIndex() == 2)
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
    if (ui->tabWidget_3->currentIndex() == 3)
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

void ViewOrgContactDialog::daysChanged()
{
     days = ui->comboBox_2->currentText();
     go = "default";
     updateCount();
}

void ViewOrgContactDialog::tabSelected()
{
    go = "default";
    page = "1";
    updateCount();
}

void ViewOrgContactDialog::updateCount()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);

    if (ui->tabWidget_3->currentIndex() == 0)
    {

        QString queryString = "SELECT COUNT(*) FROM cdr "
                              "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                              "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                              "'"+ days +"' DAY) AND ( ";
        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" dst = '"+numbersList[i]+"' OR src = '"+numbersList[i]+"'");
            else
                queryString.append(" OR dst = '"+numbersList[i]+"' OR src = '"+numbersList[i]+"'");
            if (i == countNumbers-1)
                 queryString.append(")");
        }
        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadAllCalls();
    }
    else if (ui->tabWidget_3->currentIndex() == 1)
    {
        QString queryString = ("SELECT COUNT(*) FROM cdr WHERE (disposition = 'NO ANSWER'"
                      " OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND (");
        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" src = '"+numbersList[i]+"'");
            else
                queryString.append(" OR src = '"+numbersList[i]+"'");
            if (i == countNumbers-1)
                 queryString.append(")");
        }

        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadMissedCalls();
    }
    else if (ui->tabWidget_3->currentIndex() == 2)
    {
        QString queryString = ("SELECT COUNT(*) FROM cdr WHERE disposition = 'ANSWERED' "
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND (");
        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" src = '"+numbersList[i]+"'");
            else
                queryString.append(" OR src = '"+numbersList[i]+"'");
            if (i == countNumbers-1)
                 queryString.append(")");
        }
        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadReceivedCalls();
    }
    else if (ui->tabWidget_3->currentIndex() == 3)
    {
        QString queryString = ("SELECT COUNT(*) FROM cdr WHERE "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND (");

        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" dst = '"+numbersList[i]+"'");
            else
                queryString.append(" OR dst = '"+numbersList[i]+"'");
            if (i == countNumbers-1)
                 queryString.append(")");
        }
        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadPlacedCalls();
    }
}

void ViewOrgContactDialog::loadMissedCalls()
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

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE ("
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
        queryString.append("DESC LIMIT "
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

    ui->tableView_3->setModel(query1);
    ui->tableView_3->setColumnHidden(5, true);
    ui->tableView_3->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {

        extfield1 = query1->data(query1->index(row_index, 0)).toString();
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_3->setIndexWidget(query1->index(row_index, 4), loadNote());

        if (extfield1.isEmpty())
            ui->tableView_3->setIndexWidget(query1->index(row_index, 0), loadName());
    }

    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView_3->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewOrgContactDialog::loadReceivedCalls()
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

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE "
                          "disposition = 'ANSWERED' AND (";
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
        queryString.append("DESC LIMIT "
                         + QString::number(ui->lineEdit_page->text().toInt()
                                           * ui->comboBox_list->currentText().toInt() -
                                           ui->comboBox_list->currentText().toInt()) + " , " +
                         QString::number(ui->comboBox_list->currentText().toInt()));

    query2->setQuery(queryString, dbAsterisk);
    query2->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query2->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query2->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query2->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query2->insertColumn(4);
    query2->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_4->setModel(query2);
    ui->tableView_4->setColumnHidden(5, true);
    ui->tableView_4->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {

        extfield1 = query2->data(query2->index(row_index, 0)).toString();
        uniqueid = query2->data(query2->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_4->setIndexWidget(query2->index(row_index, 4), loadNote());

        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query2->index(row_index, 0), loadName());
    }
    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_4->resizeColumnsToContents();
    ui->tableView_4->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView_4->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewOrgContactDialog::loadPlacedCalls()
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

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE (";
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
        queryString.append("DESC LIMIT "
                           + QString::number(ui->lineEdit_page->text().toInt()
                                             * ui->comboBox_list->currentText().toInt() -
                                             ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    query3->setQuery(queryString, dbAsterisk);
    query3->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query3->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query3->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query3->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query3->insertColumn(4);
    query3->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_5->setModel(query3);
    ui->tableView_5->setColumnHidden(5, true);
    ui->tableView_5->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_5->model()->rowCount(); ++row_index)
    {

        extfield1 = query3->data(query3->index(row_index, 0)).toString();
        uniqueid = query3->data(query3->index(row_index, 5)).toString();

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_5->setIndexWidget(query3->index(row_index, 4), loadNote());

        if (extfield1.isEmpty())
            ui->tableView_5->setIndexWidget(query3->index(row_index, 0), loadName());
    }

    ui->tableView_5->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_5->resizeColumnsToContents();
    ui->tableView_5->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView_5->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

QWidget* ViewOrgContactDialog::loadNote()
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

    if(ui->tabWidget_3->currentIndex() == 0)
    {
        widgets.append(wgt);
        notes.append(note);
    }
    else if (ui->tabWidget_3->currentIndex() == 1)
    {
        widgetsMissed.append(wgt);
        notesMissed.append(note);
    }
    else if (ui->tabWidget_3->currentIndex() == 2)
    {
        widgetsReceived.append(wgt);
        notesReceived.append(note);
    }
    else if (ui->tabWidget_3->currentIndex() == 3)
    {
        widgetsPlaced.append(wgt);
        notesPlaced.append(note);
    }
    return wgt;
}

void ViewOrgContactDialog::deleteNotesObjects()
{
    if (ui->tabWidget_3->currentIndex() == 0)
    {
        for (int i = 0; i < widgets.size(); ++i)
            widgets[i]->deleteLater();
        qDeleteAll(notes);
        widgets.clear();
        notes.clear();
    }
    else if (ui->tabWidget_3->currentIndex() == 1)
    {
        for (int i = 0; i < widgetsMissed.size(); ++i)
            widgetsMissed[i]->deleteLater();
        qDeleteAll(notesMissed);
        widgetsMissed.clear();
        notesMissed.clear();
    }
    else if (ui->tabWidget_3->currentIndex() == 2)
    {
        for (int i = 0; i < widgetsReceived.size(); ++i)
            widgetsReceived[i]->deleteLater();
        qDeleteAll(notesReceived);
        widgetsReceived.clear();
        notesReceived.clear();
    }
    else if (ui->tabWidget_3->currentIndex() == 3)
    {
        for (int i = 0; i < widgetsPlaced.size(); ++i)
            widgetsPlaced[i]->deleteLater();
        qDeleteAll(notesPlaced);
        widgetsPlaced.clear();
        notesPlaced.clear();
    }
}

void ViewOrgContactDialog::searchFunction()
{
    if (ui->lineEdit->text().isEmpty())
    {
        update = "default";
        filter = false;
        onUpdate();
        return;
    }

    update = "default";
    filter = true;
    m_horiz_header->setSortIndicatorShown(false);

    entry_name = "NULL";
    entry_phone = "NULL";
    entry_comment = "NULL";

    if (ui->comboBox->currentText() == tr("Поиск по ФИО"))
    {
        query_model = new QSqlQueryModel;
        entry_name = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY entry_name ASC");
        onUpdate();
    }
    else if (ui->comboBox->currentText() == tr("Поиск по номеру телефона"))
    {
        query_model = new QSqlQueryModel;
        entry_phone = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY entry_name ASC");
        onUpdate();
    }
    else if (ui->comboBox->currentText() == tr("Поиск по заметке"))
    {
        query_model = new QSqlQueryModel;
        entry_comment = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY entry_name ASC");
        onUpdate();
    }
}

void ViewOrgContactDialog::on_pushButton_clicked()
{
    searchFunction();
}

void ViewOrgContactDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

void ViewOrgContactDialog::receivePersonID(QString &id)
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE entry SET entry_person_org_id = '" + updateID + "' WHERE id = " + id);
    query.exec();

    onUpdate();
}

void ViewOrgContactDialog::on_addPersonToOrg_clicked()
{
    addPersonToOrg = new AddPersonToOrg;
    connect(addPersonToOrg, SIGNAL(sendPersonID(QString&)), this, SLOT(receivePersonID(QString&)));
    addPersonToOrg->show();
    addPersonToOrg->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::viewMissedNotes(const QModelIndex &index)
{
    uniqueid = query1->data(query1->index(index.row(), 5)).toString();

    state_call = "save_disable";

    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::viewRecievedNotes(const QModelIndex &index)
{
    uniqueid = query2->data(query2->index(index.row(), 5)).toString();

    state_call = "save_disable";

    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::viewPlacedNotes(const QModelIndex &index)
{
    uniqueid = query3->data(query3->index(index.row(), 5)).toString();

    state_call = "save_disable";

    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::viewAllNotes(const QModelIndex &index)
{
    uniqueid = query4->data(query4->index(index.row(), 7)).toString();

    state_call = "save_disable";

    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::on_previousButton_clicked()
{
    go = "previous";

    onUpdateCalls();
}

void ViewOrgContactDialog::on_nextButton_clicked()
{
    go = "next";

    onUpdateCalls();
}

void ViewOrgContactDialog::on_previousStartButton_clicked()
{
    go = "previousStart";

    onUpdateCalls();
}

void ViewOrgContactDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";

    onUpdateCalls();;
}

void ViewOrgContactDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";

    onUpdateCalls();
}

void ViewOrgContactDialog::onPlayAudio()
{
    if ((ui->tabWidget_3->currentIndex() == 0 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_3->currentIndex() == 1 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_3->currentIndex() == 2 && ui->tableView_4->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_3->currentIndex() == 3 && ui->tableView_5->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    if (!recordpath.isEmpty())
    {
        if (playAudioDialog != nullptr)
            playAudioDialog->close();

        playAudioDialog = new PlayAudioDialog;
        playAudioDialog->setValuesCallHistory(recordpath);
        connect(playAudioDialog, SIGNAL(isClosed(bool)), this, SLOT(playerClosed(bool)));
        playAudioDialog->show();
        playAudioDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void ViewOrgContactDialog::onPlayAudioPhone()
{
    if ((ui->tabWidget_3->currentIndex() == 0 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_3->currentIndex() == 1 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_3->currentIndex() == 2 && ui->tableView_4->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_3->currentIndex() == 3 && ui->tableView_5->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    if (!recordpath.isEmpty())
    {
        const QString protocol = global::getSettingsValue(my_number, "extensions").toString();
        g_pAsteriskManager->originateAudio(my_number, protocol, recordpath);
    }
}

void ViewOrgContactDialog::playerClosed(bool closed)
{
    if (closed)
        playAudioDialog = nullptr;
}

void ViewOrgContactDialog::getDataAll(const QModelIndex &index)
{
    recordpath = query4->data(query4->index(index.row(), 8)).toString();

    if (!recordpath.isEmpty())
    {
        ui->playAudio->setDisabled(false);
        ui->playAudioPhone->setDisabled(false);
    }
    else
    {
        ui->playAudio->setDisabled(true);
        ui->playAudioPhone->setDisabled(true);
    }
}

void ViewOrgContactDialog::getDataMissed()
{
    recordpath = "";

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewOrgContactDialog::getDataReceived(const QModelIndex &index)
{
    recordpath = query2->data(query2->index(index.row(), 6)).toString();

    if (!recordpath.isEmpty())
    {
        ui->playAudio->setDisabled(false);
        ui->playAudioPhone->setDisabled(false);
    }
    else
    {
        ui->playAudio->setDisabled(true);
        ui->playAudioPhone->setDisabled(true);
    }
}

void ViewOrgContactDialog::getDataPlaced(const QModelIndex &index)
{
    recordpath = query3->data(query3->index(index.row(), 6)).toString();

    if (!recordpath.isEmpty())
    {
        ui->playAudio->setDisabled(false);
        ui->playAudioPhone->setDisabled(false);
    }
    else
    {
        ui->playAudio->setDisabled(true);
        ui->playAudioPhone->setDisabled(true);
    }
}

