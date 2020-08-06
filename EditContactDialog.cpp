#include "EditContactDialog.h"
#include "ui_EditContactDialog.h"

#include "ViewContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QDesktopWidget>

EditContactDialog::EditContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText(tr("Имя:<span style=\"color: red;\">*</span>"));

    connect(ui->backButton, &QPushButton::clicked, this, &EditContactDialog::onReturn);
    connect(ui->saveButton, &QPushButton::clicked, this, &EditContactDialog::onSave);
}

EditContactDialog::~EditContactDialog()
{
    delete ui;
}

void EditContactDialog::onReturn()
{    
    emit sendData(false, this->pos().x(), this->pos().y());

    close();
}

void EditContactDialog::setPos(int x, int y)
{
    int nDesktopHeight;
    int nDesktopWidth;
    int nWidgetHeight = QWidget::height();
    int nWidgetWidth = QWidget::width();

    QDesktopWidget desktop;
    QRect rcDesktop = desktop.availableGeometry(this);

    nDesktopWidth = rcDesktop.width();
    nDesktopHeight = rcDesktop.height();

    if (x < 0 && (nDesktopHeight - y) > nWidgetHeight)
    {
        x = 0;
        this->move(x, y);
    }
    else if (x < 0 && ((nDesktopHeight - y) < nWidgetHeight))
    {
        x = 0;
        y = nWidgetHeight;
        this->move(x, y);
    }
    else if ((nDesktopWidth - x) < nWidgetWidth && (nDesktopHeight - y) > nWidgetHeight)
    {
        x = nWidgetWidth;
        this->move(x, y);
    }
    else if ((nDesktopWidth - x) < nWidgetWidth && ((nDesktopHeight - y) < nWidgetHeight))
    {
        x = nWidgetWidth;
        y = nWidgetHeight;
        this->move(x, y);
    }
    else if (x > 0 && ((nDesktopHeight - y) < nWidgetHeight))
    {
        y = nWidgetHeight;
        this->move(x, y);
    }
    else
    {
        this->move(x, y);
    }
}

void EditContactDialog::onSave()
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
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FirstNumber->text() + "' AND NOT entry_id = " + updateID + ")");
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
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->SecondNumber->text() + "' AND NOT entry_id = " + updateID + ")");
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
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->ThirdNumber->text() + "' AND NOT entry_id = " + updateID + ")");
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
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FourthNumber->text() + "' AND NOT entry_id = " + updateID + ")");
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
            query1.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + ui->FifthNumber->text() + "' AND NOT entry_id = " + updateID + ")");
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
            query.exec();

            QString firstNum = QString(ui->FirstNumber->text());
            QString secondNum = QString(ui->SecondNumber->text());
            QString thirdNum = QString(ui->ThirdNumber->text());
            QString fourthNum = QString(ui->FourthNumber->text());
            QString fifthNum = QString(ui->FifthNumber->text());

            QString sql1 = QString("select COUNT(fone) from fones where entry_id = %1").arg(updateID);

            query1.prepare(sql1);
            query1.exec();
            query1.next();

            int count = query1.value(0).toInt();

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
                query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_person_org_id = ?, entry_person_lname = ?, entry_person_fname = ?, entry_person_mname = ?, entry_city = ?, entry_address = ?, entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
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

                    if (queryOrg.next())
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
                query.addBindValue(updateID);
                query.exec();

                if (count > 0)
                {
                    query1.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                    query1.addBindValue(firstNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(firstNumber);
                    query1.exec();

                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO fones (entry_id, fone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(firstNum);
                    query1.exec();
                }

                if (count > 0)
                {
                    query1.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                    query1.addBindValue(secondNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(secondNumber);
                    query1.exec();

                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO fones (entry_id, fone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(secondNum);
                    query1.exec();
                }

                if (count > 0)
                {
                    query1.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                    query1.addBindValue(thirdNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(thirdNumber);
                    query1.exec();

                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO fones (entry_id, fone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(thirdNum);
                    query1.exec();
                }

                if (count > 0)
                {
                    query1.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                    query1.addBindValue(fourthNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(fourthNumber);
                    query1.exec();

                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO fones (entry_id, fone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(fourthNum);
                    query1.exec();
                }

                if (count > 0)
                {
                    query1.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                    query1.addBindValue(fifthNum);
                    query1.addBindValue(updateID);
                    query1.addBindValue(fifthNumber);
                    query1.exec();

                    count--;
                }
                else
                {
                    query1.prepare("INSERT INTO fones (entry_id, fone) VALUES(?, ?)");
                    query1.addBindValue(updateID);
                    query1.addBindValue(fifthNum);
                    query1.exec();
                }

                emit sendData(true, this->pos().x(), this->pos().y());

                close();

                QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Запись успешно изменена!"), QMessageBox::Ok);
            }
        }
    }
}

bool EditContactDialog::isPhone(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[\\+]?[0-9]{1,12}"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool EditContactDialog::isVyborID(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[0-9]*"));

    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void EditContactDialog::setValuesContacts(QString &i)
{
    updateID = i;

    QSqlDatabase db;
    QSqlQuery query(db);

    QString sql = QString("SELECT entry_phone FROM entry_phone WHERE entry_id = %1").arg(updateID);

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

    sql = QString("SELECT distinct entry_person_fname, entry_person_mname, entry_person_lname, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = %1").arg(updateID);

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

    query.prepare("SELECT entry_person_org_id FROM entry_phone WHERE entry_id = " + updateID);
    query.exec();

    QString orgID = NULL;

    while (query.next())
        orgID = query.value(0).toString();

    query.prepare("SELECT entry_name FROM entry_phone WHERE entry_id = " + orgID);
    query.exec();

    QString orgName = NULL;

    while (query.next())
        orgName = query.value(0).toString();

    if (!orgName.isEmpty() && !orgName.isNull())
        ui->label_org->setText(orgName);
    else
        ui->label_org->setText(tr("Нет"));

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
}

void EditContactDialog::receiveOrgID(QString &id)
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

void EditContactDialog::on_addOrgButton_clicked()
{
    addOrgToPerson = new AddOrgToPerson;
    connect(addOrgToPerson, SIGNAL(sendOrgID(QString&)), this, SLOT(receiveOrgID(QString&)));
    addOrgToPerson->show();
    addOrgToPerson->setAttribute(Qt::WA_DeleteOnClose);
}

void EditContactDialog::on_deleteOrgButton_clicked()
{
    ui->label_org->setText(tr("Нет"));
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
        ui->FirstNumber->setText(number);
}
