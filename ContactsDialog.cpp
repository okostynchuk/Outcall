#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QSqlQueryModel>
#include <QTableView>
#include <QBoxLayout>
#include <QClipboard>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QGuiApplication>
#include <QScreen>
#include <QLabel>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);

    this->showMaximized();
    ui->widget->move(0, 0);
    ui->widget->resize(QGuiApplication::screens().at(0)->geometry().width(), QGuiApplication::screens().at(0)->geometry().height());

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;
    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
    query2->setQuery("SELECT DISTINCT entry_type FROM entry_phone");
    //ui->tableView->horizontalHeader()->resizeColumnsToContents();
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, tr("Тип"));
    //query1->setHeaderData(1, Qt::Horizontal, QObject::tr("Тип"));
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

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->addButton, &QAbstractButton::clicked, this, &ContactsDialog::onAdd);
    connect(ui->deleteButton, &QAbstractButton::clicked, this, &ContactsDialog::onDelete);
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));


    for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
    {
        ui->tableView->setIndexWidget(query1->index(i, 1), addImageLabel());
        ui->tableView->setIndexWidget(query1->index(i, 9), createEditButton());
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
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

QWidget* ContactsDialog::addImageLabel() const
{
    QWidget* wgt = new QWidget;
    QBoxLayout* l = new QHBoxLayout;
    QLabel *imageLabel = new QLabel(wgt);
    l->addWidget(imageLabel);
    //if(query2->index(i, 1))
    imageLabel->setPixmap(QPixmap("D:/org.png").scaled(30, 30, Qt::KeepAspectRatio));
    wgt->setLayout(l);
    return wgt;
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

void ContactsDialog::on_lineEdit_returnPressed()
{
    //    QSqlDatabase db;
    //    QSqlTableModel query(db);

    //    query.setFilter("SELECT entry_type = 'org' FROM entry");

//    QSqlTableModel model;
//    model.setTable("entry");
//    model.setFilter("entry_city = 'Киев'");
//    //QString entry_name = ui->lineEdit->text();
//    //if(model.setFilter("entry_name") == entry_name){
//        model.select();
//        query1 = new QSqlQueryModel;
//        ui->tableView->setModel(query1);
//        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

   // }
    //private: char entry_name;
    //model.setFilter("entry_city = 'Киев'");
    //model.setFilter("entry_vybor_id = 4978");
    //model.setSort(2, Qt::DescendingOrder);

//    for (int i = 0; i < model.rowCount(); ++i) {
//         QString entry_name = model.record(i).value("entry_name").toString();
//         QString entry_city = model.record(i).value("entry_city").toString();
//         int id = model.record(i).value("entry_vybor_id").toInt();
//         qDebug() << entry_name << id << entry_city;
//        }
}
