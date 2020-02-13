#include "ViewOrgContactDialog.h"
#include "ui_ViewOrgContactDialog.h"
#include "ViewContactDialog.h"
#include "SettingsDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

ViewOrgContactDialog::ViewOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewOrgContactDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->tableView->setSortingEnabled(false);
    m_horiz_header = ui->tableView->horizontalHeader();

    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));
    connect(m_horiz_header, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &ViewOrgContactDialog::clearEditText);

    onComboBoxSelected();

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    deleteObjects();
    delete settingsDialog;
    delete query_model;
    delete ui;
}

void ViewOrgContactDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
    {
        widgets[i]->deleteLater();
    }
    for (int i = 0; i < queries.size(); ++i)
    {
        queries[i]->deleteLater();
    }
    widgets.clear();
    queries.clear();
}

void ViewOrgContactDialog::clearEditText()
{
    ui->lineEdit->clear();
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
    if (update == "default" && filter == false)
    {
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' GROUP BY ep.entry_id");
        m_horiz_header->setSortIndicatorShown(false);
    }

    valueV = ui->tableView->verticalScrollBar()->sliderPosition();
    valueH = ui->tableView->horizontalScrollBar()->sliderPosition();

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

    ui->tableView->verticalScrollBar()->setSliderPosition(valueV);
    ui->tableView->horizontalScrollBar()->setSliderPosition(valueH);
}

void ViewOrgContactDialog::onComboBoxSelected()
{
    ui->comboBox->addItem("Поиск по ФИО");
    ui->comboBox->addItem("Поиск по номеру телефона");
    ui->comboBox->addItem("Поиск по заметке");
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

void ViewOrgContactDialog::on_lineEdit_returnPressed()
{
    if (ui->lineEdit->text().isEmpty())
    {
        update = "default";
        filter = false;
        onUpdate();
    }

    update = "default";
    filter = true;
    m_horiz_header->setSortIndicatorShown(false);
    entry_name = "NULL";
    entry_phone = "NULL";
    entry_comment = "NULL";

    if (ui->comboBox->currentText() == "Поиск по ФИО")
    {
        entry_name = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по номеру телефона")
    {
        entry_phone = ui->lineEdit->text();
        query_model->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + updateID + "' AND ep.entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по заметке")
    {
        entry_comment = ui->lineEdit->text();
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
    filter = false;

    if(secondNumber != 0)
    {
        count2++;
        if(thirdNumber != 0)
        {
            count2++;
            if(fourthNumber != 0)
            {
                count2++;
                if(fifthNumber != 0)
                {
                   count2++;
                }
            }
        }
    }

    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}

void ViewOrgContactDialog::setOrgValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void ViewOrgContactDialog::loadMissedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    QString number1 = QString(ui->FirstNumber->text());
    QString number2 = QString(ui->SecondNumber->text());
    QString number3 = QString(ui->ThirdNumber->text());
    QString number4 = QString(ui->FourthNumber->text());
    QString number5 = QString(ui->FifthNumber->text());

    query1 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    if(count2 == 1)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = '"+my_number+"' AND src = '"+number1+"' ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 2)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 3)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"','"+number3+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 4)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 5)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"','"+number5+"') ORDER BY datetime DESC", dbAsterisk);
    }
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->insertColumn(4);
    query1->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_2->setModel(query1);
    ui->tableView_2->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_2->setIndexWidget(query1->index(row_index, 4), loadNote());
    }
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->resizeRowsToContents();
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    queries.append(query1);
}

void ViewOrgContactDialog::loadReceivedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    QString number1 = QString(ui->FirstNumber->text());
    QString number2 = QString(ui->SecondNumber->text());
    QString number3 = QString(ui->ThirdNumber->text());
    QString number4 = QString(ui->FourthNumber->text());
    QString number5 = QString(ui->FifthNumber->text());

    query1 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    if(count2 == 1)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = '"+my_number+"' AND src = '"+number1+"' ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 2)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 3)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"','"+number3+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 4)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 5)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = '"+my_number+"' AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"','"+number5+"') ORDER BY datetime DESC", dbAsterisk);
    }
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->insertColumn(4);
    query1->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_3->setModel(query1);
    ui->tableView_3->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_3->setIndexWidget(query1->index(row_index, 4), loadNote());
    }
    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_3->resizeRowsToContents();
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    queries.append(query1);
}

void ViewOrgContactDialog::loadPlacedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    QString number1 = QString(ui->FirstNumber->text());
    QString number2 = QString(ui->SecondNumber->text());
    QString number3 = QString(ui->ThirdNumber->text());
    QString number4 = QString(ui->FourthNumber->text());
    QString number5 = QString(ui->FifthNumber->text());

    query1 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    if(count2 == 1)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE src = '"+my_number+"' AND dst = '"+number1+"' ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 2)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE src = '"+my_number+"' AND dst IN ('"+number1+"','"+number2+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 3)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE src = '"+my_number+"' AND dst IN ('"+number1+"','"+number2+"','"+number3+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 4)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE src = '"+my_number+"' AND dst IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"') ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 5)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE src = '"+my_number+"' AND dst IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"','"+number5+"') ORDER BY datetime DESC", dbAsterisk);
    }

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->insertColumn(4);
    query1->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_4->setModel(query1);
    ui->tableView_4->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_4->setIndexWidget(query1->index(row_index, 4), loadNote());
    }
    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_4->resizeRowsToContents();
    ui->tableView_4->resizeColumnsToContents();
    ui->tableView_4->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    queries.append(query1);
}

QWidget* ViewOrgContactDialog::loadNote()
{
    QWidget* wgt = new QWidget;
    QLabel *note = new QLabel(wgt);

    QSqlDatabase db;
    QSqlQuery query2(db);

    query2.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
    query2.exec();
    query2.first();
    note->setText(query2.value(0).toString());

    widgets.append(wgt);
    notes.append(note);
    return wgt;
}
