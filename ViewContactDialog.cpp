#include "ViewContactDialog.h"
#include "SettingsDialog.h"
#include "ui_ViewContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>
#include <QDebug>

ViewContactDialog::ViewContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

}

ViewContactDialog::~ViewContactDialog()
{
    delete ui;
}

void ViewContactDialog::setValuesContacts(QString &i)
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

    if(firstNumber != 0)
    {
        contact_number = QString(ui->FirstNumber->text());
        loadMissedCalls();
        loadReceivedCalls();
        loadPlacedCalls();
    }
    if(secondNumber != 0)
    {
        contact_number = QString(ui->SecondNumber->text());
        loadMissedCalls();
        loadReceivedCalls();
        loadPlacedCalls();
    }
    if(thirdNumber != 0)
    {
        contact_number = QString(ui->ThirdNumber->text());
        loadMissedCalls();
        loadReceivedCalls();
        loadPlacedCalls();
    }
    if(fourthNumber != 0)
    {
        contact_number = QString(ui->FourthNumber->text());
        loadMissedCalls();
        loadReceivedCalls();
        loadPlacedCalls();
    }
    if(fifthNumber != 0)
    {
        contact_number = QString(ui->FifthNumber->text());
        loadMissedCalls();
        loadReceivedCalls();
        loadPlacedCalls();
    }
}

void ViewContactDialog::loadMissedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    SettingsDialog *settingsDialog = new SettingsDialog();
    QString number = settingsDialog->getExtension();

    query1 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = "+number+" AND src = "+contact_number, dbAsterisk);
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
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView->setIndexWidget(query1->index(row_index, 4), loadNote(row_index));
    }
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void ViewContactDialog::loadReceivedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    SettingsDialog *settingsDialog = new SettingsDialog();
    QString number = settingsDialog->getExtension();

    query2 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = "+number+" AND src = "+contact_number, dbAsterisk);

    query2->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    query2->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query2->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query2->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query2->insertColumn(4);
    query2->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_2->setModel(query2);
    ui->tableView_2->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        uniqueid = query2->data(query2->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_2->setIndexWidget(query2->index(row_index, 4), loadNote(row_index));
    }
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->resizeRowsToContents();
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void ViewContactDialog::loadPlacedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    SettingsDialog *settingsDialog = new SettingsDialog();
    QString number = settingsDialog->getExtension();

    query3 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    query3->setQuery("SELECT extfield2, dst, src, datetime, uniqueid FROM cdr WHERE src = "+number+" AND dst = "+contact_number, dbAsterisk);

    query3->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    query3->setHeaderData(1, Qt::Horizontal, QObject::tr("Кому"));
    query3->setHeaderData(2, Qt::Horizontal, QObject::tr("Откуда"));
    query3->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query3->insertColumn(4);
    query3->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_3->setModel(query3);
    ui->tableView_3->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        uniqueid = query3->data(query3->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_3->setIndexWidget(query3->index(row_index, 4), loadNote(row_index));
    }
    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_3->resizeRowsToContents();
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

QWidget* ViewContactDialog::loadNote(int &row_index)
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
