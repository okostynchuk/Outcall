#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QBoxLayout>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);
    query1 = new QSqlQueryModel;
    query1->setQuery("SELECT id, entry_type, entry_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry");
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Тип"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Город"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Адрес"));
    query1->setHeaderData(5, Qt::Horizontal, QObject::tr("Email"));
    query1->setHeaderData(6, Qt::Horizontal, QObject::tr("VyborID"));
    query1->setHeaderData(7, Qt::Horizontal, QObject::tr("Заметка"));
    query1->insertColumn(8);
    query1->setHeaderData(8, Qt::Horizontal, tr("Редактирование"));
    ui->tableView->setModel(query1);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->addButton, &QAbstractButton::clicked, this, &ContactsDialog::onAdd);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &ContactsDialog::onDelete);


    for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
    {
        ui->tableView->setIndexWidget(query1->index(i, 8), createEditButton());
    }
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
}

ContactsDialog::~ContactsDialog()
{
    delete ui;
}

void ContactsDialog::onEdit()
{
    ui->textEdit->append("123");
    /*QWidget *but = qobject_cast<QWidget *>(sender());
    QTableView *tableview = qobject_cast<QTableView*>(but->parent()->parent());
    int k = tableview->currentIndex().row();
    if(k == 4)
    {
        ui->textEdit->append("123");
    }*/

}

void ContactsDialog::onAdd()
{

}

void ContactsDialog::onDelete()
{

}

QWidget* ContactsDialog::createEditButton() const
{
    QWidget* wgt = new QWidget;
    QBoxLayout* l = new QHBoxLayout;
    QPushButton* editButton = new QPushButton("Редактировать");
    connect(editButton, SIGNAL(clicked(bool)), SLOT(onEdit()));
    l->addWidget(editButton);
    wgt->setLayout(l);
    return wgt;
}


