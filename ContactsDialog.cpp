#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QBoxLayout>
#include <QClipboard>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);
    this->showMaximized();

    ui->widget->showMaximized();
    ui->tableView->showMaximized();
    ui->widget->setMinimumSize(1200,950);

    query1 = new QSqlQueryModel;
    query1->setQuery("SELECT ep.entry_id, ep.entry_type, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Тип"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Город"));
    query1->setHeaderData(5, Qt::Horizontal, QObject::tr("Адрес"));
    query1->setHeaderData(6, Qt::Horizontal, QObject::tr("Email"));
    query1->setHeaderData(7, Qt::Horizontal, QObject::tr("VyborID"));
    query1->setHeaderData(8, Qt::Horizontal, QObject::tr("Заметка"));
    query1->insertColumn(9);
    query1->setHeaderData(9, Qt::Horizontal, tr("Редактирование"));
    ui->tableView->setModel(query1);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->addButton, &QAbstractButton::clicked, this, &ContactsDialog::onAdd);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &ContactsDialog::onDelete);
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));


    for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
    {
        ui->tableView->setIndexWidget(query1->index(i, 9), createEditButton());
    }
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
}

ContactsDialog::~ContactsDialog()
{
    delete ui;
}

void ContactsDialog::onTableClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString cellText = index.data().toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(cellText);
    }
}

void ContactsDialog::onEdit()
{
    //ui->textEdit->append("123");
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
