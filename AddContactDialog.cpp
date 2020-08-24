#include "AddContactDialog.h"
#include "ui_AddContactDialog.h"

#include <QMessageBox>

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

    phonesList = { ui->FirstNumber, ui->SecondNumber, ui->ThirdNumber, ui->FourthNumber, ui->FifthNumber };

    if (QString(ui->FirstName->text()).isEmpty())
    {
         ui->label_15->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

         ui->FirstName->setStyleSheet("border: 1px solid red");\

         return;
    }
    else
    {
        ui->label_15->setText(tr(""));

        ui->FirstName->setStyleSheet("border: 1px solid grey");
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

    if (!QString(ui->FirstName->text()).isEmpty() && !QString(phonesList.at(0)->text()).isEmpty())
    {
        ui->label_15->setText(tr(""));
        ui->label_14->setText(tr(""));

        ui->FirstName->setStyleSheet("border: 1px solid grey");

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
        if (isVyborID(&vyborId))
            ui->VyborID->setStyleSheet("border: 1px solid grey");
        else
        {
            ui->VyborID->setStyleSheet("border: 1px solid red");

            QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("VyborID не соответствует формату!"), QMessageBox::Ok);

            return;
        }

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

        if (queryOrg.next())
            query.addBindValue(queryOrg.value(0).toInt());
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


bool AddContactDialog::isInnerPhone(QString *str)
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

bool AddContactDialog::isPhone(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[\\+]?[0-9]{1,12}"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool AddContactDialog::isVyborID(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[0-9]*"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
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

void AddContactDialog::setValues(QString &number)
{
    ui->FirstNumber->setText(number);
}

