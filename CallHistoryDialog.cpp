#include "CallHistoryDialog.h"
#include "ui_CallHistoryDialog.h"

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

    QRegExp RegExp("^[0-9]+$");
    validator = new QRegExpValidator(RegExp, this);
    ui->lineEdit_page->setValidator(validator);

    my_number = global::getExtensionNumber("extensions");
    my_group = global::getGroupExtensionNumber("group_extensions");
    setWindowTitle(QObject::tr("История звонков по номеру:") + " " + my_number);

    ui->comboBox_list->setVisible(false);

    connect(ui->playAudio,           &QPushButton::clicked, this, &CallHistoryDialog::onPlayAudioClick);
    connect(ui->callButton,          &QPushButton::clicked, this, &CallHistoryDialog::onCallClicked);
    connect(ui->addContactButton,    &QPushButton::clicked, this, &CallHistoryDialog::onAddContact);
    connect(ui->addOrgContactButton, &QPushButton::clicked, this, &CallHistoryDialog::onAddOrgContact);
    connect(ui->updateButton,        &QPushButton::clicked, this, &CallHistoryDialog::onUpdateClick);

    connect(ui->comboBox_2,  SIGNAL(currentTextChanged(QString)), this, SLOT(daysChanged()));

    connect(ui->tableView,   SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToMissed(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToReceived(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNoteToPlaced(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addNotes(const QModelIndex &)));

    connect(ui->tableView,   SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberMissed(const QModelIndex &)));
    connect(ui->tableView_2, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberReceived(const QModelIndex &)));
    connect(ui->tableView_3, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumberPlaced(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getNumber(const QModelIndex &)));
    connect(ui->tableView_4, SIGNAL(clicked(const QModelIndex &)), this, SLOT(getRecordpath(const QModelIndex &)));

    ui->tabWidget->setCurrentIndex(0);
    ui->tableView->  verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView_4->verticalHeader()->setSectionsClickable(false);
    ui->tableView->  horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_4->horizontalHeader()->setSectionsClickable(false);

    ui->tableView->setStyleSheet  ("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_2->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_3->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
    ui->tableView_4->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    go="default";
    page = "1";
    days = ui->comboBox_2->currentText();
    loadAllCalls();
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
}

void CallHistoryDialog::onPlayAudioClick()
{
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
    else
         QMessageBox::information(this, QObject::tr("Внимание"), QObject::tr("Данный вызов не имеет записи!"), QMessageBox::Ok);
}

void CallHistoryDialog::playerClosed(bool closed)
{
    if (closed)
        playAudioDialog = nullptr;
}

void CallHistoryDialog::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
    ui->comboBox_2->setCurrentText("7");
}

void CallHistoryDialog::daysChanged()
{
     days = ui->comboBox_2->currentText();
     go = "default";
     updateCount();
}

void CallHistoryDialog::tabSelected()
{
    go = "default";
    page = "1";
    updateCount();
}

void CallHistoryDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    updateCount();
}

void CallHistoryDialog::updateCount() {
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);
    if(ui->tabWidget->currentIndex() == 0)
    {
        query.prepare("SELECT COUNT(*) FROM cdr "
                      "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                      "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                      "'"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+""
                      "[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                      "'^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"' "
                    "OR src = '"+my_number+"')");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        loadAllCalls();
     }
    if(ui->tabWidget->currentIndex() == 1)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE (disposition = 'NO ANSWER'"
                      " OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                      "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' "
                      "OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' "
                      "OR dst = '"+my_group+"')");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        loadMissedCalls();
    }
    if(ui->tabWidget->currentIndex() == 2)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE disposition = 'ANSWERED' "
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                      "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$')");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        loadReceivedCalls();
    }
    if(ui->tabWidget->currentIndex() == 3)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND src = '"+my_number+"'");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        loadPlacedCalls();
    }
}

void CallHistoryDialog::getRecordpath(const QModelIndex &index)
{
    recordpath = query4->data(query4->index(index.row(), 8)).toString();
    recordpath.remove(0,16);
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
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Выберите одну запись!"), QMessageBox::Ok);
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
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Выберите одну запись!"), QMessageBox::Ok);
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
    deleteObjects();
    delete ui;
}

void CallHistoryDialog::onCallClicked()
{
    if ((ui->tabWidget->currentIndex() == 1 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 0 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Выберите одну запись!"), QMessageBox::Ok);
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
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Данный контакт принадлежит организации!"), QMessageBox::Ok);
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
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Данный контакт принадлежит физ. лицу!"), QMessageBox::Ok);
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
    if (!widgets.isEmpty())
        deleteObjectsOfAllCalls();
    if (!widgetsAllName.isEmpty())
        deleteNameObjects();
    if (!widgetsStatus.isEmpty())
        deleteStatusObjects();

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
    ui->label_pages->setText(tr("из ") + pages);
    query4 = new QSqlQueryModel;
    if (ui->lineEdit_page->text() == "1")
    {
        query4->setQuery("SELECT extfield1, src, dst, disposition, datetime, uniqueid, recordpath FROM cdr "
                        "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL'"
                        " OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                        "'"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+""
                        "[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                        "'^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"' "
                        "OR src = '"+my_number+"') ORDER BY datetime DESC LIMIT 0,"
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ", dbAsterisk);

    }
    else
    {
        query4->setQuery("SELECT extfield1, src, dst, disposition, datetime, uniqueid, recordpath FROM cdr "
                        "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                        "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                        "'"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+""
                        "[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                        "'^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"' "
                        "OR src = '"+my_number+"') ORDER BY datetime DESC LIMIT "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()), dbAsterisk);
    }

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
    ui->tableView_4->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {
        extfield1 = query4->data(query4->index(row_index, 0)).toString();
        src = query4->data(query4->index(row_index, 1)).toString();
        uniqueid = query4->data(query4->index(row_index, 7)).toString();
        dialogStatus = query4->data(query4->index(row_index, 3)).toString();

        ui->tableView_4->setIndexWidget(query4->index(row_index, 4), loadStatus());

        if (extfield1.isEmpty())
            ui->tableView_4->setIndexWidget(query4->index(row_index, 0), loadName());
        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();
        if(query.value(0).toInt() != 0)
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

    if (ui->tabWidget->currentIndex() == 0)
    {
        layoutsAllName.append(nameLayout);
        widgetsAllName.append(nameWgt);
        labelsAllName.append(nameLabel);
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        layoutsMissedName.append(nameLayout);
        widgetsMissedName.append(nameWgt);
        labelsMissedName.append(nameLabel);
    }
    if (ui->tabWidget->currentIndex() == 2)
    {
        layoutsReceivedName.append(nameLayout);
        widgetsReceivedName.append(nameWgt);
        labelsReceivedName.append(nameLabel);
    }
    if (ui->tabWidget->currentIndex() == 3)
    {
        layoutsPlacedName.append(nameLayout);
        widgetsPlacedName.append(nameWgt);
        labelsPlacedName.append(nameLabel);
    }

    return nameWgt;
}

void CallHistoryDialog::deleteNameObjects()
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        for (int i = 0; i < layoutsAllName.size(); ++i)
            layoutsAllName[i]->deleteLater();

        for (int i = 0; i < widgetsAllName.size(); ++i)
            widgetsAllName[i]->deleteLater();

        qDeleteAll(labelsAllName);
        layoutsAllName.clear();
        widgetsAllName.clear();
        labelsAllName.clear();
    }
    if (ui->tabWidget->currentIndex() == 1)
    {
        for (int i = 0; i < layoutsMissedName.size(); ++i)
            layoutsMissedName[i]->deleteLater();

        for (int i = 0; i < widgetsMissedName.size(); ++i)
            widgetsMissedName[i]->deleteLater();

        qDeleteAll(labelsMissedName);
        layoutsMissedName.clear();
        widgetsMissedName.clear();
        labelsMissedName.clear();
    }
    if (ui->tabWidget->currentIndex() == 2)
    {
        for (int i = 0; i < layoutsReceivedName.size(); ++i)
            layoutsReceivedName[i]->deleteLater();

        for (int i = 0; i < widgetsReceivedName.size(); ++i)
            widgetsReceivedName[i]->deleteLater();

        qDeleteAll(labelsReceivedName);
        layoutsReceivedName.clear();
        widgetsReceivedName.clear();
        labelsReceivedName.clear();
    }
    if (ui->tabWidget->currentIndex() == 3)
    {
        for (int i = 0; i < layoutsPlacedName.size(); ++i)
            layoutsPlacedName[i]->deleteLater();

        for (int i = 0; i < widgetsPlacedName.size(); ++i)
            widgetsPlacedName[i]->deleteLater();

        qDeleteAll(labelsPlacedName);
        layoutsPlacedName.clear();
        widgetsPlacedName.clear();
        labelsPlacedName.clear();
    }
}

QWidget* CallHistoryDialog::loadStatus()
{
    QHBoxLayout* statusLayout = new QHBoxLayout;
    QWidget* statusWgt = new QWidget;
    QLabel* statusLabel = new QLabel(statusWgt);

    if (dialogStatus == "NO ANSWER")
        statusLabel->setText(tr("Пропущенный"));
    else if (dialogStatus == "BUSY")
        statusLabel->setText(tr("Занято"));
    else if (dialogStatus == "CANCEL")
        statusLabel->setText(tr("Отколено"));
    else if (dialogStatus == "ANSWERED")
        statusLabel->setText(tr("Принятый"));

    statusLayout->addWidget(statusLabel);
    statusLayout->setContentsMargins(3, 0, 0, 0);
    statusWgt->setLayout(statusLayout);

    layoutsStatus.append(statusLayout);
    widgetsStatus.append(statusWgt);
    labelsStatus.append(statusLabel);
    return statusWgt;
}

void CallHistoryDialog::deleteStatusObjects()
{
    for (int i = 0; i < layoutsStatus.size(); ++i)
        layoutsStatus[i]->deleteLater();

    for (int i = 0; i < widgetsStatus.size(); ++i)
        widgetsStatus[i]->deleteLater();

    qDeleteAll(labelsStatus);
    layoutsStatus.clear();
    widgetsStatus.clear();
    labelsStatus.clear();
}

void CallHistoryDialog::loadMissedCalls()
{
    if (!widgetsMissed.isEmpty())
        deleteMissedObjects();
    if (!widgetsMissedName.isEmpty())
        deleteNameObjects();

    query1 = new QSqlQueryModel;
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
    ui->label_pages->setText(tr("из ") + pages);
    query4 = new QSqlQueryModel;
    if (ui->lineEdit_page->text() == "1")
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE "
                          "(disposition = 'NO ANSWER' "
                          "OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                          "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                          "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                          "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' "
                          "OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' "
                          "OR dst = '"+my_group+"') ORDER BY datetime DESC LIMIT 0,"
                          + QString::number(ui->lineEdit_page->text().toInt() *
                                            ui->comboBox_list->currentText().toInt()) + " ", dbAsterisk);

    }
    else
    {
        query1->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE "
                          "(disposition = 'NO ANSWER' "
                          "OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                          "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                          "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                          "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' "
                          "OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' "
                          "OR dst = '"+my_group+"') ORDER BY datetime DESC LIMIT "
                          + QString::number(ui->lineEdit_page->text().toInt()
                                            * ui->comboBox_list->currentText().toInt() -
                                            ui->comboBox_list->currentText().toInt()) + " , " +
                          QString::number(ui->comboBox_list->currentText().toInt()), dbAsterisk);
    }

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
        extfield1 = query1->data(query1->index(row_index, 0)).toString();
        if (extfield1.isEmpty())
            ui->tableView->setIndexWidget(query1->index(row_index, 0), loadName());

        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if (query.value(0).toInt() != 0)
            ui->tableView->setIndexWidget(query1->index(row_index, 4), loadMissedNote());
    }
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::loadReceivedCalls()
{
    if (!widgetsReceived.isEmpty())
        deleteReceivedObjects();
    if (!widgetsReceivedName.isEmpty())
        deleteNameObjects();

    query2 = new QSqlQueryModel;
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
    ui->label_pages->setText(tr("из ") + pages);
    query4 = new QSqlQueryModel;
    if (ui->lineEdit_page->text() == "1")
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED'"
                         " AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                         "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                         "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR "
                         "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$') "
                         "ORDER BY datetime DESC LIMIT 0,"
                         + QString::number(ui->lineEdit_page->text().toInt() *
                                           ui->comboBox_list->currentText().toInt()) + " ", dbAsterisk);
    }
    else
    {
        query2->setQuery("SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED'"
                         " AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                         "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                         "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR "
                         "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$') "
                         "ORDER BY datetime DESC LIMIT "
                         + QString::number(ui->lineEdit_page->text().toInt()
                                           * ui->comboBox_list->currentText().toInt() -
                                           ui->comboBox_list->currentText().toInt()) + " , " +
                         QString::number(ui->comboBox_list->currentText().toInt()), dbAsterisk);
    }

    query2->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    query2->setHeaderData(1, Qt::Horizontal, QObject::tr("Откуда"));
    query2->setHeaderData(2, Qt::Horizontal, QObject::tr("Кому"));
    query2->setHeaderData(3, Qt::Horizontal, QObject::tr("Дата и время"));
    query2->insertColumn(4);
    query2->setHeaderData(4, Qt::Horizontal, tr("Заметки"));

    ui->tableView_2->setModel(query2);
    ui->tableView_2->setColumnHidden(5, true);
    if (my_group.isEmpty())
        ui->tableView_2->setColumnHidden(2, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        uniqueid = query2->data(query2->index(row_index, 5)).toString();
        extfield1 = query2->data(query2->index(row_index, 0)).toString();
        if (extfield1.isEmpty())
            ui->tableView_2->setIndexWidget(query2->index(row_index, 0), loadName());

        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if (query.value(0).toInt() != 0)
            ui->tableView_2->setIndexWidget(query2->index(row_index, 4), loadReceivedNote());
    }
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::loadPlacedCalls()
{
    if (!widgetsPlaced.isEmpty())
        deletePlacedObjects();
    if (!widgetsPlacedName.isEmpty())
        deleteNameObjects();

    query3 = new QSqlQueryModel;

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
    ui->label_pages->setText(tr("из ") + pages);
    query4 = new QSqlQueryModel;
    if (ui->lineEdit_page->text() == "1")
    {
        query3->setQuery("SELECT extfield2, dst, src, datetime, uniqueid FROM cdr WHERE "
                         "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                         "src = '"+my_number+"' ORDER BY datetime DESC LIMIT 0,"
                         + QString::number(ui->lineEdit_page->text().toInt() *
                                           ui->comboBox_list->currentText().toInt()) + " ", dbAsterisk);
    }
    else
    {
        query3->setQuery("SELECT extfield2, dst, src, datetime, uniqueid FROM cdr WHERE "
                         "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                         "src = '"+my_number+"' ORDER BY datetime DESC LIMIT "
                         + QString::number(ui->lineEdit_page->text().toInt()
                                           * ui->comboBox_list->currentText().toInt() -
                                           ui->comboBox_list->currentText().toInt()) + " , " +
                         QString::number(ui->comboBox_list->currentText().toInt()), dbAsterisk);
    }

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
        extfield1 = query3->data(query3->index(row_index, 0)).toString();
        if (extfield1.isEmpty())
           ui->tableView_3->setIndexWidget(query3->index(row_index, 0), loadName());

        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();
        if (query.value(0).toInt() != 0)
            ui->tableView_3->setIndexWidget(query3->index(row_index, 4), loadPlacedNote());
    }

    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void CallHistoryDialog::onUpdateClick()
{
    go = "default";
    updateCount();
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
    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
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

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
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

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
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

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
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
    deleteStatusObjects();
    deleteNameObjects();
}


void CallHistoryDialog::on_previousButton_clicked()
{
    go = "previous";
    onUpdate();
}

void CallHistoryDialog::on_nextButton_clicked()
{
    go = "next";
    onUpdate();
}

void CallHistoryDialog::on_previousStartButton_clicked()
{
    go = "previousStart";
    onUpdate();
}

void CallHistoryDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";
    onUpdate();;
}

void CallHistoryDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";
    onUpdate();
}

