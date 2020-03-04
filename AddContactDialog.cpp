#include "AddContactDialog.h"
#include "ui_AddContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>
#include <QLineEdit>
#include <QStringList>
#include <QDebug>
AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText(tr("Имя:<span style=\"color: red;\">*</span>"));
    ui->label_org->setText(tr("Нет"));

    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddContactDialog::onSave);
}

AddContactDialog::~AddContactDialog()
{
    delete ui;
}

void AddContactDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString lastName = QString(ui->LastName->text());
    QString firstName = QString(ui->FirstName->text());
    QString patronymic = QString(ui->Patronymic->text());
    if (QString(ui->FirstName->text()).isEmpty() == true)
    {
         ui->label_15->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));
         ui->FirstName->setStyleSheet("border: 1px solid red");
    }
    else { ui->label_15->hide();  ui->FirstName->setStyleSheet("border: 1px solid grey"); }
    if (QString(ui->FirstNumber->text()).isEmpty() == true)
    {
        ui->label_14->show();
        ui->label_14->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));
        ui->FirstNumber->setStyleSheet("border: 1px solid red");
    }
    else { ui->label_14->hide(); ui->FirstNumber->setStyleSheet("border: 1px solid grey"); }

    if (QString(ui->FirstName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->hide();
        ui->FirstName->setStyleSheet("border: 1px solid grey");
        ui->label_14->hide();
        ui->FirstNumber->setStyleSheet("border: 1px solid grey");
        ui->SecondNumber->setStyleSheet("border: 1px solid grey");
        ui->ThirdNumber->setStyleSheet("border: 1px solid grey");
        ui->FourthNumber->setStyleSheet("border: 1px solid grey");
        ui->FifthNumber->setStyleSheet("border: 1px solid grey");

        numbers.clear();
        QSqlQuery query1(db);
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FirstNumber->text() + "')");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->FirstNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->FirstNumber->text());
        }        
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->SecondNumber->text() + "')");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->SecondNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->SecondNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->ThirdNumber->text() + "')");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->ThirdNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->ThirdNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FourthNumber->text() + "')");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->FourthNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->FourthNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FifthNumber->text() + "')");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->FifthNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->FifthNumber->text());
        }
        if (!numbers.isEmpty())
        {
            QString str = numbers.join(", ");
            QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Введены существующие номера!\n%1").arg(str), QMessageBox::Ok);
        }
        else
        {
            QString firstNum = QString(ui->FirstNumber->text());
            QString secondNum = QString(ui->SecondNumber->text());
            QString thirdNum = QString(ui->ThirdNumber->text());
            QString fourthNum = QString(ui->FourthNumber->text());
            QString fifthNum = QString(ui->FifthNumber->text());
            bool validPhones = true;

            if (firstNum != 0)
            {
                if (isPhone(&firstNum) == true)
                { ui->FirstNumber->setStyleSheet("border: 1px solid grey");}
                else
                {
                    validPhones = false;
                    ui->FirstNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (secondNum != 0)
            {
                if (isPhone(&secondNum) == true)
                { ui->SecondNumber->setStyleSheet("border: 1px solid grey"); }
                else
                {
                    validPhones = false;
                    ui->SecondNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (thirdNum != 0)
            {
                if (isPhone(&thirdNum) == true)
                { ui->ThirdNumber->setStyleSheet("border: 1px solid grey");}
                else
                {
                    validPhones = false;
                    ui->ThirdNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (fourthNum != 0)
            {
                if (isPhone(&fourthNum) == true)
                { ui->FourthNumber->setStyleSheet("border: 1px solid grey");  }
                else
                {
                    validPhones = false;
                    ui->FourthNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (fifthNum != 0)
            {
                if (isPhone(&fifthNum) == true)
                { ui->FifthNumber->setStyleSheet("border: 1px solid grey");}
                else
                {
                    validPhones = false;
                    ui->FifthNumber->setStyleSheet("border: 1px solid red");
                }
            }         

            if (!validPhones)
                QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Номер не соответствует формату!"), QMessageBox::Ok);
            else
            {
                query.prepare("INSERT INTO entry (entry_type, entry_name, entry_person_org_id, entry_person_lname, entry_person_fname, entry_person_mname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment)"
                              "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                query.addBindValue("person");
                if(ui->LastName->text().isEmpty())
                {
                    query.addBindValue(firstName + ' ' + patronymic);
                }
                else { query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic); }

                QString orgName = ui->label_org->text();
                if (orgName != "Нет" || orgName != "Any" || orgName != "Відсутня")
                {
                    QSqlQuery queryOrg(db);
                    QString sqlOrg = QString("SELECT id FROM entry WHERE entry_org_name = '%1'").arg(orgName);
                    queryOrg.prepare(sqlOrg);
                    queryOrg.exec();
                    queryOrg.next();
                    query.addBindValue(queryOrg.value(0).toString());
                }
                else
                    query.addBindValue(NULL);

                query.addBindValue(lastName);
                query.addBindValue(firstName);
                query.addBindValue(patronymic);
                query.addBindValue(ui->City->text());
                query.addBindValue(ui->Address->text());
                query.addBindValue(ui->Email->text());
                query.addBindValue(ui->VyborID->text());
                query.addBindValue(ui->Comment->toPlainText());
                query.exec();

                int id = query.lastInsertId().toInt();
                query1.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->FirstNumber->text());
                query1.exec();

                query1.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->SecondNumber->text());
                query1.exec();

                ui->ThirdNumber->setStyleSheet("border: 1px solid grey");
                query1.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->ThirdNumber->text());
                query1.exec();

                query1.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->FourthNumber->text());
                query1.exec();

                query1.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->FifthNumber->text());
                query1.exec();

                emit sendData(true);
                close();
                QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Запись успешно добавлена!"), QMessageBox::Ok);
                destroy(true);
            }
        }
    }
}

bool AddContactDialog::isPhone(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[\\+]?[0-9]{1,12}"));
    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;
    return false;
}

void AddContactDialog::receiveOrgID(QString &id)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT entry_name FROM entry_phone WHERE entry_id = " + id);
    query.exec();
    query.first();
    if (!query.value(0).toString().isEmpty())
        ui->label_org->setText(query.value(0).toString());
    else
        ui->label_org->setText(tr("Нет"));
}

void AddContactDialog::on_addOrgButton_clicked()
{
    addOrgToPerson = new AddOrgToPerson;
    connect(addOrgToPerson, SIGNAL(sendOrgID(QString&)), this, SLOT(receiveOrgID(QString&)));
    addOrgToPerson->exec();
    addOrgToPerson->deleteLater();
}

void AddContactDialog::on_deleteOrgButton_clicked()
{
    ui->label_org->setText(trUtf8("Нет"));
}

void AddContactDialog::setValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void AddContactDialog::setValuesPopupWindow(QString &number)
{
    ui->FirstNumber->setText(number);
}

