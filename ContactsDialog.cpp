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
    query1->setQuery("SELECT * FROM entry");
    ui->tableView->setModel(query1);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->addButton, &QAbstractButton::clicked, this, &ContactsDialog::onAdd);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &ContactsDialog::onDelete);

    query1->insertColumn(13);
    query1->setHeaderData(13, Qt::Horizontal, tr("Редактирование"));
    for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
    {
        ui->tableView->setIndexWidget(query1->index(i, 13), createEditButton());
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
    QTableView *tableview = qobject_cast<QTableView*>( but->parent()->parent() );
    int k = tableview->currentIndex().row();
    if(k == 4)
    {
        ui->textEdit->insertPlainText("123");
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


