#include "AddContactDialog.h"
#include "ui_AddContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>
#include <QLineEdit>
#include <QStringList>

AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText(tr("Имя:<span style=\"color: red;\">*</span>"));
    ui->label_org->setText(tr("Нет"));

    connect(ui->saveButton, &QPushButton::clicked, this, &AddContactDialog::onSave);
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
    else
    {
        ui->label_15->setText(tr(""));

        ui->FirstName->setStyleSheet("border: 1px solid grey");
    }

    if (QString(ui->FirstNumber->text()).isEmpty() == true)
    {
        ui->label_14->show();
        ui->label_14->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

        ui->FirstNumber->setStyleSheet("border: 1px solid red");
    }
    else
    {
        ui->label_14->setText(tr(""));

        ui->FirstNumber->setStyleSheet("border: 1px solid grey");
    }

    if (QString(ui->FirstName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->setText(tr(""));
        ui->label_14->setText(tr(""));

        ui->FirstName->setStyleSheet("border: 1px solid grey");
        ui->FirstNumber->setStyleSheet("border: 1px solid grey");
        ui->SecondNumber->setStyleSheet("border: 1px solid grey");
        ui->ThirdNumber->setStyleSheet("border: 1px solid grey");
        ui->FourthNumber->setStyleSheet("border: 1px solid grey");
        ui->FifthNumber->setStyleSheet("border: 1px solid grey");
        ui->VyborID->setStyleSheet("border: 1px solid grey");

        int count_invalid_num = 0;

        QSqlQuery query1(db);

        if (!ui->FirstNumber->text().isEmpty())
        {
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FirstNumber->text() + "')");
            query1.exec();
            query1.next();

            if (query1.value(0) != 0)
            {
                ui->FirstNumber->setStyleSheet("border: 1px solid red");

                count_invalid_num++;
            }
        }
        if (!ui->SecondNumber->text().isEmpty())
        {
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->SecondNumber->text() + "')");
            query1.exec();
            query1.next();

            if (query1.value(0) != 0)
            {
                ui->SecondNumber->setStyleSheet("border: 1px solid red");

                count_invalid_num++;
            }
        }
        if (!ui->ThirdNumber->text().isEmpty())
        {
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->ThirdNumber->text() + "')");
            query1.exec();
            query1.next();

            if (query1.value(0) != 0)
            {
                ui->ThirdNumber->setStyleSheet("border: 1px solid red");

                count_invalid_num++;
            }
        }
        if (!ui->FourthNumber->text().isEmpty())
        {
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FourthNumber->text() + "')");
            query1.exec();
            query1.next();

            if (query1.value(0) != 0)
            {
                ui->FourthNumber->setStyleSheet("border: 1px solid red");

                count_invalid_num++;
            }
        }
        if (!ui->FifthNumber->text().isEmpty())
        {
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FifthNumber->text() + "')");
            query1.exec();
            query1.next();

            if (query1.value(0) != 0)
            {
                ui->FifthNumber->setStyleSheet("border: 1px solid red");

                count_invalid_num++;
            }
        }

        if (count_invalid_num != 0)
            QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Введены существующие номера!"), QMessageBox::Ok);
        else
        {
            QString firstNum = QString(ui->FirstNumber->text());
            QString secondNum = QString(ui->SecondNumber->text());
            QString thirdNum = QString(ui->ThirdNumber->text());
            QString fourthNum = QString(ui->FourthNumber->text());
            QString fifthNum = QString(ui->FifthNumber->text());
            QString vyborId = QString(ui->VyborID->text());

            bool validPhones = true;

            if (vyborId != 0)
            {
                if (isVyborID(&vyborId) == true)
                    ui->VyborID->setStyleSheet("border: 1px solid grey");
                else
                {
                    ui->VyborID->setStyleSheet("border: 1px solid red");

                    QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("VyborID не соответствует формату!"), QMessageBox::Ok);

                    return;
                }
            }

            if (firstNum != 0)
            {
                if (isPhone(&firstNum))
                    ui->FirstNumber->setStyleSheet("border: 1px solid grey");
                else
                {
                    validPhones = false;

                    ui->FirstNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (secondNum != 0)
            {
                if (isPhone(&secondNum))
                    ui->SecondNumber->setStyleSheet("border: 1px solid grey");
                else
                {
                    validPhones = false;

                    ui->SecondNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (thirdNum != 0)
            {
                if (isPhone(&thirdNum))
                    ui->ThirdNumber->setStyleSheet("border: 1px solid grey");
                else
                {
                    validPhones = false;

                    ui->ThirdNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (fourthNum != 0)
            {
                if (isPhone(&fourthNum))
                    ui->FourthNumber->setStyleSheet("border: 1px solid grey");
                else
                {
                    validPhones = false;

                    ui->FourthNumber->setStyleSheet("border: 1px solid red");
                }
            }
            if (fifthNum != 0)
            {
                if (isPhone(&fifthNum))
                    ui->FifthNumber->setStyleSheet("border: 1px solid grey");
                else
                {
                    validPhones = false;

                    ui->FifthNumber->setStyleSheet("border: 1px solid red");
                }
            }         

            if (!validPhones)
                QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Номер не соответствует формату!"), QMessageBox::Ok);
            else
            {
                query.prepare("INSERT INTO entry (entry_type, entry_name, entry_person_org_id, entry_person_lname, entry_person_fname, entry_person_mname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment)"
                              "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                query.addBindValue("person");

                if (ui->LastName->text().isEmpty())
                    query.addBindValue(firstName + ' ' + patronymic);
                else
                    query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic);

                QString orgName = ui->label_org->text();

                if (orgName != tr("Нет"))
                {
                    QSqlQuery queryOrg(db);

                    QString sqlOrg = QString("SELECT id FROM entry WHERE entry_org_name = '%1'").arg(orgName);

                    queryOrg.prepare(sqlOrg);
                    queryOrg.exec();

                    if (query.next())
                        query.addBindValue(queryOrg.value(0).toString());
                    else
                    {
                        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Организации не существует или она была изменена!"), QMessageBox::Ok);

                        return;
                    }
                }
                else
                    query.addBindValue(QVariant(QVariant::Int));

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

                QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Запись успешно добавлена!"), QMessageBox::Ok);

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

bool AddContactDialog::isVyborID(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[0-9]*"));

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
    addOrgToPerson->show();
    addOrgToPerson->setAttribute(Qt::WA_DeleteOnClose);
}

void AddContactDialog::on_deleteOrgButton_clicked()
{
    ui->label_org->setText(tr("Нет"));
}

void AddContactDialog::setValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void AddContactDialog::setValuesPopupWindow(QString &number)
{
    ui->FirstNumber->setText(number);
}

