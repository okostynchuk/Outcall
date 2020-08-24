#include "AddOrgContactDialog.h"
#include "ui_AddOrgContactDialog.h"

#include <QMessageBox>

AddOrgContactDialog::AddOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOrgContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText(tr("Название организации:<span style=\"color: red;\">*</span>"));

    connect(ui->saveButton, &QPushButton::clicked, this, &AddOrgContactDialog::onSave);
}

AddOrgContactDialog::~AddOrgContactDialog()
{
    delete ui;
}

void AddOrgContactDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    QString orgName = QString(ui->OrgName->text());

    phonesList = { ui->FirstNumber, ui->SecondNumber, ui->ThirdNumber, ui->FourthNumber, ui->FifthNumber };

    if (QString(ui->OrgName->text()).isEmpty())
    {
         ui->label_15->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

         ui->OrgName->setStyleSheet("border: 1px solid red");

         return;
    }
    else
    {
        ui->label_15->setText(tr(""));

        ui->OrgName->setStyleSheet("border: 1px solid grey");
    }

    if (phonesList.at(0)->text().isEmpty())
    {
        ui->label_14->show();
        ui->label_14->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

        phonesList.at(0)->setStyleSheet("border: 1px solid red");

        return;
    }
    else
    {
        ui->label_14->setText(tr(""));

        phonesList.at(0)->setStyleSheet("border: 1px solid grey");
    }

    for (int i = 0; i < phonesList.length(); ++i)
    {
        if (!phonesList.at(i)->text().isEmpty())
        {
            QString phone = QString(phonesList.at(i)->text());

            if (isPhone(&phone) && !isInnerPhone(&phone))
                phonesList.at(i)->setStyleSheet("border: 1px solid grey");
            else
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");

                QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Номер не соответствует формату!"), QMessageBox::Ok);

                return;
            }
        }
    }

    if (!QString(ui->OrgName->text()).isEmpty() && !QString(phonesList.at(0)->text()).isEmpty())
    {
        ui->label_15->setText(tr(""));
        ui->label_14->setText(tr(""));

        ui->OrgName->setStyleSheet("border: 1px solid grey");


        for(int i = 0; i < phonesList.length(); ++i)
            phonesList.at(i)->setStyleSheet("border: 1px solid grey");
    }

    for (int i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + phonesList.at(i)->text() + "')");
            query.exec();
            query.next();

            if (query.value(0) != 0)
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");

                QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Введены существующие номера!"), QMessageBox::Ok);

                return;
            }
        }

    QString vyborId = QString(ui->VyborID->text());

    if (!vyborId.isEmpty())
    {
        if (isVyborID(&vyborId))
            ui->VyborID->setStyleSheet("border: 1px solid grey");
        else
        {
            ui->VyborID->setStyleSheet("border: 1px solid red");

            QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("VyborID не соответствует формату!"), QMessageBox::Ok);

            return;
        }
    }

    query.prepare("INSERT INTO entry (entry_type, entry_name, entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue("org");
    query.addBindValue(orgName);
    query.addBindValue(orgName);
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());
    query.addBindValue(ui->Comment->toPlainText());
    query.exec();

    int id = query.lastInsertId().toInt();

    for (int i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            query.prepare("INSERT INTO fones (entry_id, fone)"
                           "VALUES(?, ?)");
            query.addBindValue(id);
            query.addBindValue(phonesList.at(i)->text());
            query.exec();
        }

    emit sendData(true);

    close();

    QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Запись успешно добавлена!"), QMessageBox::Ok);

}


bool AddOrgContactDialog::isInnerPhone(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("[0-9]{4}"));
    QRegularExpressionValidator validator2(QRegularExpression("[2][0-9]{2}"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool AddOrgContactDialog::isPhone(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[\\+]?[0-9]{1,12}"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool AddOrgContactDialog::isVyborID(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[0-9]*"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void AddOrgContactDialog::setOrgValuesCallHistory(QString &number)
{
    ui->FirstNumber->setText(number);
}

void AddOrgContactDialog::setOrgValuesPopupWindow(QString &number)
{
    ui->FirstNumber->setText(number);
}
