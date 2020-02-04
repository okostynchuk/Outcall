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

