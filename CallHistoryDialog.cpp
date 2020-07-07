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
#include <QRegExp>

CallHistoryDialog::CallHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallHistoryDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();
    my_group = settingsDialog->getGroupExtension();
    setWindowTitle(QObject::tr("История звонков по номеру:") + " " + my_number);

    connect(ui->callButton,          &QPushButton::clicked, this, &CallHistoryDialog::onCallClicked);
    connect(ui->addContactButton,    &QPushButton::clicked, this, &CallHistoryDialog::onAddContact);
    connect(ui->addOrgContactButton, &QPushButton::clicked, this, &CallHistoryDialog::onAddOrgContact);
    connect(ui->updateButton,        &QPushButton::clicked, this, &CallHistoryDialog::onUpdate);

    connect(ui->comboBox_2,  SIGNAL(currentTextChanged(QString)), this, SLOT(tabSelected()));

    connect(ui->tableView,   SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToMissed(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToReceived(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToPlaced(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNotes(const QModelIndex &)));

    connect(ui->tableView,   SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberMissed(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberReceived(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberPlaced(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumber(const QModelIndex &)));

    ui->tabWidget->setCurrentIndex(0);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView_4->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_4->horizontalHeader()->setSectionsClickable(false);

    ui->tableView->setStyleSheet  ("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_2->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_3->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_4->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    days = ui->comboBox_2->currentText();
    loadAllCalls();
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
}

void CallHistoryDialog::tabSelected()
{
    days = ui->comboBox_2->currentText();
    if(ui->tabWidget->currentIndex() == 0)
        loadAllCalls();
    if(ui->tabWidget->currentIndex() == 1)
        loadMissedCalls();
    if(ui->tabWidget->currentIndex() == 2)
        loadReceivedCalls();
    if(ui->tabWidget->currentIndex() == 3)
        loadPlacedCalls();
}

void CallHistoryDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    onUpdate();
}

void CallHistoryDialog::getNumber(const QModelIndex &index)
{
    number = query4->data(query4->index(index.row(), 1)).toString();

    if(number == my_number)
    {
        number = query4->data(query4->index(index.row(), 2)).toString();
        number.remove(QRegExp("[(][a-z]+ [0-9]+[)]"));
    }
}

void CallHistoryDialog::getNumberMissed(const QModelIndex &index)
{
    number = query1->data(query1->index(index.row(), 1)).toString();
    number.remove(QRegExp("[(][a-z]+ [0-9]+[)]"));
}

void CallHistoryDialog::getNumberReceived(const QModelIndex &index)
{
    number = query2->data(query2->index(index.row(), 1)).toString();
    number.remove(QRegExp("[(][a-z]+ [0-9]+[)]"));
}

void CallHistoryDialog::getNumberPlaced(const QModelIndex &index)
{
    number = query3->data(query3->index(index.row(), 1)).toString();
    number.remove(QRegExp("[(][a-z]+ [0-9]+[)]"));
}

void CallHistoryDialog::onAddContact()
{
    if ((ui->tabWidget->currentIndex() == 1 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 0 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    addContactDialog = new AddContactDialog;
    bool a = checkNumber(number);
    if (a)
    {
        addContactDialog->setValuesCallHistory(number);
        addContactDialog->show();
        addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        editContact(number);
}

void CallHistoryDialog::onAddOrgContact()
{
    if ((ui->tabWidget->currentIndex() == 1 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 0 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    addOrgContactDialog = new AddOrgContactDialog;

    bool a = checkNumber(number);
    if (a)
    {
        addOrgContactDialog->setOrgValuesCallHistory(number);
        addOrgContactDialog->show();
        addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        editOrgContact(number);
}

void CallHistoryDialog::addNotes(const QModelIndex &index)
{
    state_call = "all";
    uniqueid = query4->data(query4->index(index.row(), 7)).toString();
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    connect(notesDialog, SIGNAL(sendDataToAllCalls()), this, SLOT(receiveDataToAllCalls()));
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void CallHistoryDialog::receiveDataToAllCalls()
{
    deleteObjectsOfAllCalls();
    loadAllCalls();
}

void CallHistoryDialog::addNoteToMissed(const QModelIndex &index)
{
    state_call = "missed";
    uniqueid = query1->data(query1->index(index.row(), 5)).toString();
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    connect(notesDialog, SIGNAL(sendDataToMissed()), this, SLOT(receiveDataToMissed()));
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
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
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    connect(notesDialog, SIGNAL(sendDataToReceived()), this, SLOT(receiveDataToReceived()));
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
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
    notesDialog = new NotesDialog;
    notesDialog->setCallId(uniqueid, state_call);
    connect(notesDialog, SIGNAL(sendDataToPlaced()), this, SLOT(receiveDataToPlaced()));
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void CallHistoryDialog::receiveDataToPlaced()
{
    deletePlacedObjects();
    loadPlacedCalls();
}

CallHistoryDialog::~CallHistoryDialog()
{
    delete settingsDialog;
    deleteObjects();
    delete ui;
}

void CallHistoryDialog::onCallClicked()
{
    if ((ui->tabWidget->currentIndex() == 1 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 0 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    const QString from = my_number;
    QString to = number;

    const QString protocol = global::getSettingsValue(from, "extensions").toString();
    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

bool CallHistoryDialog::checkNumber(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString number_internal = number;
    query.prepare("SELECT EXISTS(SELECT fone FROM fones WHERE fone = '"+number_internal+"')");
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
    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '"+number+"')");
    query.exec();
    query.first();
    if (query.value(0).toString() == "person")
    {
        editContactDialog = new EditContactDialog;
        editContactDialog->setValuesContacts(updateID);
        editContactDialog->show();
        editContactDialog->setAttribute(Qt::WA_DeleteOnClose);
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
    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '"+number+"')");
    query.exec();
    query.first();
    if (query.value(0).toString() == "org")
    {
        editOrgContactDialog = new EditOrgContactDialog;
        editOrgContactDialog->setOrgValuesContacts(updateID);
        editOrgContactDialog->show();
        editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
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
    query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '"+number+"')");
    query.exec();
    query.first();
    QString updateID = query.value(0).toString();
    return updateID;
}

void CallHistoryDialog::loadAllCalls()
{
    if(!widgets.isEmpty())
        deleteObjectsOfAllCalls();
    if(!widgetsName.isEmpty())
        deleteNameObjects();
    if(!widgetsMissedStatus.isEmpty())
        deleteMissedStatusObjects();
    if(!widgetsBusyStatus.isEmpty())
        deleteBusyStatusObjects();
    if(!widgetsCancelStatus.isEmpty())
        deleteCancelStatusObjects();
    if(!widgetsReceivedStatus.isEmpty())
        deleteReceivedStatusObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query4 = new QSqlQueryModel;
    query4->setQuery("SELECT extfield1, src, dst, disposition, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"' OR src = '"+my_number+"') ORDER BY datetime DESC", dbAsterisk);
    query4->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query4->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query4->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query4->insertColumn(4);
    query4->setHeaderData(4, Qt::Horizontal, QObject::tr("Статус"));
    query4->setHeaderData(5, Qt::Horizontal, QObject::tr("Дата и время"));
    query4->insertColumn(6);
    query4->setHeaderData(6, Qt::Horizontal, tr("Заметки"));

    ui->tableView_4->setModel(query4);
    ui->tableView_4->setColumnHidden(3,true);
    ui->tableView_4->setColumnHidden(7, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {
        extfield1 = query4->data(query4->index(row_index, 0)).toString();
        src = query4->data(query4->index(row_index, 1)).toString();
        uniqueid = query4->data(query4->index(row_index, 7)).toString();
        dialogStatus = query4->data(query4->index(row_index, 3)).toString();
        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query4->index(row_index, 0), loadName());
        if(dialogStatus == "NO ANSWER")
            ui->tableView_4->setIndexWidget(query4->index(row_index, 4), loadMissedStatus());
        if(dialogStatus == "BUSY")
            ui->tableView_4->setIndexWidget(query4->index(row_index, 4), loadBusyStatus());
        if(dialogStatus == "CANCEL")
            ui->tableView_4->setIndexWidget(query4->index(row_index, 4), loadCancelStatus());
        if(dialogStatus == "ANSWERED")
            ui->tableView_4->setIndexWidget(query4->index(row_index, 4), loadReceivedStatus());
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0) != 0)
            ui->tableView_4->setIndexWidget(query4->index(row_index, 6), loadAllNotes());
    }

    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_4->resizeColumnsToContents();
    ui->tableView_4->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView_4->setColumnWidth(4, 90);
}

QWidget* CallHistoryDialog::loadName()
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);
    nameLabel->setText(src);

    nameLayout->addWidget(nameLabel);
    nameLayout->setContentsMargins(3, 0, 0, 0);
    nameWgt->setLayout(nameLayout);

    layoutsName.append(nameLayout);
    widgetsName.append(nameWgt);
    labelsName.append(nameLabel);
    return nameWgt;
}

void CallHistoryDialog::deleteNameObjects()
{
    for (int i = 0; i < layoutsName.size(); ++i)
        layoutsName[i]->deleteLater();

    for (int i = 0; i < widgetsName.size(); ++i)
        widgetsName[i]->deleteLater();

    qDeleteAll(labelsName);
    layoutsName.clear();
    widgetsName.clear();
    labelsName.clear();
}

QWidget* CallHistoryDialog::loadMissedStatus()
{
    QHBoxLayout* missedStatusLayout = new QHBoxLayout;
    QWidget* missedStatusWgt = new QWidget;
    QLabel* missedStatusLabel = new QLabel(missedStatusWgt);
    missedStatusLabel->setText(tr("Пропущенный"));

    missedStatusLayout->addWidget(missedStatusLabel);
    missedStatusLayout->setContentsMargins(3, 0, 0, 0);
    missedStatusWgt->setLayout(missedStatusLayout);

    layoutsMissedStatus.append(missedStatusLayout);
    widgetsMissedStatus.append(missedStatusWgt);
    labelsMissedStatus.append(missedStatusLabel);
    return missedStatusWgt;
}

void CallHistoryDialog::deleteMissedStatusObjects()
{
    for (int i = 0; i < layoutsMissedStatus.size(); ++i)
        layoutsMissedStatus[i]->deleteLater();

    for (int i = 0; i < widgetsMissedStatus.size(); ++i)
        widgetsMissedStatus[i]->deleteLater();

    qDeleteAll(labelsMissedStatus);
    layoutsMissedStatus.clear();
    widgetsMissedStatus.clear();
    labelsMissedStatus.clear();
}

QWidget* CallHistoryDialog::loadBusyStatus()
{
    QHBoxLayout* busyStatusLayout = new QHBoxLayout;
    QWidget* busyStatusWgt = new QWidget;
    QLabel * busyStatusLabel = new QLabel(busyStatusWgt);
    busyStatusLabel->setText(tr("Занято"));

    busyStatusLayout->addWidget(busyStatusLabel);
    busyStatusLayout->setContentsMargins(3, 0, 0, 0);
    busyStatusWgt->setLayout(busyStatusLayout);

    layoutsBusyStatus.append(busyStatusLayout);
    widgetsBusyStatus.append(busyStatusWgt);
    labelsBusyStatus.append(busyStatusLabel);
    return busyStatusWgt;
}

void CallHistoryDialog::deleteBusyStatusObjects()
{
    for (int i = 0; i < layoutsBusyStatus.size(); ++i)
        layoutsBusyStatus[i]->deleteLater();

    for (int i = 0; i < widgetsBusyStatus.size(); ++i)
        widgetsBusyStatus[i]->deleteLater();

    qDeleteAll(labelsBusyStatus);
    layoutsBusyStatus.clear();
    widgetsBusyStatus.clear();
    labelsBusyStatus.clear();
}

QWidget* CallHistoryDialog::loadCancelStatus()
{
    QHBoxLayout* cancelStatusLayout = new QHBoxLayout;
    QWidget* cancelStatusWgt = new QWidget;
    QLabel * cancelStatusLabel = new QLabel(cancelStatusWgt);
    cancelStatusLabel->setText(tr("Отклонено"));

    cancelStatusLayout->addWidget(cancelStatusLabel);
    cancelStatusLayout->setContentsMargins(3, 0, 0, 0);
    cancelStatusWgt->setLayout(cancelStatusLayout);

    layoutsCancelStatus.append(cancelStatusLayout);
    widgetsCancelStatus.append(cancelStatusWgt);
    labelsCancelStatus.append(cancelStatusLabel);
    return cancelStatusWgt;
}

void CallHistoryDialog::deleteCancelStatusObjects()
{
    for (int i = 0; i < layoutsCancelStatus.size(); ++i)
        layoutsCancelStatus[i]->deleteLater();

    for (int i = 0; i < widgetsCancelStatus.size(); ++i)
        widgetsCancelStatus[i]->deleteLater();

    qDeleteAll(labelsCancelStatus);
    layoutsCancelStatus.clear();
    widgetsCancelStatus.clear();
    labelsCancelStatus.clear();
}

QWidget* CallHistoryDialog::loadReceivedStatus()
{
    QHBoxLayout* receivedStatusLayout = new QHBoxLayout;
    QWidget* receivedStatusWgt = new QWidget;
    QLabel * receivedStatusLabel = new QLabel(receivedStatusWgt);
    receivedStatusLabel->setText(tr("Принятый"));

    receivedStatusLayout->addWidget(receivedStatusLabel);
    receivedStatusLayout->setContentsMargins(3, 0, 0, 0);
    receivedStatusWgt->setLayout(receivedStatusLayout);

    layoutsReceivedStatus.append(receivedStatusLayout);
    widgetsReceivedStatus.append(receivedStatusWgt);
    labelsReceivedStatus.append(receivedStatusLabel);
    return receivedStatusWgt;
}

void CallHistoryDialog::deleteReceivedStatusObjects()
{
    for (int i = 0; i < layoutsReceivedStatus.size(); ++i)
        layoutsReceivedStatus[i]->deleteLater();

    for (int i = 0; i < widgetsReceivedStatus.size(); ++i)
        widgetsReceivedStatus[i]->deleteLater();

    qDeleteAll(labelsReceivedStatus);
    layoutsReceivedStatus.clear();
    widgetsReceivedStatus.clear();
    labelsReceivedStatus.clear();
}

void CallHistoryDialog::loadMissedCalls()
{
    if(!widgetsMissed.isEmpty())
        deleteMissedObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query1 = new QSqlQueryModel;
    query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"') ORDER BY datetime DESC", dbAsterisk);
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("Имя"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query1->insertColumn(4);
    query1->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView->setModel(query1);
    ui->tableView->setColumnHidden(5, true);
    if(my_group.isEmpty())
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
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::loadReceivedCalls()
{
    if(!widgetsReceived.isEmpty())
        deleteReceivedObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query2 = new QSqlQueryModel;
    query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$') ORDER BY datetime DESC", dbAsterisk);

    query2->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    query2->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query2->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query2->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query2->insertColumn(4);
    query2->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_2->setModel(query2);
    ui->tableView_2->setColumnHidden(5, true);
    if(my_group.isEmpty())
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
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::loadPlacedCalls()
{
    if(!widgetsPlaced.isEmpty())
        deletePlacedObjects();

    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlDatabase db;
    QSqlQuery query(db);

    query3 = new QSqlQueryModel;
    query3->setQuery("SELECT extfield2, dst, src, datetime, uniqueid FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND src = '"+my_number+"' ORDER BY datetime DESC", dbAsterisk);

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
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::onUpdate()
{
    if(ui->tabWidget->currentIndex() == 0)
        loadAllCalls();
    if(ui->tabWidget->currentIndex() == 1)
        loadMissedCalls();
    if(ui->tabWidget->currentIndex() == 2)
        loadReceivedCalls();
    if(ui->tabWidget->currentIndex() == 3)
        loadPlacedCalls();

}

QWidget* CallHistoryDialog::loadAllNotes()
{
    QWidget* wgt = new QWidget;
    QLabel* note = new QLabel(wgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' order by datetime desc");
    query.exec();
    query.first();
    note->setText(query.value(0).toString());
    note->setWordWrap(true);

    widgets.append(wgt);
    notes.append(note);
    return wgt;
}

void CallHistoryDialog::deleteObjectsOfAllCalls()
{
    for (int i = 0; i < widgets.size(); ++i)
        widgets[i]->deleteLater();

    qDeleteAll(notes);
    widgets.clear();
    notes.clear();
    delete query4;
}

QWidget* CallHistoryDialog::loadMissedNote()
{
    QWidget* missedWgt = new QWidget;
    QLabel *missedNote = new QLabel(missedWgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' order by datetime desc");
    query.exec();
    query.first();
    missedNote->setText(query.value(0).toString());
    missedNote->setWordWrap(true);

    widgetsMissed.append(missedWgt);
    notesMissed.append(missedNote);
    return missedWgt;
}

void CallHistoryDialog::deleteMissedObjects()
{
    for (int i = 0; i < widgetsMissed.size(); ++i)
        widgetsMissed[i]->deleteLater();

    qDeleteAll(notesMissed);
    widgetsMissed.clear();
    notesMissed.clear();
    delete query1;
}

QWidget* CallHistoryDialog::loadReceivedNote()
{
    QWidget* receivedWgt = new QWidget;
    QLabel *receivedNote = new QLabel(receivedWgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' order by datetime desc");
    query.exec();
    query.first();
    receivedNote->setText(query.value(0).toString());
    receivedNote->setWordWrap(true);

    widgetsReceived.append(receivedWgt);
    notesReceived.append(receivedNote);
    return receivedWgt;
}

void CallHistoryDialog::deleteReceivedObjects()
{
    for (int i = 0; i < widgetsReceived.size(); ++i)
        widgetsReceived[i]->deleteLater();

    qDeleteAll(notesReceived);
    widgetsReceived.clear();
    notesReceived.clear();
    delete query2;
}

QWidget* CallHistoryDialog::loadPlacedNote()
{
    QHBoxLayout* layout = new QHBoxLayout;
    QWidget* placedWgt = new QWidget;
    QLabel *placedNote = new QLabel(placedWgt);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' order by datetime desc");
    query.exec();
    query.first();
    placedNote->setText(query.value(0).toString());
    placedNote->setWordWrap(true);

    layout->addWidget(placedNote);
    layout->setContentsMargins(3, 0, 0, 0);
    placedWgt->setLayout(layout);

    widgetsPlaced.append(placedWgt);
    notesPlaced.append(placedNote);
    return placedWgt;
}

void CallHistoryDialog::deletePlacedObjects()
{
    for (int i = 0; i < widgetsPlaced.size(); ++i)
        widgetsPlaced[i]->deleteLater();

    qDeleteAll(notesPlaced);
    widgetsPlaced.clear();
    notesPlaced.clear();
    delete query3;
}

void CallHistoryDialog::deleteObjects()
{
    deleteObjectsOfAllCalls();
    deleteMissedObjects();
    deleteReceivedObjects();
    deletePlacedObjects();
    deleteReceivedStatusObjects();
    deleteMissedStatusObjects();
    deleteCancelStatusObjects();
    deleteBusyStatusObjects();
    deleteNameObjects();
}
