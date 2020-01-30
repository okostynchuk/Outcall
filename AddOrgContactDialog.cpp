#include "AddOrgContactDialog.h"
#include "ui_AddOrgContactDialog.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>

AddOrgContactDialog::AddOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOrgContactDialog)
{
    ui->setupUi(this);
    QRegExp RegExp("^[\\+]?[0-9]{1,12}$");
    validator = new QRegExpValidator(RegExp, this);
    ui->FirstNumber->setValidator(validator);
    ui->SecondNumber->setValidator(validator);
    ui->ThirdNumber->setValidator(validator);
    ui->FourthNumber->setValidator(validator);
    ui->FifthNumber->setValidator(validator);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText("Имя<span style=\"color: red;\">*</span>");

    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddOrgContactDialog::onSave);
}

AddOrgContactDialog::~AddOrgContactDialog()
{
    delete validator;
    delete ui;
}

void AddOrgContactDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString orgName = QString(ui->OrgName->text());

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

    QSqlQuery query1(db);
    QString sql1 = QString("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '%1' OR entry_phone = '%2' OR entry_phone = '%3' OR entry_phone = '%4' OR entry_phone = '%5')")
            .arg(ui->FirstNumber->text(),
            ui->SecondNumber->text(),
            ui->ThirdNumber->text(),
            ui->FourthNumber->text(),
            ui->FifthNumber->text());
    query1.prepare(sql1);
    query1.exec();
    query1.next();

    if (QString(ui->OrgName->text()).isEmpty() == true)
    {
         ui->label_15->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
         ui->OrgName->setStyleSheet("border: 1px solid red");
    }
    else { ui->label_15->hide();  ui->OrgName->setStyleSheet("border: 1px solid grey"); }
    if (QString(ui->FirstNumber->text()).isEmpty() == true)
    {
        ui->label_14->setText("<span style=\"color: red;\">Заполните обязательное поле!</span>");
        ui->FirstNumber->setStyleSheet("border: 1px solid red");
    }
    else { ui->label_14->hide(); ui->FirstNumber->setStyleSheet("border: 1px solid grey"); }
    if (QString(ui->OrgName->text()).isEmpty() == false && QString(ui->FirstNumber->text()).isEmpty() == false)
    {
        ui->label_15->hide();
        ui->OrgName->setStyleSheet("border: 1px solid grey");
        ui->label_14->hide();
        ui->FirstNumber->setStyleSheet("border: 1px solid grey");
        if (query1.value(0) != 0){
            QMessageBox::information(this, trUtf8("Error"), trUtf8("Record is exists"));
        }
        else
        {
            query.exec();
            qint32 id = query.lastInsertId().toInt();
            QString firstNum = QString(ui->FirstNumber->text());
            QString secondNum = QString(ui->SecondNumber->text());
            QString thirdNum = QString(ui->ThirdNumber->text());
            QString fourthNum = QString(ui->FourthNumber->text());
            QString fifthNum = QString(ui->FifthNumber->text());
            if (firstNum != 0)
            {
                query1.prepare("INSERT INTO phone (entry_id, phone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->FirstNumber->text());
                query1.exec();
            }
            if (secondNum != 0)
            {
                query1.prepare("INSERT INTO phone (entry_id, phone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->SecondNumber->text());
                query1.exec();
            }
            if (thirdNum != 0)
            {
                query1.prepare("INSERT INTO phone (entry_id, phone)"
                               "VALUES(?, ?)");
                query1.addBindValue(id);
                query1.addBindValue(ui->ThirdNumber->text());
                query1.exec();
            }
            if (fourthNum != 0)
            {
                    query1.prepare("INSERT INTO phone (entry_id, phone)"
                                   "VALUES(?, ?)");
                    query1.addBindValue(id);
                    query1.addBindValue(ui->FourthNumber->text());
                    query1.exec();
            }

            if (fifthNum != 0)
            {
                    query1.prepare("INSERT INTO phone (entry_id, phone)"
                                   "VALUES(?, ?)");
                    query1.addBindValue(id);
                    query1.addBindValue(ui->FifthNumber->text());
                    query1.exec();
            }
            ui->label_16->setText("<span style=\"color: green;\">Запись успешно добавлена!</span>");
            emit sendData(true);
        }
    }
}

void AddOrgContactDialog::setOrgValuesCallHistory(QString &)
{

}
