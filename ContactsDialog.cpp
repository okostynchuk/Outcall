#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QGuiApplication>
#include <QScreen>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMetaMethod>
#include <QMessageBox>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);

    this->showMaximized();
    ui->widget->move(0, 0);
    ui->widget->resize(QGuiApplication::screens().at(0)->geometry().width(), QGuiApplication::screens().at(0)->geometry().height());

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;
    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
    query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, tr("Тип"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Город"));
    query1->setHeaderData(5, Qt::Horizontal, QObject::tr("Адрес"));
    query1->setHeaderData(6, Qt::Horizontal, QObject::tr("Email"));
    query1->setHeaderData(7, Qt::Horizontal, QObject::tr("VyborID"));
    query1->setHeaderData(8, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(query1);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    m_horiz_header = ui->tableView->horizontalHeader();

    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->addOrgButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton, &QAbstractButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));
    connect(m_horiz_header, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query1->index(row_index, 1), addImageLabel(row_index));

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);

    onComboBoxSelected();

    update = "default";
    filter = false;
}

ContactsDialog::~ContactsDialog()
{
    delete query1;
    delete query2;
    delete ui;
}

void ContactsDialog::recieveData(bool updating)
{
    if (updating)
    {
        query1->setQuery(query1->query().lastQuery());
        query2->setQuery(query2->query().lastQuery());
        if (update == "sortASC")
            m_horiz_header->setSortIndicator(1, Qt::AscendingOrder);
        else if (update == "sortDESC")
            m_horiz_header->setSortIndicator(1, Qt::DescendingOrder);
        else if (update == "sortIDASC")
            m_horiz_header->setSortIndicator(0, Qt::AscendingOrder);
        else if (update == "sortIDDESC")
            m_horiz_header->setSortIndicator(0, Qt::DescendingOrder);
        else if (update == "default")
            m_horiz_header->setSortIndicatorShown(false);
        onUpdate();
    }
}

void ContactsDialog::showCard(const QModelIndex &index)
{
    QString updateID = query1->data(query1->index(index.row(), 0)).toString();
    int row = ui->tableView->currentIndex().row();
    if (query2->data(query2->index(row, 0)).toString() == "person")
    {
         viewContactDialog = new ViewContactDialog;
         viewContactDialog->setValuesContacts(updateID);
         connect(viewContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
         viewContactDialog->exec();
         viewContactDialog->deleteLater();
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        viewOrgContactDialog->exec();
        viewOrgContactDialog->deleteLater();
    }
}

void ContactsDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
    {
        widgets[i]->deleteLater();
    }
    qDeleteAll(layouts);
    qDeleteAll(labels);
    widgets.clear();
    layouts.clear();
    labels.clear();
}

void ContactsDialog::onUpdate()
{
    if (update == "default" && filter == false)
    {
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
        m_horiz_header->setSortIndicatorShown(false);
    }

    valueV = ui->tableView->verticalScrollBar()->sliderPosition();
    valueH = ui->tableView->horizontalScrollBar()->sliderPosition();

    deleteObjects();

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, tr("Тип"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Город"));
    query1->setHeaderData(5, Qt::Horizontal, QObject::tr("Адрес"));
    query1->setHeaderData(6, Qt::Horizontal, QObject::tr("Email"));
    query1->setHeaderData(7, Qt::Horizontal, QObject::tr("VyborID"));
    query1->setHeaderData(8, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(query1);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query1->index(row_index, 1), addImageLabel(row_index));

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);

    ui->tableView->verticalScrollBar()->setSliderPosition(valueV);
    ui->tableView->horizontalScrollBar()->setSliderPosition(valueH);
}

void ContactsDialog::onAddPerson()
{
    addContactDialog = new AddContactDialog;
    connect(addContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addContactDialog->exec();
    addContactDialog->deleteLater();
}

void ContactsDialog::onAddOrg()
{
    addOrgContactDialog = new AddOrgContactDialog;
    connect(addOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addOrgContactDialog->exec();
    addOrgContactDialog->deleteLater();
}

QWidget* ContactsDialog::addImageLabel(int &row_index)
{
    QWidget* wgt = new QWidget;
    QBoxLayout* l = new QHBoxLayout;
    QLabel *imageLabel = new QLabel(wgt);
    l->addWidget(imageLabel);
    if (query2->data(query2->index(row_index, 0)).toString() == "person")
    {
        imageLabel->setPixmap(QPixmap(":/images/person.png").scaled(30, 30, Qt::KeepAspectRatio));
    }
    else
    {
        imageLabel->setPixmap(QPixmap(":/images/org.png").scaled(30, 30, Qt::KeepAspectRatio));
    }
    wgt->setLayout(l);
    widgets.append(wgt);
    layouts.append(l);
    labels.append(imageLabel);
    return wgt;
}

void ContactsDialog::onComboBoxSelected()
{
    ui->comboBox->addItem("Поиск по ФИО / названию");
    ui->comboBox->addItem("Поиск по номеру телефона");
    ui->comboBox->addItem("Поиск по заметке");
}

void ContactsDialog::onSectionClicked(int logicalIndex)
{
    if (logicalIndex != 0 && logicalIndex != 2 && sort == "id")
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
                query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id ORDER BY ep.entry_id");
                query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id ORDER BY entry_id");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_id");
                }
                else if (entry_phone != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_id");
                }
                else if (entry_comment != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_id");
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
                query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id ORDER BY entry_id DESC");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_id DESC");
                }
                else if (entry_phone != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_id DESC");
                }
                else if (entry_comment != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_id DESC");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_id DESC");
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
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id");
                }
                else if (entry_phone != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id");
                }
                else if (entry_comment != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id");
                }
            }
            onUpdate();
        }
    }
    else if (logicalIndex != 0 && logicalIndex != 2 && sort == "name")
    {
        if (update == "sortASC")
        {
            m_horiz_header->setSortIndicator(2, Qt::AscendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
        }
        else if (update == "sortDESC")
        {
            m_horiz_header->setSortIndicator(2, Qt::DescendingOrder);
            m_horiz_header->setSortIndicatorShown(true);
        }
        else if (update == "default")
            m_horiz_header->setSortIndicatorShown(false);
        return;
    }
    else if (logicalIndex == 2)
    {
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
                query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id ORDER BY ep.entry_name");
                query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id ORDER BY entry_name");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name");
                }
                else if (entry_phone != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name");
                }
                else if (entry_comment != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name");
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
                query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id ORDER BY entry_name DESC");
            }
            else
            {
                if (entry_name != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name DESC");
                }
                else if (entry_phone != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name DESC");
                }
                else if (entry_comment != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name DESC");
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
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id");
                }
                else if (entry_phone != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id");
                }
                else if (entry_comment != "NULL")
                {
                    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
                    query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id");
                }
            }
            onUpdate();
        }
    }
}

void ContactsDialog::searchFunction()
{
    if (ui->lineEdit->text().isEmpty())
        return;

    update = "default";
    filter = true;
    m_horiz_header->setSortIndicatorShown(false);
    entry_name = "NULL";
    entry_phone = "NULL";
    entry_comment = "NULL";

    if (ui->comboBox->currentText() == "Поиск по ФИО / названию")
    {
        entry_name = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id");
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по номеру телефона")
    {
        entry_phone = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id");
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по заметке")
    {
        entry_comment = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id");
        onUpdate();
    }
}

void ContactsDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

void ContactsDialog::on_searchButton_clicked()
{
    searchFunction();
}
