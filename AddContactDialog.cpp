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
    query1 = new QSqlQueryModel;

    ui->textEdit->setReadOnly(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddContactDialog::onSave);
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}

void AddContactDialog::onSave()
{
    //QSqlDatabase db;
    QSqlQuery query;

    /*query.prepare("INSERT INTO entry (entry_type, entry_person_lname, entry_person_fname, entry_person_mname, entry_city, entry_address, entry_email, entry_vybor_id)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue("person");
    query.addBindValue(ui->LastName->text());
    query.addBindValue(ui->FirstName->text());
    query.addBindValue(ui->Patronymic->text());
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());
    query.exec();*/
    //QSqlQuery query(db);


   // QString sql = QString("SELECT EXIST(SELECT" entry_phone "FROM" entry_phone WHERE entry_phone IN('%1', '%2', '%3')").arg(phone1).arg(phone2).arg(phone3);

    QString sql1 = QString("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '%1');")
            .arg(ui->FirstNumber->text());
    //QString sql2 = "SELECT entry_phone FROM entry_phone WHERE entry_phone ="+phone2;
   // QString sql3 = "SELECT entry_phone FROM entry_phone WHERE entry_phone ="+phone3;

    query.prepare(sql1);
    query.exec();
    //query.next();

    /* Если запись существует, то вызывается
     * информационное сообщение
     * */
    if(query.value(0) != 0){
        QMessageBox::information(this, trUtf8("Error"), trUtf8("Record is exists"));
    }
    else
    {
        // ToDo something
    }
    //if(query.exec(sql)==true || query.exec(sql2)==true || query.exec(sql3)==true)
    /*if(query.exec(sql1))
    {
        ui->textEdit->append("Такой номер уже есть!");
    }*/
}

