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

}



