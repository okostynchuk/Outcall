#include "EditContactDialog.h"
#include "ui_EditContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>
#include <QStringList>

EditContactDialog::EditContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditContactDialog)
{
    ui->setupUi(this);

    QRegExp RegExp("^[\\+]?[0-9]{1,12}$");
    validator = new QRegExpValidator(RegExp, this);
    ui->FirstNumber->setValidator(validator);
    ui->SecondNumber->setValidator(validator);
    ui->ThirdNumber->setValidator(validator);
    ui->FourthNumber->setValidator(validator);
    ui->FifthNumber->setValidator(validator);

    QRegExp RegExp2("^[0-9]+$");
    validator2 = new QRegExpValidator(RegExp2, this);
    ui->VyborID->setValidator(validator2);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText("Имя:<span style=\"color: red;\">*</span>");

    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditContactDialog::onSave);
}

EditContactDialog::~EditContactDialog()
{
    delete validator;
    delete validator2;
    delete ui;
}

void EditContactDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString lastName = QString(ui->LastName->text());
    QString firstName = QString(ui->FirstName->text());
    QString patronymic = QString(ui->Patronymic->text());

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_person_org_id = ?, entry_person_lname = ?, entry_person_fname = ?, entry_person_mname = ?, entry_city = ?, entry_address = ?, entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
    query.addBindValue("person");
    if(ui->LastName->text().isEmpty())
    {
        query.addBindValue(firstName + ' ' + patronymic);
    }
    else { query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic); }

    QString orgName = ui->comboBox->currentText();
    if (orgName != "Нет")
    {
        QSqlQuery queryOrg(db);
        QString sqlOrg = QString("SELECT id FROM entry WHERE entry_org_name = '%1'").arg(orgName);
        queryOrg.prepare(sqlOrg);
        queryOrg.exec();
        queryOrg.next();
        query.addBindValue(queryOrg.value(0).toString());
    }
    else
    {
        query.addBindValue(NULL);
    }

    query.addBindValue(lastName);
    query.addBindValue(firstName);
    query.addBindValue(patronymic);
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());
    query.addBindValue(ui->Comment->toPlainText());
    query.addBindValue(updateID);

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
        ui->SecondNumber->setStyleSheet("border: 1px solid grey");
        ui->ThirdNumber->setStyleSheet("border: 1px solid grey");
        ui->FourthNumber->setStyleSheet("border: 1px solid grey");
        ui->FifthNumber->setStyleSheet("border: 1px solid grey");

        numbers.clear();
        QSqlQuery query1(db);
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FirstNumber->text() + "' AND NOT entry_id = " + updateID + ")");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->FirstNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->FirstNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->SecondNumber->text() + "' AND NOT entry_id = " + updateID + ")");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->SecondNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->SecondNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->ThirdNumber->text() + "' AND NOT entry_id = " + updateID + ")");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->ThirdNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->ThirdNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FourthNumber->text() + "' AND NOT entry_id = " + updateID + ")");
        query1.exec();
        query1.next();
        if (query1.value(0) != 0)
        {
            ui->FourthNumber->setStyleSheet("border: 1px solid red");
            numbers << QString(ui->FourthNumber->text());
        }
        query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FifthNumber->text() + "' AND NOT entry_id = " + updateID + ")");
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
            emit sendData(true);
            close();
            QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Запись успешно изменена!"), QMessageBox::Ok);
            destroy(true);
        }
    }
}

void EditContactDialog::onComboBoxSelected()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT entry_person_org_id FROM entry WHERE id = " + updateID);
    query.exec();
    QString orgID = NULL;
    while (query.next())
        orgID = query.value(0).toString();
    query.prepare("SELECT entry_org_name FROM entry WHERE id = " + orgID);
    query.exec();
    QString orgName = NULL;
    while (query.next())
        orgName = query.value(0).toString();
    ui->comboBox->addItem("Нет");
    query.prepare("SELECT entry_org_name FROM entry WHERE entry_org_name IS NOT NULL");
    query.exec();
    while (query.next())
    {
        if (!query.value(0).toString().isEmpty())
        {
            ui->comboBox->addItem(query.value(0).toString());
        }
    }
    if (!orgName.isEmpty())
    {
        ui->comboBox->setCurrentText(orgName);
    }
    else
    {
        ui->comboBox->setCurrentText("Нет");
    }
}

void EditContactDialog::setValuesContacts(QString &i)
{
    updateID = i;
    QSqlDatabase db;
    QSqlQuery query(db);
    QString sql = QString("select entry_phone from entry_phone where entry_id = %1").arg(updateID);
    query.prepare(sql);
    query.exec();
    int count = 1;
    while (query.next())
    {
        if (count == 1)
            firstNumber = query.value(0).toString();
        else if (count == 2)
            secondNumber = query.value(0).toString();
        else if (count == 3)
            thirdNumber = query.value(0).toString();
        else if (count == 4)
            fourthNumber = query.value(0).toString();
        else if (count == 5)
            fifthNumber = query.value(0).toString();
        count++;
    }
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
    onComboBoxSelected();
}

void EditContactDialog::setValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void EditContactDialog::setValuesPopupWindow(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + number + "')");
    query.exec();
    query.next();
    if (query.value(0) != 0)
    {
        ui->FirstNumber->setText(number);
    }
}
