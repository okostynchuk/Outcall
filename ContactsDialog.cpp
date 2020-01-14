#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);
    /*QSqlQueryModel *setquery1 = new QSqlQueryModel;
    setquery1->setQuery("SELECT * FROM entry");
    QTableView *tv = new QTableView(this);
    tv->setModel(setquery1);
    ui->tableView->setModel(setquery1);*/

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->addButton, &QAbstractButton::clicked, this, &ContactsDialog::onAdd);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &ContactsDialog::onDelete);
}

ContactsDialog::~ContactsDialog()
{
    delete ui;
}

void ContactsDialog::onAdd()
{
    delete ui;
}

void ContactsDialog::onDelete()
{
    delete ui;
}


