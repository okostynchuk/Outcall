#include "AddContactDialog.h"
#include "Global.h"
#include "ui_AddContactDialog.h"

#include <QVariantList>
#include <QVariantMap>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>


AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);
    QRegExp RegExp("^[\\+]?[0-9]{1,12}$");
    QValidator *Validator = new QRegExpValidator(RegExp, this);
    ui->FirstNumber->setValidator(Validator);
    ui->SecondNumber->setValidator(Validator);
    ui->ThirdNumber->setValidator(Validator);
    ui->FourthNumber->setValidator(Validator);
    ui->FifthNumber->setValidator(Validator);

    QRegExp RegExp2("^[A-ZА-Яa-zа-я]+[\\ |\\-]?[A-ZА-Яa-zа-я]+$");
    QValidator *ValidatorForName = new QRegExpValidator(RegExp2, this);
    ui->FirstName->setValidator(ValidatorForName);
    ui->LastName->setValidator(ValidatorForName);
    ui->Patronymic->setValidator(ValidatorForName);

    query1 = new QSqlQueryModel;

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText("Имя<span style=\"color: red;\">*</span>");

    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddContactDialog::onSave);
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}

void AddContactDialog::onSave()
{
    if (g_Switch == "addPerson")
    {
        addContact();
    }
    else if (g_Switch == "updatePerson")
    {
        updateContact();
    }
}

void AddContactDialog::addContact()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString lastname = QString(ui->LastName->text());
    QString firstname = QString(ui->FirstName->text());
    QString patronymic = QString(ui->Patronymic->text());


    query.prepare("INSERT INTO entry (entry_type, entry_name,entry_person_lname, entry_person_fname, entry_person_mname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue("person");
    if(ui->LastName->text().isEmpty())
    {
        query.addBindValue(firstname + ' ' + patronymic);
    }
    else { query.addBindValue(lastname + ' ' + firstname + ' ' + patronymic); }

    query.addBindValue(lastname);
    query.addBindValue(firstname);
    query.addBindValue(patronymic);
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());
    query.addBindValue(ui->Comment->toPlainText());

    QSqlQuery query1(db);
    QString sql1 = QString("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '%1' OR entry_phone = '%2' OR entry_phone = '%3');")
            .arg(ui->FirstNumber->text(),
            ui->SecondNumber->text(),
            ui->ThirdNumber->text(),
            ui->FourthNumber->text(),
            ui->FifthNumber->text());
    query1.prepare(sql1);
    query1.exec();
    query1.next();

    QSqlQuery query2(db);

    if(QString(ui->FirstName->text()).isEmpty() == true)
    {
         ui->label_15->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
         ui->FirstName->setStyleSheet("border: 1px solid red");
    }
    else{ ui->label_15->hide();  ui->FirstName->setStyleSheet("border: 1px solid grey"); }
    if(QString(ui->FirstNumber->text()).isEmpty() == true)
    {
        ui->label_14->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
        ui->FirstNumber->setStyleSheet("border: 1px solid red");
    }
    else{ ui->label_14->hide(); ui->FirstNumber->setStyleSheet("border: 1px solid grey"); }
    if(QString(ui->FirstName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->hide();
        ui->FirstName->setStyleSheet("border: 1px solid grey");
        ui->label_14->hide();
        ui->FirstNumber->setStyleSheet("border: 1px solid grey");
        if(query1.value(0) != 0){
            QMessageBox::information(this, trUtf8("Error"), trUtf8("Record is exists"));
        }
        else
        {
            query.exec();
            qint32 id=query.lastInsertId().toInt();
            QString firstnum = QString(ui->FirstNumber->text());
            QString secondnum = QString(ui->SecondNumber->text());
            QString thirdnum = QString(ui->ThirdNumber->text());
            QString fourthnum = QString(ui->FourthNumber->text());
            QString fifthnum = QString(ui->FifthNumber->text());
            if(firstnum!=0)
            {
                query2.prepare("INSERT INTO phone (entry_id, phone)"
                               "VALUES(?, ?)");
                query2.addBindValue(id);
                query2.addBindValue(ui->FirstNumber->text());
                query2.exec();
            }
            if(secondnum!=0)
            {
                query2.prepare("INSERT INTO phone (entry_id, phone)"
                               "VALUES(?, ?)");
                query2.addBindValue(id);
                query2.addBindValue(ui->SecondNumber->text());
                query2.exec();
            }
            if(thirdnum!=0)
            {
                query2.prepare("INSERT INTO phone (entry_id, phone)"
                               "VALUES(?, ?)");
                query2.addBindValue(id);
                query2.addBindValue(ui->ThirdNumber->text());
                query2.exec();
            }
            if(fourthnum!=0)
            {
                    query2.prepare("INSERT INTO phone (entry_id, phone)"
                                   "VALUES(?, ?)");
                    query2.addBindValue(id);
                    query2.addBindValue(ui->FourthNumber->text());
                    query2.exec();
            }

            if(fifthnum!=0)
            {
                    query2.prepare("INSERT INTO phone (entry_id, phone)"
                                   "VALUES(?, ?)");
                    query2.addBindValue(id);
                    query2.addBindValue(ui->FifthNumber->text());
                    query2.exec();
            }
            ui->label_16->setText("<span style=\"color: green;\">Запись успешно добавлена!</span>");
        }
    }
}

void AddContactDialog::updateContact()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString lastName = QString(ui->LastName->text());
    QString firstName = QString(ui->FirstName->text());
    QString patronymic = QString(ui->Patronymic->text());

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_person_lname = ?, entry_person_fname = ?, entry_person_mname = ?, entry_city = ?, entry_address = ?, entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
    query.addBindValue("person");
    if(ui->LastName->text().isEmpty())
    {
        query.addBindValue(firstName + ' ' + patronymic);
    }
    else { query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic); }

    query.addBindValue(lastName);
    query.addBindValue(firstName);
    query.addBindValue(patronymic);
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());
    query.addBindValue(ui->Comment->toPlainText());
    query.addBindValue(updateID);

    QSqlQuery query1(db);
    QString sql1 = QString("SELECT entry_phone FROM entry_phone WHERE entry_phone = '%1' AND NOT entry_id = %6 OR entry_phone = '%2' AND NOT entry_id = %6 OR entry_phone = '%3' AND NOT entry_id = %6 OR entry_phone = '%4' AND NOT entry_id = %6 OR entry_phone = '%5' AND NOT entry_id = %6")
            .arg(ui->FirstNumber->text(),
            ui->SecondNumber->text(),
            ui->ThirdNumber->text(),
            ui->FourthNumber->text(),
            ui->FifthNumber->text(),
            updateID);
    query1.prepare(sql1);
    query1.exec();
    query1.next();

    if (QString(ui->FirstName->text()).isEmpty() == true)
    {
         ui->label_15->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
         ui->FirstName->setStyleSheet("border: 1px solid red");
    }
    else { ui->label_15->hide();  ui->FirstName->setStyleSheet("border: 1px solid grey"); }
    if (QString(ui->FirstNumber->text()).isEmpty() == true)
    {
        ui->label_14->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
        ui->FirstNumber->setStyleSheet("border: 1px solid red");
    }
    else { ui->label_14->hide(); ui->FirstNumber->setStyleSheet("border: 1px solid grey"); }

    if (QString(ui->FirstName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->hide();
        ui->FirstName->setStyleSheet("border: 1px solid grey");
        ui->label_14->hide();
        ui->FirstNumber->setStyleSheet("border: 1px solid grey");

        if (!query1.value(0).isNull()){
            QMessageBox::information(this, trUtf8("Error"), trUtf8("Record exists"));
        }
        else
        {
            query.exec();
            QString firstNum = QString(ui->FirstNumber->text());
            QString secondNum = QString(ui->SecondNumber->text());
            QString thirdNum = QString(ui->ThirdNumber->text());
            QString fourthNum = QString(ui->FourthNumber->text());
            QString fifthNum = QString(ui->FifthNumber->text());
            QString sql1 = QString("select COUNT(phone) from phone where entry_id = %1").arg(updateID);
            query1.prepare(sql1);
            query1.exec();
            query1.next();
            int count = query1.value(0).toInt();

            if (firstNum != 0)
            {
                if (count > 0)
                {
                    query1.prepare("UPDATE phone SET phone = ? WHERE entry_id = ? AND phone = ?");
                    query1.addBindValue(firstNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(firstNumber);
                    query1.exec();
                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO phone (entry_id, phone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(firstNum);
                    query1.exec();
                }

            }
            if (secondNum != 0)
            {
                if (count > 0)
                {
                    query1.prepare("UPDATE phone SET phone = ? WHERE entry_id = ? AND phone = ?");
                    query1.addBindValue(secondNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(secondNumber);
                    query1.exec();
                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO phone (entry_id, phone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(secondNum);
                    query1.exec();
                }
            }
            if (thirdNum != 0)
            {
                if (count > 0)
                {
                    query1.prepare("UPDATE phone SET phone = ? WHERE entry_id = ? AND phone = ?");
                    query1.addBindValue(thirdNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(thirdNumber);
                    query1.exec();
                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO phone (entry_id, phone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(thirdNum);
                    query1.exec();
                }
            }
            if (fourthNum != 0)
            {
                if (count > 0)
                {
                    query1.prepare("UPDATE phone SET phone = ? WHERE entry_id = ? AND phone = ?");
                    query1.addBindValue(fourthNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(fourthNumber);
                    query1.exec();
                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO phone (entry_id, phone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(fourthNum);
                    query1.exec();
                }
            }
            if (fifthNum != 0)
            {
                if (count > 0)
                {
                    query1.prepare("UPDATE phone SET phone = ? WHERE entry_id = ? AND phone = ?");
                    query1.addBindValue(fifthNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(fifthNumber);
                    query1.exec();
                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO phone (entry_id, phone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(fifthNum);
                    query1.exec();
                }
            }
            ui->label_16->setText("<span style=\"color: green;\">Запись успешно добавлена!</span>");
        }
    }
}

void AddContactDialog::setValuesContacts(int &i)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query1 = new QSqlQueryModel;
    query1->setQuery("SELECT entry_id FROM entry_phone GROUP BY entry_id");
    QString sql = QString("select entry_phone from entry_phone where entry_id = %1").arg(query1->data(query1->index(i, 0)).toString());
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
    sql = QString("select distinct entry_person_fname, entry_person_mname, entry_person_lname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment from entry where id = %1").arg(query1->data(query1->index(i, 0)).toString());
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
    updateID = query1->data(query1->index(i, 0)).toString();
}



