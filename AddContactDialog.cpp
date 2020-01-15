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

AddContactDialog::AddContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddContactDialog)
{
    ui->setupUi(this);
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
    query.prepare("INSERT INTO entry (entry_type, entry_person_lname, entry_person_fname, entry_person_mname, entry_city, entry_address, entry_email, entry_vybor_id)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?)"); // подготавливаем запрос
   /* query.addBindValue("person");
    query.addBindValue(LastName); // заполняем вопросительные знаки значениями
    query.addBindValue(FirstName);
    query.addBindValue(ui->Patronymic);
    query.addBindValue(ui->City);
    query.addBindValue(ui->Address);
    query.addBindValue(ui->Email);
    query.addBindValue(ui->VyborID);
    query.exec();*/
}

