#include "ViewOrgContactDialog.h"
#include "ui_ViewOrgContactDialog.h"
#include "ViewContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>
#include <QHeaderView>
#include <QClipboard>
#include <QScrollBar>
#include <QDebug>

ViewOrgContactDialog::ViewOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewOrgContactDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->tableView->setSortingEnabled(false);
    m_horiz_header = ui->tableView->horizontalHeader();
    m_horiz_header1 = ui->tableView->horizontalHeader();

    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));
    connect(m_horiz_header, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &ViewOrgContactDialog::clearEditText);
    connect(m_horiz_header1, SIGNAL(sectionClicked(int)), this, SLOT(onSortingSectionClicked(int)));

    onComboBoxSelected();
    counter = true;
    counter1 = 0;
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    delete query_model;
    delete ui;
}

void ViewOrgContactDialog::clearEditText(){
    ui->lineEdit->clear();
}

void ViewOrgContactDialog::onTableClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString cellText = index.data().toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(cellText);
    }
}

void ViewOrgContactDialog::showCard(const QModelIndex &index)
{
    QString id = query_model->data(query_model->index(index.row(), 0)).toString();
    viewContactDialog = new ViewContactDialog;
    viewContactDialog->setValuesContacts(id);
    viewContactDialog->exec();
    viewContactDialog->deleteLater();
}

void ViewOrgContactDialog::onUpdate()
{
    if (update == "default")
    {
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id");
    }

    verticalScroll = ui->tableView->verticalScrollBar();
    horizontalScroll = ui->tableView->horizontalScrollBar();
    int valueV = verticalScroll->value();
    int valueH = horizontalScroll->value();

    query_model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query_model->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО"));
    query_model->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query_model->setHeaderData(3, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(query_model);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    if (update == "default")
    {
        ui->tableView->verticalScrollBar()->setSliderPosition(valueV);
        ui->tableView->horizontalScrollBar()->setSliderPosition(valueH);
    }

    update = "default";
}

void ViewOrgContactDialog::onComboBoxSelected()
{
    ui->comboBox->addItem("Поиск по ФИО");
    ui->comboBox->addItem("Поиск по номеру телефона");
    ui->comboBox->addItem("Поиск по заметке");
}

void ViewOrgContactDialog::onSortingSectionClicked(int logicalIndex)
{
    QString entry_name1 = ui->lineEdit->text();
    if (ui->comboBox->currentText() == "Поиск по ФИО")
    {
        update = "sort";

        if(logicalIndex != 1) return;

        if (counter1 == 0)
        {
            query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name1 + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name");
            onUpdate();
            counter1++;
        }
        else
        {
            query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name1 + "%' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");
            onUpdate();
            counter1 = 0;
        }
    }
}

void ViewOrgContactDialog::onSectionClicked (int logicalIndex)
{
    if (logicalIndex != 1) return;

    update = "sort";

    if (counter == true)
    {
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY ep.entry_name");

        onUpdate();
        counter = false;
    }
    else
    {
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id ORDER BY ep.entry_name DESC");

        onUpdate();
        counter = true;
    }
}

void ViewOrgContactDialog::on_lineEdit_returnPressed()
{
    update = "filter";
    QComboBox::AdjustToContents;

    if (ui->comboBox->currentText() == "Поиск по ФИО")
    {
        QString entry_name = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");

        onUpdate();
    }

    if (ui->comboBox->currentText() == "Поиск по номеру телефона")
    {
        QString entry_phone = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");

        onUpdate();
    }

    if (ui->comboBox->currentText() == "Поиск по заметке")
    {
        QString entry_comment = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_comment LIKE '%" + entry_comment + "%' GROUP BY ep.entry_id");

        onUpdate();
    }
}

void ViewOrgContactDialog::setOrgValuesContacts(QString &i)
{
    updateID = i;
    QSqlDatabase db;
    QSqlQuery query(db);
    QString sql = QString("select entry_phone from entry_phone where entry_id = %1").arg(updateID);
    query.prepare(sql);
    query.exec();
    query.next();
    firstNumber = query.value(0).toString();
    query.next();
    secondNumber = query.value(0).toString();
    query.next();
    thirdNumber = query.value(0).toString();
    query.next();
    fourthNumber = query.value(0).toString();
    query.next();
    fifthNumber = query.value(0).toString();
    sql = QString("select distinct entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment from entry where id = %1").arg(updateID);
    query.prepare(sql);
    query.exec();
    query.next();
    QString entryORGName = query.value(0).toString();
    QString entryCity = query.value(1).toString();
    QString entryAddress = query.value(2).toString();
    QString entryEmail = query.value(3).toString();
    QString entryVyborID = query.value(4).toString();
    QString entryComment = query.value(5).toString();
    ui->FirstNumber->setText(firstNumber);
    ui->SecondNumber->setText(secondNumber);
    ui->ThirdNumber->setText(thirdNumber);
    ui->FourthNumber->setText(fourthNumber);
    ui->FifthNumber->setText(fifthNumber);
    ui->OrgName->setText(entryORGName);
    ui->City->setText(entryCity);
    ui->Address->setText(entryAddress);
    ui->Email->setText(entryEmail);
    ui->VyborID->setText(entryVyborID);
    ui->Comment->setText(entryComment);

    if(!firstNumber.isEmpty())
         ui->FirstNumber->setInputMask("999-999-9999;_");
    if(!secondNumber.isEmpty())
         ui->SecondNumber->setInputMask("999-999-9999;_");
    if(!thirdNumber.isEmpty())
         ui->ThirdNumber->setInputMask("999-999-9999;_");
    if(!fourthNumber.isEmpty())
         ui->FourthNumber->setInputMask("999-999-9999;_");
    if(!fifthNumber.isEmpty())
         ui->FifthNumber->setInputMask("999-999-9999;_");
    query_model = new QSqlQueryModel;

    query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id");
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
}

void ViewOrgContactDialog::setOrgValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}
