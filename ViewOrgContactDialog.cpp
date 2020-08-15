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

    connect(ui->tabWidget_3, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
    connect(m_horiz_header,  SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));
    connect(ui->comboBox_2,  SIGNAL(currentTextChanged(QString)), this, SLOT(daysChanged()));

    connect(ui->playAudio,      &QPushButton::clicked, this, &ViewOrgContactDialog::onPlayAudio);
    connect(ui->playAudioPhone, &QPushButton::clicked, this, &ViewOrgContactDialog::onPlayAudioPhone);

    connect(ui->tableView,   SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));

    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewNotes(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewNotes(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewNotes(const QModelIndex &)));
    connect(ui->tableView_5, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewNotes(const QModelIndex &)));

    connect(ui->tableView_2, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getData(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getData(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getData(const QModelIndex &)));
    connect(ui->tableView_5, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getData(const QModelIndex &)));

    onComboBoxSelected();

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
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    deleteObjects();
    delete ui;
}

void ViewOrgContactDialog::onAddReminder()
{
    addReminderDialog = new AddReminderDialog;
    addReminderDialog->setCallId(updateID);
    addReminderDialog->show();
    addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewOrgContactDialog::onOpenAccess()
{
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
    dbMSSQL.open();

    if (dbMSSQL.isOpen())
    {
        QSqlQuery query(dbMSSQL);

        query.prepare("INSERT INTO CallTable (UserID, ClientID)"
                    "VALUES (?, ?)");
        query.addBindValue(userID);
        query.addBindValue(ui->VyborID->text().toInt());
        query.exec();

        ui->openAccessButton->setDisabled(true);

        dbMSSQL.close();
    }
    else
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе клиентов!"), QMessageBox::Ok);
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
    connect(editOrgContactDialog, SIGNAL(sendData(bool, int, int)), this, SLOT(receiveDataOrg(bool, int, int)));

    connect(this, SIGNAL(getPos(int, int)), editOrgContactDialog, SLOT(setPos(int, int)));
    emit getPos(this->pos().x(), this->pos().y());

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
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_id");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");              
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
            }

            onUpdate();
        }
        else if (update == "sortIDASC")
        {
            update = "sortIDDESC";

            m_horiz_header->setSortIndicator(0, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);

            if (filter == false)
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_id DESC");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
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
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
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
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_name");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
            }

            onUpdate();
        }
        else if (update == "sortASC")
        {
            update = "sortDESC";

            m_horiz_header->setSortIndicator(1, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);

            if (filter == false)
                query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY entry_name DESC");
            else
            {
                if (entry_name != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
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
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                else if (entry_phone != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                else if (entry_comment != "NULL")
                    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
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
    ui->City->QWidget::setToolTip(entryCity);
    ui->Address->setText(entryAddress);    
    ui->Address->QWidget::setToolTip(entryAddress);
    ui->Email->setText(entryEmail);
    ui->Email->QWidget::setToolTip(entryEmail);
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

    QString queryString = "SELECT if(";

    for (int i = 0; i < countNumbers; i++)
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

    for (int i = 0; i < countNumbers; i++)
    {
        if(i == 0)
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

    queryModel->setQuery(queryString, dbAsterisk);

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

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

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

    for (int i = 0; i < countNumbers; i++)
    {
            if(i == 0)
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

    queryModel->setQuery(queryString, dbAsterisk);

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

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

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

    for (int i = 0; i < countNumbers; i++)
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

    queryModel->setQuery(queryString, dbAsterisk);

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

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_4->resizeRowToContents(row_index);
        }
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

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

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

    for (int i = 0; i < countNumbers; i++)
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

    queryModel->setQuery(queryString, dbAsterisk);

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

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_5->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_5->resizeRowToContents(row_index);
        }
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

            if (!hrefs.contains(href))
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

    for (int i = 0; i < countNumbers; i++)
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
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);

    if (ui->tabWidget_3->currentIndex() == 0)
    {
        QString queryString = "SELECT COUNT(*) FROM cdr "
                              "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                              "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                              "'" + days + "' DAY) AND ( ";

        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "' OR src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "' OR src = '"+numbersList[i] + "'");

            if (i == countNumbers - 1)
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

        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");

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
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");

            if (i == countNumbers - 1)
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

        for (int i = 0; i < countNumbers; i++)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "'");

            if (i == countNumbers - 1)
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

void ViewOrgContactDialog::viewNotes(const QModelIndex &index)
{
    QString phone;

    if(ui->tabWidget_3->currentIndex() == 0)
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
