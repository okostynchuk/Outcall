#include "ViewOrgContactDialog.h"
#include "ui_ViewOrgContactDialog.h"

#include "AsteriskManager.h"
#include "Global.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDesktopWidget>

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

    connect(ui->tabWidget_3, &QTabWidget::currentChanged, this, &ViewOrgContactDialog::tabSelected);
    connect(m_horiz_header,  &QHeaderView::sectionClicked, this, &ViewOrgContactDialog::onSectionClicked);
    connect(ui->comboBox_2,  &QComboBox::currentTextChanged, this, &ViewOrgContactDialog::daysChanged);

    connect(ui->playAudio,      &QPushButton::clicked, this, &ViewOrgContactDialog::onPlayAudio);
    connect(ui->playAudioPhone, &QPushButton::clicked, this, &ViewOrgContactDialog::onPlayAudioPhone);

    connect(ui->tableView,   &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::showCard);

    connect(ui->tableView_2, &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::viewNotes);
    connect(ui->tableView_3, &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::viewNotes);
    connect(ui->tableView_4, &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::viewNotes);
    connect(ui->tableView_5, &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::viewNotes);

    connect(ui->tableView_2, &QAbstractItemView::clicked, this, &ViewOrgContactDialog::getData);
    connect(ui->tableView_3, &QAbstractItemView::clicked, this, &ViewOrgContactDialog::getData);
    connect(ui->tableView_4, &QAbstractItemView::clicked, this, &ViewOrgContactDialog::getData);
    connect(ui->tableView_5, &QAbstractItemView::clicked, this, &ViewOrgContactDialog::getData);

    my_number = global::getExtensionNumber("extensions");

    if (!MSSQLopened)
        ui->openAccessButton->hide();

    ui->comboBox_list->setVisible(false);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    ui->tableView->setStyleSheet  ("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_2->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_3->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_4->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_5->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    phonesList = { ui->FirstNumber, ui->SecondNumber, ui->ThirdNumber, ui->FourthNumber, ui->FifthNumber };
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    deleteObjects();
    delete ui;
}

void ViewOrgContactDialog::onAddReminder()
{
    if (!addReminderDialog.isNull())
        addReminderDialog.data()->close();

    addReminderDialog = new AddReminderDialog;
    addReminderDialog.data()->setCallId(contactId);
    addReminderDialog.data()->show();
    addReminderDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::onOpenAccess()
{
    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray password3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(password3));
    QString port_3 = global::getSettingsValue("port_3", "settings").toString();

    QSqlDatabase dbOrders = QSqlDatabase::addDatabase("QODBC", "Orders");
    dbOrders.setDatabaseName("DRIVER={SQL Server Native Client 10.0};"
                            "Server="+hostName_3+","+port_3+";"
                            "Database="+databaseName_3+";"
                            "Uid="+userName_3+";"
                            "Pwd="+password_3);
    dbOrders.open();

    if (dbOrders.isOpen())
    {
        QSqlQuery query(dbOrders);

        query.prepare("INSERT INTO CallTable (UserID, ClientID)"
                    "VALUES (?, ?)");
        query.addBindValue(userID);
        query.addBindValue(ui->VyborID->text().toInt());
        query.exec();

        ui->openAccessButton->setDisabled(true);

        dbOrders.close();
    }
    else
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе заказов!"), QMessageBox::Ok);
}

void ViewOrgContactDialog::receiveDataPerson(bool updating)
{
    if (updating)
    {
        emit sendData(true);

        onUpdate();
    }
}

void ViewOrgContactDialog::receiveDataOrg(bool updating, int x, int y)
{
    int nDesktopHeight;
    int nDesktopWidth;
    int nWidgetHeight = QWidget::height();
    int nWidgetWidth = QWidget::width();

    QDesktopWidget desktop;
    QRect rcDesktop = desktop.availableGeometry(this);

    nDesktopWidth = rcDesktop.width();
    nDesktopHeight = rcDesktop.height();

    if (updating)
    {
        emit sendData(true);

        close();
    }
    else
    {
        if (x < 0 && (nDesktopHeight - y) > nWidgetHeight)
        {
            x = 0;
            this->move(x, y);
        }
        else if (x < 0 && ((nDesktopHeight - y) < nWidgetHeight))
        {
            x = 0;
            y = nWidgetHeight;
            this->move(x, y);
        }
        else if ((nDesktopWidth - x) < nWidgetWidth && (nDesktopHeight - y) > nWidgetHeight)
        {
            x = nWidgetWidth * 0.9;
            this->move(x, y);
        }
        else if ((nDesktopWidth - x) < nWidgetWidth && ((nDesktopHeight - y) < nWidgetHeight))
        {
            x = nWidgetWidth * 0.9;
            y = nWidgetHeight * 0.9;
            this->move(x, y);
        }
        else if (x > 0 && ((nDesktopHeight - y) < nWidgetHeight))
        {
            y = nWidgetHeight * 0.9;
            this->move(x, y);
        }
        else
        {
            this->move(x, y);
        }

        show();
    }
}

void ViewOrgContactDialog::onCall()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT fone FROM fones WHERE entry_id = ?");
    query.addBindValue(contactId);
    query.exec();

    if (query.size() == 1)
    {
        query.next();

        QString number = query.value(0).toString();
        QString protocol = global::getSettingsValue(my_number, "extensions").toString();

        g_pAsteriskManager->originateCall(my_number, number, protocol, my_number);
    }
    else
    {
        if (!chooseNumber.isNull())
            chooseNumber.data()->close();

        chooseNumber = new ChooseNumber;
        chooseNumber.data()->setValuesNumber(contactId);
        chooseNumber.data()->show();
        chooseNumber.data()->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void ViewOrgContactDialog::showCard(const QModelIndex &index)
{   
    QString id = query_model->data(query_model->index(index.row(), 0)).toString();

    viewContactDialog = new ViewContactDialog;
    viewContactDialog->setValuesContacts(id);
    connect(viewContactDialog, &ViewContactDialog::sendData, this, &ViewOrgContactDialog::receiveDataPerson);
    viewContactDialog->show();
    viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::onEdit()
{
    hide();

    editOrgContactDialog = new EditOrgContactDialog;
    editOrgContactDialog->setOrgValuesContacts(contactId);
    connect(editOrgContactDialog, &EditOrgContactDialog::sendData, this, &ViewOrgContactDialog::receiveDataOrg);
    connect(this, &ViewOrgContactDialog::getPos, editOrgContactDialog, &EditOrgContactDialog::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    editOrgContactDialog->show();
    editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}


void ViewOrgContactDialog::onUpdate()
{
    if (update == "default" && filter == false)
    {
        query_model = new QSqlQueryModel;

        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' GROUP BY ep.entry_id ORDER BY entry_name ASC");

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
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' GROUP BY ep.entry_id ORDER BY entry_id");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
            }

            onUpdate();
        }
        else if (update == "sortIDASC")
        {
            update = "sortIDDESC";

            m_horiz_header->setSortIndicator(0, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);

            if (filter == false)
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' GROUP BY ep.entry_id ORDER BY entry_id DESC");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
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
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
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
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' GROUP BY ep.entry_id ORDER BY entry_name");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
            }

            onUpdate();
        }
        else if (update == "sortASC")
        {
            update = "sortDESC";

            m_horiz_header->setSortIndicator(1, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);

            if (filter == false)
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' GROUP BY ep.entry_id ORDER BY entry_name DESC");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
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
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
            }

            onUpdate();
        }
    }
}

void ViewOrgContactDialog::setOrgValuesContacts(QString i)
{
    contactId = i;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + contactId);
    query.exec();

    while (query.next())
         numbersList.append(query.value(0).toString());

    for (int i = 0; i < numbersList.length(); ++i)
        phonesList.at(i)->setText(numbersList.at(i));

    query.prepare("SELECT DISTINCT entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = " + contactId);
    query.exec();
    query.next();

    ui->OrgName->setText(query.value(0).toString());
    ui->City->setText(query.value(1).toString());
    ui->City->QWidget::setToolTip(query.value(1).toString());
    ui->Address->setText(query.value(2).toString());
    ui->Address->QWidget::setToolTip(query.value(2).toString());
    ui->Email->setText(query.value(3).toString());
    ui->Email->QWidget::setToolTip(query.value(3).toString());
    ui->VyborID->setText(query.value(4).toString());
    ui->Comment->setText(query.value(5).toString());

    if (ui->VyborID->text() == "0")
        ui->openAccessButton->hide();

    query_model = new QSqlQueryModel;

    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' GROUP BY ep.entry_id ORDER BY entry_name ASC");

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

    update = "default";

    filter = false;

    days = ui->comboBox_2->currentText();

    page = "1";

    updateCount();
}

void ViewOrgContactDialog::loadAllCalls()
{
    if (!queriesAll.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesAll.append(queryModel);

    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

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

    QString queryString = "SELECT IF(";

    for (int i = 0; i < numbersList.length(); i++)
    {
        if (i == 0)
            queryString.append("src = '"+numbersList[i]+"'");
        else
            queryString.append(" || src = '"+numbersList[i]+"'");
    }

    queryString.append(", extfield2, extfield1), src, dst, disposition, datetime, uniqueid, recordpath FROM cdr "
                          "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL'"
                          " OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                          "'" + days + "' DAY) AND (");

    for (int i = 0; i < numbersList.length(); i++)
    {
        if (i == 0)
            queryString.append(" src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
        else
            queryString.append(" OR src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
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

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Статус"));
    queryModel->setHeaderData(5, Qt::Horizontal, QObject::tr("Дата и время"));
    queryModel->insertColumn(6);
    queryModel->setHeaderData(6, Qt::Horizontal, tr("Заметка"));

    ui->tableView_2->setModel(queryModel);

    ui->tableView_2->setColumnHidden(3,true);
    ui->tableView_2->setColumnHidden(7, true);
    ui->tableView_2->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 7)).toString();
        dialogStatus = queryModel->data(queryModel->index(row_index, 3)).toString();

        ui->tableView_2->setIndexWidget(queryModel->index(row_index, 4), loadStatus());

        if (extfield.isEmpty())
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 0), loadName());

        QSqlDatabase db;
        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 6), loadNote());

            ui->tableView_2->resizeRowToContents(row_index);
        }
        else
            ui->tableView_2->setRowHeight(row_index, 34);
    }

    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_2->resizeColumnsToContents();

    ui->tableView_2->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewOrgContactDialog::loadMissedCalls()
{
    if (!queriesMissed.isEmpty())
        deleteObjects();

    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    queryModel = new QSqlQueryModel;

    queriesMissed.append(queryModel);

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

    QString queryString = "SELECT extfield2, src, dst, datetime, uniqueid FROM cdr WHERE ("
                          "disposition = 'NO ANSWER' OR disposition = 'BUSY' "
                          "OR disposition = 'CANCEL') AND (";

    for (int i = 0; i < numbersList.length(); i++)
    {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");
    }

    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) ORDER BY datetime ");

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

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_3->setModel(queryModel);

    ui->tableView_3->setColumnHidden(5, true);
    ui->tableView_3->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();

        if (extfield.isEmpty())
            ui->tableView_3->setIndexWidget(queryModel->index(row_index, 0), loadName());

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_3->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_3->resizeRowToContents(row_index);
        }
        else
            ui->tableView_3->setRowHeight(row_index, 34);
    }

    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_3->resizeColumnsToContents();

    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewOrgContactDialog::loadReceivedCalls()
{
    if (!queriesReceived.isEmpty())
        deleteObjects();

    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    queryModel = new QSqlQueryModel;

    queriesReceived.append(queryModel);

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

    QString queryString = "SELECT extfield2, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE "
                          "disposition = 'ANSWERED' AND (";

    for (int i = 0; i < numbersList.length(); i++)
    {
        if (i == 0)
            queryString.append(" src = '" + numbersList[i] + "'");
        else
            queryString.append(" OR src = '" + numbersList[i] + "'");
    }

    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) ORDER BY datetime ");

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

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_4->setModel(queryModel);

    ui->tableView_4->setColumnHidden(5, true);
    ui->tableView_4->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {

        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();

        if (extfield.isEmpty())
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 0), loadName());

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_4->resizeRowToContents(row_index);
        }
        else
            ui->tableView_4->setRowHeight(row_index, 34);
    }

    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_4->resizeColumnsToContents();

    ui->tableView_4->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewOrgContactDialog::loadPlacedCalls()
{
    if (!queriesPlaced.isEmpty())
        deleteObjects();

    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    queryModel = new QSqlQueryModel;

    queriesPlaced.append(queryModel);

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

    for (int i = 0; i < numbersList.length(); i++)
    {
        if (i == 0)
            queryString.append(" dst = '" + numbersList[i] + "'");
        else
            queryString.append(" OR dst = '"+numbersList[i] + "'");
    }

    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) ORDER BY datetime ");

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

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_5->setModel(queryModel);

    ui->tableView_5->setColumnHidden(5, true);
    ui->tableView_5->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_5->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();

        if (extfield.isEmpty())
            ui->tableView_5->setIndexWidget(queryModel->index(row_index, 0), loadName());

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_5->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_5->resizeRowToContents(row_index);
        }
        else
            ui->tableView_5->setRowHeight(row_index, 34);
    }

    ui->tableView_5->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_5->resizeColumnsToContents();

    ui->tableView_5->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

QWidget* ViewOrgContactDialog::loadNote()
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* noteLabel = new QLabel(wgt);

    layout->addWidget(noteLabel);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
    query.exec();
    query.first();

    QString note = query.value(0).toString();

    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);

    if (hrefIterator.hasNext())
    {
        QStringList hrefs;

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            hrefs << href;
        }

        for (int i = 0; i < hrefs.length(); ++i)
            note.replace(QRegularExpression("(^|\\s)" + QRegularExpression::escape(hrefs.at(i)) + "(\\s|$)"), QString(" <a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a> "));
    }

    noteLabel->setText(note);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    wgt->setLayout(layout);

    if (ui->tabWidget_3->currentIndex() == 0)
    {
        widgetsAllNotes.append(wgt);
        layoutsAllNotes.append(layout);
        notesAll.append(noteLabel);
    }
    else if (ui->tabWidget_3->currentIndex() == 1)
    {
        widgetsMissedNotes.append(wgt);
        layoutsMissedNotes.append(layout);
        notesMissed.append(noteLabel);
    }
    else if (ui->tabWidget_3->currentIndex() == 2)
    {
        widgetsReceivedNotes.append(wgt);
        layoutsReceivedNotes.append(layout);
        notesReceived.append(noteLabel);
    }
    else if (ui->tabWidget_3->currentIndex() == 3)
    {
        widgetsPlacedNotes.append(wgt);
        layoutsPlacedNotes.append(layout);
        notesPlaced.append(noteLabel);
    }

    return wgt;
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

QWidget* ViewOrgContactDialog::loadName()
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);

    int counter = 0;

    for (int i = 0; i < numbersList.length(); i++)
    {
        if (src == numbersList[i])
        {
            nameLabel->setText(dst);
            counter++;
        }
    }

    if (counter == 0)
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
    else if (ui->tabWidget_3->currentIndex() == 1)
    {
        layoutsMissedName.append(nameLayout);
        widgetsMissedName.append(nameWgt);
        labelsMissedName.append(nameLabel);
    }
    else if (ui->tabWidget_3->currentIndex() == 2)
    {
        layoutsReceivedName.append(nameLayout);
        widgetsReceivedName.append(nameWgt);
        labelsReceivedName.append(nameLabel);
    }
    else if (ui->tabWidget_3->currentIndex() == 3)
    {
        layoutsPlacedName.append(nameLayout);
        widgetsPlacedName.append(nameWgt);
        labelsPlacedName.append(nameLabel);
    }

    return nameWgt;
}

void ViewOrgContactDialog::deleteObjects()
{
    if (ui->tabWidget_3->currentIndex() == 0)
    {
        if (!widgetsAllNotes.isEmpty())
        {
            for (int i = 0; i < widgetsAllNotes.size(); ++i)
                widgetsAllNotes[i]->deleteLater();

            for (int i = 0; i < layoutsAllNotes.size(); ++i)
                layoutsAllNotes[i]->deleteLater();

            for (int i = 0; i < notesAll.size(); ++i)
                notesAll[i]->deleteLater();

            widgetsAllNotes.clear();
            layoutsAllNotes.clear();
            notesAll.clear();
        }

        if (!widgetsAllName.isEmpty())
        {
            for (int i = 0; i < layoutsAllName.size(); ++i)
                layoutsAllName[i]->deleteLater();

            for (int i = 0; i < widgetsAllName.size(); ++i)
                widgetsAllName[i]->deleteLater();

            for (int i = 0; i < labelsAllName.size(); ++i)
                labelsAllName[i]->deleteLater();

            layoutsAllName.clear();
            widgetsAllName.clear();
            labelsAllName.clear();
        }

        if (!widgetsStatus.isEmpty())
        {
            for (int i = 0; i < layoutsStatus.size(); ++i)
                layoutsStatus[i]->deleteLater();

            for (int i = 0; i < widgetsStatus.size(); ++i)
                widgetsStatus[i]->deleteLater();

            for (int i = 0; i < labelsStatus.size(); ++i)
                labelsStatus[i]->deleteLater();

            layoutsStatus.clear();
            widgetsStatus.clear();
            labelsStatus.clear();
        }

        for (int i = 0; i < queriesAll.size(); ++i)
            queriesAll[i]->deleteLater();

        queriesAll.clear();
    }
    else if (ui->tabWidget_3->currentIndex() == 1)
    {
        if (!widgetsMissedNotes.isEmpty())
        {
            for (int i = 0; i < widgetsMissedNotes.size(); ++i)
                widgetsMissedNotes[i]->deleteLater();

            for (int i = 0; i < layoutsMissedNotes.size(); ++i)
                layoutsMissedNotes[i]->deleteLater();

            for (int i = 0; i < notesMissed.size(); ++i)
                notesMissed[i]->deleteLater();

            widgetsMissedNotes.clear();
            layoutsMissedNotes.clear();
            notesMissed.clear();
        }

        if (!widgetsMissedName.isEmpty())
        {
            for (int i = 0; i < layoutsMissedName.size(); ++i)
                layoutsMissedName[i]->deleteLater();

            for (int i = 0; i < widgetsMissedName.size(); ++i)
                widgetsMissedName[i]->deleteLater();

            for (int i = 0; i < labelsMissedName.size(); ++i)
                labelsMissedName[i]->deleteLater();

            layoutsMissedName.clear();
            widgetsMissedName.clear();
            labelsMissedName.clear();
        }

        for (int i = 0; i < queriesMissed.size(); ++i)
            queriesMissed[i]->deleteLater();

        queriesMissed.clear();
    }
    else if (ui->tabWidget_3->currentIndex() == 2)
    {
        if (!widgetsReceivedNotes.isEmpty())
        {
            for (int i = 0; i < widgetsReceivedNotes.size(); ++i)
                widgetsReceivedNotes[i]->deleteLater();

            for (int i = 0; i < layoutsReceivedNotes.size(); ++i)
                layoutsReceivedNotes[i]->deleteLater();

            for (int i = 0; i < notesReceived.size(); ++i)
                notesReceived[i]->deleteLater();

            widgetsReceivedNotes.clear();
            layoutsReceivedNotes.clear();
            notesReceived.clear();
        }

        if (!widgetsReceivedName.isEmpty())
        {
            for (int i = 0; i < layoutsReceivedName.size(); ++i)
                layoutsReceivedName[i]->deleteLater();

            for (int i = 0; i < widgetsReceivedName.size(); ++i)
                widgetsReceivedName[i]->deleteLater();

            for (int i = 0; i < labelsReceivedName.size(); ++i)
                labelsReceivedName[i]->deleteLater();

            layoutsReceivedName.clear();
            widgetsReceivedName.clear();
            labelsReceivedName.clear();
        }

        for (int i = 0; i < queriesReceived.size(); ++i)
            queriesReceived[i]->deleteLater();

        queriesReceived.clear();
    }
    else if (ui->tabWidget_3->currentIndex() == 3)
    {
        if (!widgetsPlacedNotes.isEmpty())
        {
            for (int i = 0; i < widgetsPlacedNotes.size(); ++i)
                widgetsPlacedNotes[i]->deleteLater();

            for (int i = 0; i < layoutsPlacedNotes.size(); ++i)
                layoutsPlacedNotes[i]->deleteLater();

            for (int i = 0; i < notesPlaced.size(); ++i)
                notesPlaced[i]->deleteLater();

            widgetsPlacedNotes.clear();
            layoutsPlacedNotes.clear();
            notesPlaced.clear();
        }

        if (!widgetsPlacedName.isEmpty())
        {
            for (int i = 0; i < layoutsPlacedName.size(); ++i)
                layoutsPlacedName[i]->deleteLater();

            for (int i = 0; i < widgetsPlacedName.size(); ++i)
                widgetsPlacedName[i]->deleteLater();

            for (int i = 0; i < labelsPlacedName.size(); ++i)
                labelsPlacedName[i]->deleteLater();

            layoutsPlacedName.clear();
            widgetsPlacedName.clear();
            labelsPlacedName.clear();
        }

        for (int i = 0; i < queriesPlaced.size(); ++i)
            queriesPlaced[i]->deleteLater();

        queriesPlaced.clear();
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
    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");
    QSqlQuery query(dbCalls);

    if (ui->tabWidget_3->currentIndex() == 0)
    {
        QString queryString = "SELECT COUNT(*) FROM cdr "
                              "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                              "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                              "'" + days + "' DAY) AND ( ";

        for (int i = 0; i < numbersList.length(); i++)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "' OR src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "' OR src = '"+numbersList[i] + "'");

            if (i == numbersList.length() - 1)
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
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < numbersList.length(); i++)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");

            if (i == numbersList.length()-1)
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
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < numbersList.length(); i++)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");

            if (i == numbersList.length() - 1)
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
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < numbersList.length(); i++)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "'");

            if (i == numbersList.length() - 1)
                 queryString.append(")");
        }

        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadPlacedCalls();
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

    query_model = new QSqlQueryModel;

    QString queryString = "SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' ";

    if (ui->comboBox->currentIndex() == 0)
         queryString.append("AND ep.entry_name LIKE '%" + ui->lineEdit->text() + "%' ");
    else if (ui->comboBox->currentIndex() == 1)
        queryString.append("AND ep.entry_phone LIKE '%" + ui->lineEdit->text() + "%' ");
    else if (ui->comboBox->currentIndex() == 2)
        queryString.append("AND ep.entry_comment LIKE '%" + ui->lineEdit->text() + "%' ");


    queryString.append("GROUP BY ep.entry_id ORDER BY entry_name ASC");

    query_model->setQuery(queryString);

    onUpdate();
}

void ViewOrgContactDialog::on_searchButton_clicked()
{
    searchFunction();
}

void ViewOrgContactDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

void ViewOrgContactDialog::on_addPersonToOrg_clicked()
{
    if (!addPersonToOrg.isNull())
        addPersonToOrg.data()->close();

    addPersonToOrg = new AddPersonToOrg;
    addPersonToOrg.data()->setOrgId(contactId);
    connect(addPersonToOrg.data(), &AddPersonToOrg::newPerson, this, &ViewOrgContactDialog::onUpdate);
    addPersonToOrg.data()->show();
    addPersonToOrg.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::viewNotes(const QModelIndex &index)
{
    QString phone;

    if (ui->tabWidget_3->currentIndex() == 0)
        uniqueid = queryModel->data(queryModel->index(index.row(), 7)).toString();
    else
        uniqueid = queryModel->data(queryModel->index(index.row(), 5)).toString();

    notesDialog = new NotesDialog;
    notesDialog->receiveData(uniqueid, phone, "byId");
    notesDialog->hideAddNote();
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
        if (!playAudioDialog.isNull())
            playAudioDialog.data()->close();

        playAudioDialog = new PlayAudioDialog;
        playAudioDialog.data()->setValuesCallHistory(recordpath);
        playAudioDialog.data()->show();
        playAudioDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
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
        QString protocol = global::getSettingsValue(my_number, "extensions").toString();

        g_pAsteriskManager->originateAudio(my_number, protocol, recordpath);
    }
}

void ViewOrgContactDialog::getData(const QModelIndex &index)
{
    if (ui->tabWidget_3->currentIndex() == 0)
        recordpath = queryModel->data(queryModel->index(index.row(), 8)).toString();
    if (ui->tabWidget_3->currentIndex() == 1)
        recordpath = "";
    if (ui->tabWidget_3->currentIndex() == 2 || ui->tabWidget_3->currentIndex() == 3)
        recordpath = queryModel->data(queryModel->index(index.row(), 6)).toString();

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
