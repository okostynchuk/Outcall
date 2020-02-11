#include "CallHistoryDialog.h"
#include "ui_callhistorydialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QWidget>
#include <QList>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QString>
#include <QTableView>
#include <QLabel>
#include <QTextBlock>
#include <QClipboard>

CallHistoryDialog::CallHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallHistoryDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

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

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView_3->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tabWidget->setCurrentIndex(0);
    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
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
    loadReceivedCalls();
}

void CallHistoryDialog::addNoteToPlaced(const QModelIndex &index)
{
    state_call = "placed";
    uniqueid = query3->data(query3->index(index.row(), 5)).toString();
    addNoteDialog = new AddNoteDialog;
    addNoteDialog->setCallId(uniqueid, state_call);
    connect(addNoteDialog, SIGNAL(sendDataToPlaced()), this, SLOT(loadPlacedCalls()));
    addNoteDialog->exec();
    addNoteDialog->deleteLater();
}

void CallHistoryDialog::receiveDataToPlaced()
{
    loadPlacedCalls();
}

QWidget* CallHistoryDialog::loadNote(int &row_index)
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

CallHistoryDialog::~CallHistoryDialog()
{
    delete ui;
}

void CallHistoryDialog::showEvent(QShowEvent *)
{
   //onUpdate();
}

void CallHistoryDialog::onCallClicked()
{
//    QList<QTreeWidgetItem*> selectedItems;

//    if (ui->tabWidget->currentIndex() == MISSED)
//    {
//       selectedItems = ui->treeWidgetMissed->selectedItems();

//       if (selectedItems.size() == 0)
//           return;

//       QTreeWidgetItem *item = selectedItems.at(0);
//       const QString from = item->text(1);
//       int ind1 = item->text(2).indexOf("(");
//       int ind2 = item->text(2).indexOf(")");
//       const QString to = item->text(2).mid(0, ind1);
//       const QString protocol = item->text(2).mid(ind1 + 1, ind2 - ind1 - 1);

//       g_pAsteriskManager->originateCall(to, from, protocol, to);
//    }
//    else if (ui->tabWidget->currentIndex() == RECIEVED)
//    {
//       selectedItems = ui->treeWidgetReceived->selectedItems();

//       if (selectedItems.size() == 0)
//           return;

//       QTreeWidgetItem *item = selectedItems.at(0);
//       const QString from = item->text(1);
//       int ind1 = item->text(2).indexOf("(");
//       int ind2 = item->text(2).indexOf(")");
//       const QString to = item->text(2).mid(0, ind1);
//       const QString protocol = item->text(2).mid(ind1+1, ind2-ind1-1);

//       g_pAsteriskManager->originateCall(to, from, protocol, to);
//    }
//    else if (ui->tabWidget->currentIndex() == PLACED)
//    {
//       selectedItems = ui->treeWidgetPlaced->selectedItems();

//       if (selectedItems.size() == 0)
//           return;

//       QTreeWidgetItem *item = selectedItems.at(0);
//       int ind1 = item->text(0).indexOf("(");
//       int ind2 = item->text(0).indexOf(")");
//       const QString from = item->text(0).mid(0, ind1);
//       const QString to = item->text(1);
//       const QString protocol = item->text(0).mid(ind1 + 1, ind2 - ind1 - 1);

//       g_pAsteriskManager->originateCall(from, to, protocol, from);
//    }
}

void CallHistoryDialog::onAddOrgContact()
{
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

    SettingsDialog *settingsDialog = new SettingsDialog();
    QString number = settingsDialog->getExtension();

    query1 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = "+number, dbAsterisk);
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

void CallHistoryDialog::loadReceivedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    SettingsDialog *settingsDialog = new SettingsDialog();
    QString number = settingsDialog->getExtension();

    query2 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = "+number, dbAsterisk);

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

void CallHistoryDialog::loadPlacedCalls()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

    SettingsDialog *settingsDialog = new SettingsDialog();
    QString number = settingsDialog->getExtension();

    query3 = new QSqlQueryModel;
    QSqlDatabase db;
    QSqlQuery query(db);
    query3->setQuery("SELECT extfield2, dst, src, datetime, uniqueid FROM cdr WHERE src = "+number, dbAsterisk);

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

void CallHistoryDialog::onUpdate()
{
    deleteObjects();
    loadMissedCalls();
    loadReceivedCalls();
    loadPlacedCalls();
}

void CallHistoryDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
    {
        widgets[i]->deleteLater();
    }
    //qDeleteAll(labels);
    //qDeleteAll(buttons);
    widgets.clear();
   // labels.clear();
   // buttons.clear();
}
