#include "AddOrgContactDialog.h"
#include "ui_AddOrgContactDialog.h"
#include "Global.h"

#include <QVariantList>
#include <QVariantMap>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>


AddOrgContactDialog::AddOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOrgContactDialog)
{
    ui->setupUi(this);
    QRegExp RegExp("^[\\+]?[0-9]{1,12}$");
    QValidator *Validator = new QRegExpValidator(RegExp, this);
    ui->FirstNumber->setValidator(Validator);
    ui->SecondNumber->setValidator(Validator);
    ui->ThirdNumber->setValidator(Validator);
    ui->FourthNumber->setValidator(Validator);
    ui->FifthNumber->setValidator(Validator);

    query1 = new QSqlQueryModel;

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText("Название организации<span style=\"color: red;\">*</span>");

    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddOrgContactDialog::onSave);
}

AddOrgContactDialog::~AddOrgContactDialog()
{
    delete ui;
}

void AddOrgContactDialog::onSave()
{
    if (g_Switch == "addOrg")
    {
        addOrgContact();
    }
    else if (g_Switch == "updateOrg")
    {
        updateOrgContact();
    }
}

void AddOrgContactDialog::addOrgContact()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString OrgName = QString(ui->OrgName->text());

    query.prepare("INSERT INTO entry (entry_type, entry_name, entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue("org");
    query.addBindValue(OrgName);
    query.addBindValue(OrgName);
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
    QSqlQuery query3(db);
    QSqlQuery query4(db);

    if(QString(ui->OrgName->text()).isEmpty() == true)
    {
         ui->label_15->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
         ui->OrgName->setStyleSheet("border: 1px solid red");
    }
    else{ ui->label_15->hide();  ui->OrgName->setStyleSheet("border: 1px solid grey"); }
    if(QString(ui->FirstNumber->text()).isEmpty() == true)
    {
        ui->label_14->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
        ui->FirstNumber->setStyleSheet("border: 1px solid red");
    }
    else{ ui->label_14->hide(); ui->FirstNumber->setStyleSheet("border: 1px solid grey"); }
    if(QString(ui->OrgName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->hide();
        ui->OrgName->setStyleSheet("border: 1px solid grey");
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

void AddOrgContactDialog::updateOrgContact()
{

}

void AddOrgContactDialog::setValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void AddOrgContactDialog::setOrgValuesContacts(int &i)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QSqlQueryModel *query1 = new QSqlQueryModel;
    query1->setQuery("SELECT entry_id FROM entry_phone GROUP BY entry_id");
    QString sql = QString("select entry_phone from entry_phone where entry_id = %1").arg(query1->data(query1->index(i, 0)).toString());
    query.prepare(sql);
    query.exec();
    query.next();
    QString firstNumber = query.value(0).toString();
    query.next();
    QString secondNumber = query.value(0).toString();
    query.next();
    QString thirdNumber = query.value(0).toString();
    query.next();
    QString fourthNumber = query.value(0).toString();
    query.next();
    QString fifthNumber = query.value(0).toString();
    sql = QString("select distinct entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment from entry where id = %1").arg(query1->data(query1->index(i, 0)).toString());
    query.prepare(sql);
    query.exec();
    query.next();
    QString entryORGName = query.value(0).toString();
    QString entryCity = query.value(1).toString();
    QString entryAddress = query.value(2).toString();
    QString entryEmail = query.value(3).toString();
    QString entryVyborID = query.value(4).toString();
    QString entryComment = query.value(5).toString();
    ui->FirstNumber->setText(firstNumber);
    ui->SecondNumber->setText(secondNumber);
    ui->ThirdNumber->setText(thirdNumber);
    ui->FourthNumber->setText(fourthNumber);
    ui->FifthNumber->setText(fifthNumber);
    ui->OrgName->setText(entryORGName);
    ui->City->setText(entryCity);
    ui->Address->setText(entryAddress);
    ui->Email->setText(entryEmail);
    ui->VyborID->setText(entryVyborID);
    ui->Comment->setText(entryComment);
}
