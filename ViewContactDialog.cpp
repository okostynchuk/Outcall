#include "ViewContactDialog.h"
#include "ui_ViewContactDialog.h"
#include "AsteriskManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QMessageBox>
#include <QDebug>

ViewContactDialog::ViewContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->editButton, &QAbstractButton::clicked, this, &ViewContactDialog::onEdit);
    connect(ui->callButton, &QAbstractButton::clicked, this, &ViewContactDialog::onCall);
    connect(ui->comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(updateCalls()));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewMissedNotes(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewRecievedNotes(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(viewPlacedNotes(const QModelIndex &)));


    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();
}

ViewContactDialog::~ViewContactDialog()
{
    deleteObjects();
    delete settingsDialog;
    delete ui;
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
    deleteObjects();
    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}

void ViewContactDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
    {
        widgets[i]->deleteLater();
    }
    for (int i = 0; i < queries.size(); ++i)
    {
        queries[i]->deleteLater();
    }
    qDeleteAll(notes);
    widgets.clear();
    queries.clear();
    notes.clear();
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

    days = ui->comboBox->currentText();
    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}

void ViewContactDialog::loadMissedCalls()
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
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') AND src = '"+number1+"' AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 2)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') AND src IN ('"+number1+"','"+number2+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 3)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') AND src IN ('"+number1+"','"+number2+"','"+number3+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 4)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 5)
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"','"+number5+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
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
            ui->tableView->setIndexWidget(query1->index(row_index, 4), loadNote());
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
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    QString number1 = QString(ui->FirstNumber->text());
    QString number2 = QString(ui->SecondNumber->text());
    QString number3 = QString(ui->ThirdNumber->text());
    QString number4 = QString(ui->FourthNumber->text());
    QString number5 = QString(ui->FifthNumber->text());

    query2 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    if(count2 == 1)
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND src = '"+number1+"' AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 2)
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND src IN ('"+number1+"','"+number2+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 3)
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND src IN ('"+number1+"','"+number2+"','"+number3+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 4)
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 5)
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND src IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"','"+number5+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
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
        uniqueid = query2->data(query2->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_2->setIndexWidget(query2->index(row_index, 4), loadNote());
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
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    QString number1 = QString(ui->FirstNumber->text());
    QString number2 = QString(ui->SecondNumber->text());
    QString number3 = QString(ui->ThirdNumber->text());
    QString number4 = QString(ui->FourthNumber->text());
    QString number5 = QString(ui->FifthNumber->text());

    query3 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    if(count2 == 1)
    {
        query3->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE dst = '"+number1+"' AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 2)
    {
        query3->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE dst IN ('"+number1+"','"+number2+"') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 3)
    {
        query3->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE dst IN ('"+number1+"','"+number2+"','"+number3+"')  AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 4)
    {
        query3->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE dst IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"')  AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }
    if(count2 == 5)
    {
        query3->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE dst IN ('"+number1+"','"+number2+"','"+number3+"','"+number4+"','"+number5+"')  AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime DESC", dbAsterisk);
    }

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
        uniqueid = query3->data(query3->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_3->setIndexWidget(query3->index(row_index, 4), loadNote());
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

    query.prepare("SELECT note FROM calls WHERE uniqueid =" + uniqueid);
    query.exec();
    query.first();
    note->setText(query.value(0).toString());

    widgets.append(wgt);
    notes.append(note);
    return wgt;
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
