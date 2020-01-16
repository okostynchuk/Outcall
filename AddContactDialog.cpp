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
    QRegExp RegExp("^[0][0-9]{9}$");
    QValidator *Validator = new QRegExpValidator(RegExp, this);
    ui->FirstNumber->setValidator(Validator);
    ui->SecondNumber->setValidator(Validator);
    ui->ThirdNumber->setValidator(Validator);


    QRegExp RegExp2("^[A-Z][A-Za-z]+$");
    QValidator *ValidatorForName = new QRegExpValidator(RegExp2, this);
    ui->FirstName->setValidator(ValidatorForName);
    ui->LastName->setValidator(ValidatorForName);
    ui->Patronymic->setValidator(ValidatorForName);

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
    QSqlDatabase db;
    QSqlQuery query(db);
    QString lastname = QString(ui->LastName->text());
    QString firstname = QString(ui->FirstName->text());
    QString patronymic = QString(ui->Patronymic->text());
//    if(lastname==0)
//    {
//        m_pMyWidget = new QLineEdit(ui->FirstName);

//        QPalette Pal(palette());

//        // устанавливаем цвет фона
//        Pal.setColor(QPalette::Background, Qt::red);
//        m_pMyWidget->setAutoFillBackground(true);
//        m_pMyWidget->setPalette(Pal);
//        m_pMyWidget->show();
//    }

    query.prepare("INSERT INTO entry (entry_type, entry_name,entry_person_lname, entry_person_fname, entry_person_mname, entry_city, entry_address, entry_email, entry_vybor_id)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue("person");
    query.addBindValue(lastname + ' ' + firstname + ' '     + patronymic);
    query.addBindValue(lastname);
    query.addBindValue(firstname);
    query.addBindValue(patronymic);
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());


    QSqlQuery query1(db);
    QString sql1 = QString("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '%1' OR entry_phone = '%2' OR entry_phone = '%3');")
            .arg(ui->FirstNumber->text(),
            ui->SecondNumber->text(),
            ui->ThirdNumber->text());
    query1.prepare(sql1);
    query1.exec();
    query1.next();

    QSqlQuery query2(db);
    QSqlQuery query3(db);
    QSqlQuery query4(db);

    /* Если запись существует, то вызывается
     * информационное сообщение
     * */
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
            query3.prepare("INSERT INTO phone (entry_id, phone)"
                           "VALUES(?, ?)");
            query3.addBindValue(id);
            query3.addBindValue(ui->SecondNumber->text());
            query3.exec();
        }
        if(thirdnum!=0)
        {
            query4.prepare("INSERT INTO phone (entry_id, phone)"
                           "VALUES(?, ?)");
            query4.addBindValue(id);
            query4.addBindValue(ui->ThirdNumber->text());
            query4.exec();
        }
        ui->textEdit->append("Запись добавлена");
    }
}

