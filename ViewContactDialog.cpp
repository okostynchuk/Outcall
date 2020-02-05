#include "ViewContactDialog.h"
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
    query.prepare("SELECT entry_person_org_id FROM entry WHERE id = " + updateID);
    query.exec();
    query.next();
    QString orgID = query.value(0).toString();
    query.prepare("SELECT entry_org_name FROM entry WHERE id = " + orgID);
    query.exec();
    query.next();
    ui->Organization->setText(query.value(0).toString());
    sql = QString("select distinct entry_person_fname, entry_person_mname, entry_person_lname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment from entry where id = %1").arg(updateID);
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

     if (!firstNumber.isEmpty())
          ui->FirstNumber->setInputMask("999-999-9999;_");
     if (!secondNumber.isEmpty())
          ui->SecondNumber->setInputMask("999-999-9999;_");
     if (!thirdNumber.isEmpty())
          ui->ThirdNumber->setInputMask("999-999-9999;_");
     if (!fourthNumber.isEmpty())
          ui->FourthNumber->setInputMask("999-999-9999;_");
     if (!fifthNumber.isEmpty())
          ui->FifthNumber->setInputMask("999-999-9999;_");
}

void ViewContactDialog::loadCalls(QString &contactNumber)
{
    QSqlDatabase db;
    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");
    QSqlQuery query(dbAsterisk);
    QSqlQuery query1(db);

    // Load calls
       SettingsDialog *settingsDialog = new SettingsDialog();
       QString number = settingsDialog->getExtension();
       //Load Missed calls
       query.prepare("SELECT src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'NO ANSWER' AND dst = ? AND scr = ? ORDER BY datetime DESC");
       query.addBindValue(number);
       query.addBindValue(contactNumber);
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

     //Load Recieved calls
       query.prepare("SELECT src, dst, datetime, uniqueid FROM cdr WHERE disposition = 'ANSWERED' AND dst = ? AND src = ? ORDER BY datetime DESC");
       query.addBindValue(number);
       query.addBindValue(contactNumber);
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

     //Load Placed calls
       query.prepare("SELECT dst, datetime, src, uniqueid FROM cdr WHERE src = ? AND dst = ? ORDER BY datetime DESC");
       query.addBindValue(number);
       query.addBindValue(contactNumber);
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
       settingsDialog->deleteLater();
}


void ViewContactDialog::addCall(const QMap<QString, QVariant> &call, ViewContactDialog::Calls calls)
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
