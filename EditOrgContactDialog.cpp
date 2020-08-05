#include "EditOrgContactDialog.h"
#include "ui_EditOrgContactDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopWidget>

EditOrgContactDialog::EditOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditOrgContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->label_6->setText(tr("1<span style=\"color: red;\">*</span>"));
    ui->label_3->setText(tr("Название организации:<span style=\"color: red;\">*</span>"));

    connect(ui->backButton, &QPushButton::clicked, this, &EditOrgContactDialog::onReturn);
    connect(ui->saveButton, &QPushButton::clicked, this, &EditOrgContactDialog::onSave);
}

EditOrgContactDialog::~EditOrgContactDialog()
{
    delete ui;
}

void EditOrgContactDialog::onReturn()
{
    emit sendData(false, this->pos().x(), this->pos().y());

    close();
}

void EditOrgContactDialog::setPos(int x, int y)
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
    else if ((nDesktopWidth - x) < nWidgetWidth && (nDesktopHeight-y) > nWidgetHeight)
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

void EditOrgContactDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    QString orgName = QString(ui->OrgName->text());

    if (QString(ui->OrgName->text()).isEmpty() == true)
    {
         ui->label_15->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

         ui->OrgName->setStyleSheet("border: 1px solid red");
    }
    else
    {
        ui->label_15->setText(tr(""));

        ui->OrgName->setStyleSheet("border: 1px solid grey");
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

    if (QString(ui->OrgName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->setText(tr(""));
        ui->label_14->setText(tr(""));

        ui->OrgName->setStyleSheet("border: 1px solid grey");
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

            QString sql1 = QString("SELECT COUNT(fone) FROM fones WHERE entry_id = %1").arg(updateID);

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
                query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_org_name = ?, entry_city = ?, entry_address = ?, entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
                query.addBindValue("org");
                query.addBindValue(orgName);
                query.addBindValue(orgName);
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

bool EditOrgContactDialog::isPhone(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[\\+]?[0-9]{1,12}"));

    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool EditOrgContactDialog::isVyborID(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[0-9]*"));

    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void EditOrgContactDialog::setOrgValuesContacts(QString &i)
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

    sql = QString("SELECT distinct entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = %1").arg(updateID);

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

void EditOrgContactDialog::setOrgValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void EditOrgContactDialog::setOrgValuesPopupWindow(QString &number)
{
    ui->FirstNumber->setText(number);
}
