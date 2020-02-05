#include "CallHistoryDialog.h"
#include "ui_callhistorydialog.h"
#include "Global.h"
#include "ContactManager.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "SettingsDialog.h"
#include "AddNoteDialog.h"
#include "OutCALL.h"

#include <QDebug>
#include <QList>
#include <QMessageBox>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QFile>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <QString>

CallHistoryDialog::CallHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallHistoryDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->removeButton, &QPushButton::clicked, this, &CallHistoryDialog::onRemoveButton);
    connect(ui->callButton,   &QPushButton::clicked, this, &CallHistoryDialog::onCallClicked);
    connect(ui->addContactButton, &QPushButton::clicked, this, &CallHistoryDialog::onAddContact);
    connect(ui->addOrgContactButton, &QPushButton::clicked, this, &CallHistoryDialog::onAddOrgContact);
    connect(ui->addNotes, &QPushButton::clicked, this, &CallHistoryDialog::onAddNotes);

    ui->tabWidget->setCurrentIndex(0);

    state_call = "missed";
    loadCalls(state_call);
//    state_call = "recieved";
//    loadCalls(state_call);
//    state_call = "placed";
//    loadCalls(state_call);

    //Date_time column size
    ui->treeWidgetMissed->setColumnWidth(3, 115);
    ui->treeWidgetReceived->setColumnWidth(3, 115);
    ui->treeWidgetPlaced->setColumnWidth(2, 115);
}

CallHistoryDialog::~CallHistoryDialog()
{
    delete ui;
}

void CallHistoryDialog::addCall(const QMap<QString, QVariant> &call, CallHistoryDialog::Calls calls)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QSqlQuery query1(db);

    const QString from     = call.value("from").toString();
    const QString to       = call.value("to").toString();
    const QString dateTime = call.value("date_time").toString();
    QString note           = call.value("note").toString();
    QString callerIDName;

    query.prepare("SELECT EXISTS(SELECT entry_name FROM entry WHERE id IN (SELECT entry_id FROM phone WHERE phone ="+from+"))");
    query.exec();
    query.first();
    if(query.value(0) != 0)
    {
        query1.prepare("SELECT entry_name FROM entry WHERE id IN (SELECT entry_id FROM phone WHERE phone = "+from+")");
        query1.exec();
        query1.first();
        callerIDName = query1.value(0).toString();
    }
    else
    {
        callerIDName = "Неизвестный";
    }

    if (calls == MISSED)
    {
        QTreeWidgetItem *extensionItem = new QTreeWidgetItem(ui->treeWidgetMissed);
        extensionItem->setText(0, callerIDName);
        extensionItem->setText(1, from);
        extensionItem->setText(2, to);
        extensionItem->setText(3, dateTime);
        extensionItem->setText(4, note);
        missed_count++;
    }
    else if (calls == RECIEVED)
    {
        QTreeWidgetItem *extensionItem = new QTreeWidgetItem(ui->treeWidgetReceived);
        extensionItem->setText(0, callerIDName);
        extensionItem->setText(1, from);
        extensionItem->setText(2, to);
        extensionItem->setText(3, dateTime);
        extensionItem->setText(4, note);
    }
    else if (calls == PLACED)
    {
        QTreeWidgetItem *extensionItem = new QTreeWidgetItem(ui->treeWidgetPlaced);
        extensionItem->setText(0, from);
        extensionItem->setText(1, to);
        extensionItem->setText(2, dateTime);
        extensionItem->setText(3, note);
    }
}

void CallHistoryDialog::onCallClicked()
{
    QList<QTreeWidgetItem*> selectedItems;

    if (ui->tabWidget->currentIndex() == MISSED)
    {
       selectedItems = ui->treeWidgetMissed->selectedItems();

       if (selectedItems.size() == 0)
           return;

       QTreeWidgetItem *item = selectedItems.at(0);
       const QString from = item->text(1);
       int ind1 = item->text(2).indexOf("(");
       int ind2 = item->text(2).indexOf(")");
       const QString to = item->text(2).mid(0, ind1);
       const QString protocol = item->text(2).mid(ind1 + 1, ind2 - ind1 - 1);

       g_pAsteriskManager->originateCall(to, from, protocol, to);
    }
    else if (ui->tabWidget->currentIndex() == RECIEVED)
    {
       selectedItems = ui->treeWidgetReceived->selectedItems();

       if (selectedItems.size() == 0)
           return;

       QTreeWidgetItem *item = selectedItems.at(0);
       const QString from = item->text(1);
       int ind1 = item->text(2).indexOf("(");
       int ind2 = item->text(2).indexOf(")");
       const QString to = item->text(2).mid(0, ind1);
       const QString protocol = item->text(2).mid(ind1+1, ind2-ind1-1);

       g_pAsteriskManager->originateCall(to, from, protocol, to);
    }
    else if (ui->tabWidget->currentIndex() == PLACED)
    {
       selectedItems = ui->treeWidgetPlaced->selectedItems();

       if (selectedItems.size() == 0)
           return;

       QTreeWidgetItem *item = selectedItems.at(0);
       int ind1 = item->text(0).indexOf("(");
       int ind2 = item->text(0).indexOf(")");
       const QString from = item->text(0).mid(0, ind1);
       const QString to = item->text(1);
       const QString protocol = item->text(0).mid(ind1 + 1, ind2 - ind1 - 1);

       g_pAsteriskManager->originateCall(from, to, protocol, from);
    }
}

void CallHistoryDialog::onAddContact()
{
    addContactDialog = new AddContactDialog;
    addContactDialog->setWindowTitle("Добавление физ. лица");

    if (ui->tabWidget->currentIndex() == MISSED)
        {
            QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetMissed->selectedItems();
            if (selectedItems.size() == 0 || selectedItems.size() > 1)
                return;

            QTreeWidgetItem *item = selectedItems.at(0);
            QString from = item->text(1);
            bool a = checkNumber(from);
            if (a == true)
            {  
                addContactDialog->setValuesCallHistory(from);
                addContactDialog->exec();
                addContactDialog->deleteLater();
            }
            else if (a == false) { editContact(from); }
        }
        else if (ui->tabWidget->currentIndex() == RECIEVED)
        {
            QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetReceived->selectedItems();
            if (selectedItems.size() == 0 || selectedItems.size() > 1)
                return;

            QTreeWidgetItem *item = selectedItems.at(0);
            //QString name = item->text(0);
            QString from = item->text(1);

            bool a = checkNumber(from);
            if (a == true)
            {
                addContactDialog->setValuesCallHistory(from);
                addContactDialog->exec();
                addContactDialog->deleteLater();
            }
            else if (a == false) { editContact(from); }

        }
        else if (ui->tabWidget->currentIndex() == PLACED)
        {
            QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetPlaced->selectedItems();
            if (selectedItems.size() == 0 || selectedItems.size() > 1)
                return;

            QTreeWidgetItem *item = selectedItems.at(0);
            int ind1 = item->text(1).indexOf("(");
            QString to = item->text(0).mid(0, ind1);

            bool a = checkNumber(to);
            if (a == true)
            {
                addContactDialog->setValuesCallHistory(to);
                addContactDialog->exec();
                addContactDialog->deleteLater();
            }
            else if (a == false) { editContact(to); }
        }
}

void CallHistoryDialog::onAddOrgContact()
{
    addOrgContactDialog = new AddOrgContactDialog;
    addOrgContactDialog->setWindowTitle("Добавление организации");

    if (ui->tabWidget->currentIndex() == MISSED)
        {
            QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetMissed->selectedItems();
            if (selectedItems.size() == 0 || selectedItems.size() > 1)
                return;

            QTreeWidgetItem *item = selectedItems.at(0);
            //const QString name = item->text(0);
            QString from = item->text(1);

            bool a = checkNumber(from);
            if (a == true)
            {
                addOrgContactDialog->setOrgValuesCallHistory(from);
                addOrgContactDialog->exec();
                addOrgContactDialog->deleteLater();
            }
            else if (a == false) { editContact(from); }
        }
        else if (ui->tabWidget->currentIndex() == RECIEVED)
        {
            QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetReceived->selectedItems();
            if (selectedItems.size() == 0 || selectedItems.size() > 1)
                return;

            QTreeWidgetItem *item = selectedItems.at(0);
            //QString name = item->text(0);
            QString from = item->text(1);

            bool a = checkNumber(from);
            if (a == true)
            {
                addOrgContactDialog->setOrgValuesCallHistory(from);
                addOrgContactDialog->exec();
                addOrgContactDialog->deleteLater();
            }
            else if (a == false) { editContact(from); }
        }
        else if (ui->tabWidget->currentIndex() == PLACED)
        {
            QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetPlaced->selectedItems();
            if (selectedItems.size() == 0 || selectedItems.size() > 1)
                return;

            QTreeWidgetItem *item = selectedItems.at(0);
            int ind1 = item->text(1).indexOf("(");
            QString to = item->text(1).mid(0, ind1);

            bool a = checkNumber(to);
            if (a == true)
            {
                addOrgContactDialog->setOrgValuesCallHistory(to);
                addOrgContactDialog->exec();
                addOrgContactDialog->deleteLater();
            }
            else if (a == false) { editContact(to); }
        }
}

void CallHistoryDialog::onRemoveButton()
{
    auto messageBox = [=](bool openBox)->bool
    {
        if (!openBox)
            return false;

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr(APP_NAME), tr("Are you sure you want to delete the selected items ?"),
                                          QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No)
            return false;
        return true;
    };

    if (ui->tabWidget->currentIndex() == MISSED)
    {
        QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetMissed->selectedItems();

        if (messageBox(selectedItems.size() > 0) == false)
            return;

        for (int i = 0; i < selectedItems.size(); ++i)
        {
            QTreeWidgetItem *item = selectedItems.at(i);
            int index = ui->treeWidgetMissed->indexOfTopLevelItem(item);
            ui->treeWidgetMissed->takeTopLevelItem(index);
        }
        global::removeSettinsKey("missed", "calls");

        QVariantList missedList;
        for (int i = 0; i < ui->treeWidgetMissed->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem *item       = ui->treeWidgetMissed->topLevelItem(i);
            const QString callerIDName  = item->text(0);
            const QString from          = item->text(1);

            int ind1 = item->text(2).indexOf("(");
            int ind2 = item->text(2).indexOf(")");

            const QString to       = item->text(1).mid(0, ind1);
            const QString protocol = item->text(2).mid(ind1 + 1, ind2 - ind1 - 1);
            const QString dateTime = item->text(3);

            QMap<QString, QVariant> missed;
            missed.insert("from", from);
            missed.insert("to", to);
            missed.insert("protocol", protocol);
            missed.insert("date_time", dateTime);
            missed.insert("callerIDName", callerIDName);

            missedList.append(QVariant::fromValue(missed));
        }
        global::setSettingsValue("missed", missedList, "calls");

    }
    else if (ui->tabWidget->currentIndex() == RECIEVED)
    {
        QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetReceived->selectedItems();

        if (messageBox(selectedItems.size() > 0) == false)
            return;

        for (int i = 0; i < selectedItems.size(); ++i)
        {
            QTreeWidgetItem *item = selectedItems.at(i);
            int index = ui->treeWidgetReceived->indexOfTopLevelItem(item);
            ui->treeWidgetReceived->takeTopLevelItem(index);
        }
        global::removeSettinsKey("received", "calls");

        QVariantList receivedList;

        for (int i = 0; i < ui->treeWidgetReceived->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem *item       = ui->treeWidgetReceived->topLevelItem(i);
            const QString callerIDName  = item->text(0);
            const QString from          = item->text(1);

            int ind1 = item->text(2).indexOf("(");
            int ind2 = item->text(2).indexOf(")");

            const QString to       = item->text(1).mid(0, ind1);
            const QString protocol = item->text(2).mid(ind1 + 1, ind2 - ind1 - 1);
            const QString dateTime = item->text(3);

            QMap<QString, QVariant> received;
            received.insert("from", from);
            received.insert("to", to);
            received.insert("protocol", protocol);
            received.insert("date_time", dateTime);
            received.insert("callerIDName", callerIDName);

            receivedList.append(QVariant::fromValue(received));
        }
        global::setSettingsValue("received", receivedList, "calls");
    }
    else if (ui->tabWidget->currentIndex() == PLACED)
    {
        QList<QTreeWidgetItem*> selectedItems = ui->treeWidgetPlaced->selectedItems();

        if (messageBox(selectedItems.size() > 0) == false)
            return;

        for (int i = 0; i < selectedItems.size(); ++i)
        {
            QTreeWidgetItem *item = selectedItems.at(i);
            int index = ui->treeWidgetPlaced->indexOfTopLevelItem(item);
            ui->treeWidgetPlaced->takeTopLevelItem(index);
        }
        global::removeSettinsKey("placed", "calls");

        QVariantList placedList;

        for (int i = 0; i < ui->treeWidgetPlaced->topLevelItemCount(); ++i)
        {
            QTreeWidgetItem *item   = ui->treeWidgetPlaced->topLevelItem(i);
            const QString from      = item->text(0);

            int ind1 = item->text(1).indexOf("(");
            int ind2 = item->text(1).indexOf(")");

            const QString to        = item->text(1).mid(0, ind1);
            const QString protocol  = item->text(1).mid(ind1 + 1, ind2 - ind1 - 1);
            const QString dateTime  = item->text(2);

            QMap<QString, QVariant> placed;
            placed.insert("from", from);
            placed.insert("to", to);
            placed.insert("protocol", protocol);
            placed.insert("date_time", dateTime);

            placedList.append(QVariant::fromValue(placed));
        }
        global::setSettingsValue("placed", placedList, "calls");
    }
}

bool CallHistoryDialog::checkNumber(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query1(db);
    query1.prepare("SELECT phone FROM phone WHERE phone ="+number);
    query1.exec();
    query1.next();
    if(query1.value(0).toString() == 0) { return true; }
    else { return false; }
}

void CallHistoryDialog::onAddNotes()
{
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);
    QSqlDatabase db;
    QSqlQuery query1(db);

    QList<QTreeWidgetItem*> selectedItems;
    if (ui->tabWidget->currentIndex() == MISSED)
    {
       selectedItems = ui->treeWidgetMissed->selectedItems();

       if (selectedItems.size() == 0)
           return;

       QTreeWidgetItem *item = selectedItems.at(0);
       const QString from = item->text(1);
       const QString to = item->text(2);
       const QString date_time = item->text(3);

       QString sql = QString("SELECT uniqueid FROM cdr WHERE src = '%1' AND dst = '%2' AND datetime = '%3'")
               .arg(from,
                    to,
                    date_time);
       query.prepare(sql);
       query.exec();
       query.next();
       QString uniqueid = query.value(0).toString();

       query1.prepare("SELECT EXISTS(SELECT uniqueid FROM calls WHERE uniqueid ="+uniqueid+")");
       query1.exec();
       query1.next();
       if(query1.value(0) != 0)
       {
           QString state = "edit";
           addNoteDialog = new AddNoteDialog;
           addNoteDialog->setWindowTitle("Редактирование заметки");
           addNoteDialog->setCallId(uniqueid, state);
           addNoteDialog->exec();
           addNoteDialog->deleteLater();
           ui->treeWidgetMissed->clear();
           state_call = "missed";
           loadCalls(state_call);
       }
       else
       {
           QString state = "add";
           addNoteDialog = new AddNoteDialog;
           addNoteDialog->setWindowTitle("Добавление заметки");
           addNoteDialog->setCallId(uniqueid, state);
           addNoteDialog->exec();
           addNoteDialog->deleteLater();
           ui->treeWidgetMissed->clear();
           state_call= "missed";
           loadCalls(state_call);
       }
    }
    else if (ui->tabWidget->currentIndex() == RECIEVED)
    {
       selectedItems = ui->treeWidgetReceived->selectedItems();

       if (selectedItems.size() == 0)
           return;

       QTreeWidgetItem *item = selectedItems.at(0);
       const QString from = item->text(1);
       const QString to = item->text(2);
       const QString date_time = item->text(3);

       QString sql = QString("SELECT uniqueid FROM cdr WHERE src = '%1' AND dst = '%2' AND datetime = '%3'")
               .arg(from,
                    to,
                    date_time);
       query.prepare(sql);
       query.exec();
       query.next();
       QString uniqueid = query.value(0).toString();

       query1.prepare("SELECT EXISTS(SELECT uniqueid FROM calls WHERE uniqueid ="+uniqueid+")");
       query1.exec();
       query1.next();
       if(query1.value(0) != 0)
       {
           QString state = "edit";
           addNoteDialog = new AddNoteDialog;
           addNoteDialog->setWindowTitle("Редактирование заметки");
           addNoteDialog->setCallId(uniqueid, state);
           addNoteDialog->exec();
           addNoteDialog->deleteLater();
       }
       else
       {
           QString state = "add";
           addNoteDialog = new AddNoteDialog;
           addNoteDialog->setWindowTitle("Добавление заметки");
           addNoteDialog->setCallId(uniqueid, state);
           addNoteDialog->exec();
           addNoteDialog->deleteLater();
       }
    }
    else if (ui->tabWidget->currentIndex() == PLACED)
    {
       selectedItems = ui->treeWidgetPlaced->selectedItems();

       if (selectedItems.size() == 0)
           return;

       QTreeWidgetItem *item = selectedItems.at(0);
       const QString from = item->text(0);
       const QString to = item->text(1);
       const QString date_time = item->text(2);

       QString sql = QString("SELECT uniqueid FROM cdr WHERE src = '%1' AND dst = '%2' AND datetime = '%3'")
               .arg(from,
                    to,
                    date_time);
       query.prepare(sql);
       query.exec();
       query.next();
       QString uniqueid = query.value(0).toString();

       query1.prepare("SELECT EXISTS(SELECT uniqueid FROM calls WHERE uniqueid ="+uniqueid+")");
       query1.exec();
       query1.next();
       if(query1.value(0) != 0)
       {
           QString state = "edit";
           addNoteDialog = new AddNoteDialog;
           addNoteDialog->setWindowTitle("Редактирование заметки");
           addNoteDialog->setCallId(uniqueid, state);
           addNoteDialog->exec();
           addNoteDialog->deleteLater();
       }
       else
       {
           QString state = "add";
           addNoteDialog = new AddNoteDialog;
           addNoteDialog->setWindowTitle("Добавление заметки");
           addNoteDialog->setCallId(uniqueid, state);
           addNoteDialog->exec();
           addNoteDialog->deleteLater();
       }
    }
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
        editContactDialog->setWindowTitle("Редактирование физ. лица");
        editContactDialog->setValuesContacts(updateID);
        editContactDialog->exec();
        editContactDialog->deleteLater();
    }
    else
    {
        editOrgContactDialog = new EditOrgContactDialog;
        editOrgContactDialog->setWindowTitle("Редактирование организации");
        editOrgContactDialog->setOrgValuesContacts(updateID);
        editOrgContactDialog->exec();
        editOrgContactDialog->deleteLater();
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

void CallHistoryDialog::loadCalls(QString &state)
{
    QSqlDatabase db;
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);
    QSqlQuery query1(db);

    // Load calls
       SettingsDialog *settingsDialog = new SettingsDialog();
       QString number = settingsDialog->getExtension();
       //Load Missed calls
       if(state == "missed")
       {
           query.prepare("SELECT src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = ?");
           query.addBindValue(number);
           query.exec();
           while(query.next()) {
               QMap<QString, QVariant> call;
               QString uniqueid = query.value(3).toString();
               query1.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
               query1.exec();
               query1.first();
               if(query1.value(0) != 0)
               {
                   query1.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
                   query1.exec();
                   query1.first();
                   call["note"] = query1.value(0).toString();
               }
               call["from"] = query.value(0).toString();
               call["to"] = query.value(1).toString();
               call["date_time"] = query.value(2).toString();
               addCall(call, MISSED);
           }
       }


     //Load Recieved calls
       if(state == "recieved")
       {
           query.prepare("SELECT src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = ? ORDER BY datetime DESC");
           query.addBindValue(number);
           query.exec();
           query.first();
           while(query.next()) {
               QMap<QString, QVariant> call;
               QString uniqueid = query.value(3).toString();
               query1.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
               query1.exec();
               query1.first();
               if(query1.value(0) != 0)
               {
                   query1.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
                   query1.exec();
                   query1.first();
                   call["note"] = query1.value(0).toString();
               }
               call["from"] = query.value(0).toString();
               call["to"] = query.value(1).toString();
               call["date_time"] = query.value(2).toString();
               addCall(call, RECIEVED);
           }
       }


     //Load Placed calls
       if(state == "placed")
       {
           query.prepare("SELECT dst, datetime, src, uniqueid FROM cdr WHERE src = ? ORDER BY datetime DESC");
                  query.addBindValue(number);
           query.exec();
           query.first();
           while(query.next()) {
               QMap<QString, QVariant> call;
               QString uniqueid = query.value(3).toString();
               query1.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
               query1.exec();
               query1.first();
               if(query1.value(0) != 0)
               {
                   query1.prepare("SELECT note FROM calls WHERE uniqueid ="+uniqueid);
                   query1.exec();
                   query1.first();
                   call["note"] = query1.value(0).toString();
               }
               call["from"] = query.value(0).toString();
               call["date_time"] = query.value(1).toString();
               call["to"] = query.value(2).toString();
               addCall(call, PLACED);
           }
       }
       settingsDialog->deleteLater();
}

void CallHistoryDialog::clear()
{
    ui->treeWidgetMissed->clear();
}
