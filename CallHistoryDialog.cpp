#include "CallHistoryDialog.h"
#include "ui_callhistorydialog.h"
#include "SettingsDialog.h"
#include "Global.h"
#include "AsteriskManager.h"

#include <QMessageBox>
#include <QWidget>
#include <QList>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QLabel>
#include <QTextBlock>
#include <QItemSelectionModel>

CallHistoryDialog::CallHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallHistoryDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();
    setWindowTitle("История звонков по номеру: " + my_number);

    connect(ui->callButton,   &QPushButton::clicked, this, &CallHistoryDialog::onCallClicked);
    connect(ui->addContactButton, &QPushButton::clicked, this, &CallHistoryDialog::onAddContact);
    connect(ui->addOrgContactButton, &QPushButton::clicked, this, &CallHistoryDialog::onAddOrgContact);
    connect(ui->updateButton, &QPushButton::clicked, this, &CallHistoryDialog::onUpdate);

    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToMissed(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToReceived(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToPlaced(const QModelIndex &)));

    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberMissed(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberReceived(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberPlaced(const QModelIndex &)));

    ui->tabWidget->setCurrentIndex(0);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);

    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}

void CallHistoryDialog::showEvent( QShowEvent* event ) {
    QDialog::showEvent( event );
    onUpdate();
}

void CallHistoryDialog::getNumberMissed(const QModelIndex &index)
{
    number = query1->data(query1->index(index.row(), 1)).toString();
}

void CallHistoryDialog::getNumberReceived(const QModelIndex &index)
{
    number = query2->data(query2->index(index.row(), 1)).toString();
}

void CallHistoryDialog::getNumberPlaced(const QModelIndex &index)
{
    number = query3->data(query3->index(index.row(), 1)).toString();
}

void CallHistoryDialog::onAddContact()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    addContactDialog = new AddContactDialog;

    bool a = checkNumber(number);
    if (a == true)
    {
        addContactDialog->setValuesCallHistory(number);
        addContactDialog->exec();
    }
    else if (a == false) editContact(number);
    addContactDialog->deleteLater();
}

void CallHistoryDialog::onAddOrgContact()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    addOrgContactDialog = new AddOrgContactDialog;

    bool a = checkNumber(number);
    if (a == true)
    {
        addOrgContactDialog->setOrgValuesCallHistory(number);
        addOrgContactDialog->exec();
    }
    else if (a == false) editOrgContact(number);
    addOrgContactDialog->deleteLater();
}

void CallHistoryDialog::addNoteToMissed(const QModelIndex &index)
{
    state_call = "missed";
    uniqueid = query1->data(query1->index(index.row(), 5)).toString();
    addNoteDialog = new AddNoteDialog;
    addNoteDialog->setCallId(uniqueid, state_call);
    connect(addNoteDialog, SIGNAL(sendDataToMissed()), this, SLOT(receiveDataToMissed()));
    addNoteDialog->exec();
    addNoteDialog->deleteLater();
}

void CallHistoryDialog::receiveDataToMissed()
{
    deleteMissedObjects();
    loadMissedCalls();
}

void CallHistoryDialog::addNoteToReceived(const QModelIndex &index)
{
    state_call = "received";
    uniqueid = query2->data(query2->index(index.row(), 5)).toString();
    addNoteDialog = new AddNoteDialog;
    addNoteDialog->setCallId(uniqueid, state_call);
    connect(addNoteDialog, SIGNAL(sendDataToReceived()), this, SLOT(receiveDataToReceived()));
    addNoteDialog->exec();
    addNoteDialog->deleteLater();
}

void CallHistoryDialog::receiveDataToReceived()
{
    deleteReceivedObjects();
    loadReceivedCalls();
}

void CallHistoryDialog::addNoteToPlaced(const QModelIndex &index)
{
    state_call = "placed";
    uniqueid = query3->data(query3->index(index.row(), 5)).toString();
    addNoteDialog = new AddNoteDialog;
    addNoteDialog->setCallId(uniqueid, state_call);
    connect(addNoteDialog, SIGNAL(sendDataToPlaced()), this, SLOT(receiveDataToPlaced()));
    addNoteDialog->exec();
    addNoteDialog->deleteLater();
}

void CallHistoryDialog::receiveDataToPlaced()
{
    deletePlacedObjects();
    loadPlacedCalls();
}

CallHistoryDialog::~CallHistoryDialog()
{
    settingsDialog->deleteLater();
    deleteObjects();
    delete ui;
}

void CallHistoryDialog::onCallClicked()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    QString from = my_number;
    QString to = number;
    const QString protocol = global::getSettingsValue(from, "extensions").toString();
    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

bool CallHistoryDialog::checkNumber(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString number_internal = number;
    query.prepare("SELECT EXISTS(SELECT phone FROM phone WHERE phone ="+number_internal+")");
    query.exec();
    query.next();
    if(query.value(0) != 0) return false;
    else return true;
}

void CallHistoryDialog::editContact(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString updateID = getUpdateId(number);
    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM phone WHERE phone = '"+number+"')");
    query.exec();
    query.first();
    if (query.value(0).toString() == "person")
    {
        editContactDialog = new EditContactDialog;
        editContactDialog->setValuesContacts(updateID);
        editContactDialog->exec();
        editContactDialog->deleteLater();
    }
    else
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Данный контакт принадлежит организации!"), QMessageBox::Ok);
    }
}

void CallHistoryDialog::editOrgContact(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString updateID = getUpdateId(number);
    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM phone WHERE phone = '"+number+"')");
    query.exec();
    query.first();
    if (query.value(0).toString() == "org")
    {
        editOrgContactDialog = new EditOrgContactDialog;
        editOrgContactDialog->setOrgValuesContacts(updateID);
        editOrgContactDialog->exec();
        editOrgContactDialog->deleteLater();
    }
    else
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Данный контакт принадлежит физ. лицу!"), QMessageBox::Ok);
    }
}

QString CallHistoryDialog::getUpdateId(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM phone WHERE phone = '"+number+"')");
    query.exec();
    query.first();
    QString updateID = query.value(0).toString();
    return updateID;
}

void CallHistoryDialog::loadMissedCalls()
{

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query1 = new QSqlQueryModel;
    query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL 7 DAY) AND dst = '"+my_number+"'ORDER BY datetime DESC", dbAsterisk);
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->insertColumn(4);
    query1->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView->setModel(query1);
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnHidden(2, true);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        uniqueid = query1->data(query1->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView->setIndexWidget(query1->index(row_index, 4), loadMissedNote());
    }
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::loadReceivedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query2 = new QSqlQueryModel;
    query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL 7 DAY) AND dst = '"+my_number+"'ORDER BY datetime DESC", dbAsterisk);

    query2->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    query2->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query2->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query2->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query2->insertColumn(4);
    query2->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_2->setModel(query2);
    ui->tableView_2->setColumnHidden(5, true);
    ui->tableView_2->setColumnHidden(2, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        uniqueid = query2->data(query2->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_2->setIndexWidget(query2->index(row_index, 4), loadReceivedNote());
    }
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->resizeRowsToContents();
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::loadPlacedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query3 = new QSqlQueryModel;
    query3->setQuery("SELECT extfield2, dst, src, datetime, uniqueid FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL 7 DAY) AND src = '"+my_number+"'ORDER BY datetime DESC", dbAsterisk);

    query3->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    query3->setHeaderData(1, Qt::Horizontal, QObject::tr("Кому"));
    query3->setHeaderData(2, Qt::Horizontal, QObject::tr("Откуда"));
    query3->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query3->insertColumn(4);
    query3->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_3->setModel(query3);
    ui->tableView_3->setColumnHidden(5, true);
    ui->tableView_3->setColumnHidden(2, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        uniqueid = query3->data(query3->index(row_index, 5)).toString();
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_3->setIndexWidget(query3->index(row_index, 4), loadPlacedNote());
    }
    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_3->resizeRowsToContents();
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

QWidget* CallHistoryDialog::loadMissedNote()
{
    QWidget* missedWgt = new QWidget;
    QLabel *missedNote = new QLabel(missedWgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
    query.exec();
    query.first();
    missedNote->setText(query.value(0).toString());

    widgetsMissed.append(missedWgt);
    notesMissed.append(missedNote);
    return missedWgt;
}

QWidget* CallHistoryDialog::loadReceivedNote()
{
    QWidget* receivedWgt = new QWidget;
    QLabel *receivedNote = new QLabel(receivedWgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
    query.exec();
    query.first();
    receivedNote->setText(query.value(0).toString());

    widgetsReceived.append(receivedWgt);
    notesReceived.append(receivedNote);
    return receivedWgt;
}

QWidget* CallHistoryDialog::loadPlacedNote()
{
    QWidget* placedWgt = new QWidget;
    QLabel *placedNote = new QLabel(placedWgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
    query.exec();
    query.first();
    placedNote->setText(query.value(0).toString());

    widgetsPlaced.append(placedWgt);
    notesPlaced.append(placedNote);
    return placedWgt;
}

void CallHistoryDialog::onUpdate()
{
    deleteObjects();
    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}

void CallHistoryDialog::deleteMissedObjects()
{
    for (int i = 0; i < widgetsMissed.size(); ++i)
    {
        widgetsMissed[i]->deleteLater();
    }
    qDeleteAll(notesMissed);
    widgetsMissed.clear();
    notesMissed.clear();
    delete query1;
}

void CallHistoryDialog::deleteReceivedObjects()
{
    for (int i = 0; i < widgetsReceived.size(); ++i)
    {
        widgetsReceived[i]->deleteLater();
    }
    qDeleteAll(notesReceived);
    widgetsReceived.clear();
    notesReceived.clear();
    delete query2;
}

void CallHistoryDialog::deletePlacedObjects()
{
    for (int i = 0; i < widgetsPlaced.size(); ++i)
    {
        widgetsPlaced[i]->deleteLater();
    }
    qDeleteAll(notesPlaced);
    widgetsPlaced.clear();
    notesPlaced.clear();
    delete query3;
}

void CallHistoryDialog::deleteObjects()
{
    for (int i = 0; i < widgetsMissed.size(); ++i)
    {
        widgetsMissed[i]->deleteLater();
    }
    qDeleteAll(notesMissed);
    widgetsMissed.clear();
    notesMissed.clear();
    for (int i = 0; i < widgetsReceived.size(); ++i)
    {
        widgetsReceived[i]->deleteLater();
    }
    qDeleteAll(notesReceived);
    widgetsReceived.clear();
    notesReceived.clear();
    for (int i = 0; i < widgetsPlaced.size(); ++i)
    {
        widgetsPlaced[i]->deleteLater();
    }
    widgetsPlaced.clear();
    notesPlaced.clear();
    delete query1;
    delete query2;
    delete query3;
}
