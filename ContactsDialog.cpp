#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "Global.h"

#include <QSqlQueryModel>
#include <QHeaderView>
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
#include <QAbstractProxyModel>

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
    query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, tr("Тип"));
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
    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->addOrgButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton, &QAbstractButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));

    for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
    {
        ui->tableView->setIndexWidget(query1->index(i, 1), addImageLabel(i));
        ui->tableView->setIndexWidget(query1->index(i, 9), createEditButton(i));
    }

    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
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

void ContactsDialog::onUpdate()
{
    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
    query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
    query1->insertColumn(1);
    query1->setHeaderData(1, Qt::Horizontal, tr("Тип"));
    query1->insertColumn(9);
    query1->setHeaderData(9, Qt::Horizontal, tr("Редактирование"));
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(query1);
    for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
    {
        ui->tableView->setIndexWidget(query1->index(i, 1), addImageLabel(i));
        ui->tableView->setIndexWidget(query1->index(i, 9), createEditButton(i));
    }
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
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
    int i = sender()->property("i").toInt();
    query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
    if(query2->data(query2->index(i, 0)).toString() == "person")
    {
        g_Switch = "updatePerson";
        AddContactDialog *addContactDialog = new AddContactDialog;
        addContactDialog->setWindowTitle("Редактирование физ. лица");
        //query2->setQuery("");
        addContactDialog->exec();
    }
    else
    {
        g_Switch = "updateOrg";
        AddOrgContactDialog *addOrgContactDialog = new AddOrgContactDialog;
        addOrgContactDialog->setWindowTitle("Редактирование организации");
        addOrgContactDialog->exec();
    }
}

void ContactsDialog::onAddPerson()
{
    g_Switch = "addPerson";
    AddContactDialog *addContactDialog = new AddContactDialog;
    addContactDialog->setWindowTitle("Добавление физ. лица");
    addContactDialog->exec();
}

void ContactsDialog::onAddOrg()
{
    g_Switch = "addOrg";
    AddOrgContactDialog *addOrgContactDialog = new AddOrgContactDialog;
    addOrgContactDialog->setWindowTitle("Добавление организации");
    addOrgContactDialog->exec();
}

QWidget* ContactsDialog::addImageLabel(int &i) const
{
    QWidget* wgt = new QWidget;
    QBoxLayout* l = new QHBoxLayout;
    QLabel *imageLabel = new QLabel(wgt);
    l->addWidget(imageLabel);
    if(query2->data(query2->index(i, 0)).toString() == "person")
    {
        imageLabel->setPixmap(QPixmap("D:/person.png").scaled(30, 30, Qt::KeepAspectRatio));
    }
    else
    {
        imageLabel->setPixmap(QPixmap("D:/org.png").scaled(30, 30, Qt::KeepAspectRatio));
    }
    wgt->setLayout(l);
    return wgt;
}

QWidget* ContactsDialog::createEditButton(int &i) const
{
    QWidget* wgt = new QWidget;
    QBoxLayout* l = new QHBoxLayout;
    QPushButton* editButton = new QPushButton("Редактировать");
    connect(editButton, SIGNAL(clicked(bool)), SLOT(onEdit()));
    editButton->setProperty("i", i);
    l->addWidget(editButton);
    wgt->setLayout(l);
    return wgt;
}

void ContactsDialog::on_lineEdit_returnPressed()
{
//    QSqlTableModel model;
//    query1 = new QSqlQueryModel;
//    //model = new QSqlQueryModel;
//    //model.setTable("entry_phone");
//    QString entry_city = ui->lineEdit->text();
//    QString entry_phone = ui->lineEdit->text();
//    model.setFilter(QString("entry_city = %1"));



//    //model.setFilter(QString("entry_city = "));



//    if(model.setFilter(tr("entry_city '%1' or "
//                               "entry_name '%2'")
//                                   .arg(ui->lineEdit->text(),
//                                   ui->lineEdit->text()))){
//         model.select();
//         qDebug()<<entry_city<<entry_phone;
//    }


//    QSqlTableModel model;
//       model.setTable("entry_phone");
//       //QString entry_city = ui->lineEdit->text();
//       //QString entry_name = ui->lineEdit->text();
//       model.setFilter("entry_city = 'Odessa'");
////       model.setFilter("entry_name = '%2'");
//       model.setSort(0, Qt::DescendingOrder);
//       model.select();

//           qDebug() << entry_city << entry_name;


//!    query1 = new QSqlQueryModel;

//    QString entry_city = ui->lineEdit->text();
//    QString entry_phone = ui->lineEdit->text();

//    query1->setQuery("SELECT entry_type FROM entry_phone WHERE entry_city = '@entry_city'' GROUP BY entry_id");

//!   qDebug()<<entry_city<<entry_phone;

//попробовать через where, если не получится!

    //if(model.setFilter("entry_name") = entry_name){
        //model.select();
        //qDebug()<<entry_city;

//        for (int i = 0; i < model.rowCount(); ++i) {
//                 QString entry_name = model.record(i).value("entry_name").toString();
//                 QString entry_city = model.record(i).value("entry_city").toString();
//                 int id = model.record(i).value("entry_vybor_id").toInt();
//                 qDebug() << entry_name << id << entry_city;
//                }
//        query1 = new QSqlQueryModel;
//        ui->tableView->setModel(query1);
//        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

   // }



//    QSqlTableModel model;
//         model.setTable("entry_phone");
//         QString entry_city = QString(ui->lineEdit->text());
//         //QString entry_vybor_id = ui->lineEdit->text();
//         model.setFilter(QString("entry_city = 'Odessa'"));
//         model.setSort(2, Qt::DescendingOrder);
//                model.select();
//                 qDebug()<<entry_city;





//         QSqlTableModel model;
//             model.setTable("entry_phone");
//             QString entry_city = ui->lineEdit->text();
//             QString entry_phone = ui->lineEdit->text();

//             if(model.setFilter(tr("entry_city '%1' or "
//                                        "entry_name '%2'")
//                                            .arg(entry_city,
//                                            entry_phone))){
//                  model.select();
//                  qDebug()<<entry_city<<entry_phone;
//             }


       /*modelTerminals = new ModelTerminals(this);
       modelTerminals->setTable("objects");
       modelTerminals->setHeaderData(2, Qt::Horizontal, "АЗС");
       modelTerminals->setHeaderData(4, Qt::Horizontal, "Наименование");
       modelTerminals->setHeaderData(6, Qt::Horizontal, "Примечание");
       modelTerminals->setHeaderData(5, Qt::Horizontal, "В работе");
       QString strFilter = QString("client_id = %1 order by terminal_id").arg(clientID);
       modelTerminals->setFilter(strFilter);
       modelTerminals->select();*/


//    QSqlTableModel model;
//         model.setTable("entry_phone");
//         QString entry_city = ui->lineEdit->text();
//         //QString entry_phone = ui->lineEdit->text();
//         model.setFilter("entry_city LIKE '%'" + entry_city + "'%'"); // + "or entry_phone = '%2'" + entry_phone
//         model.setSort(0, Qt::DescendingOrder);
//         model.select();



//         for (int i = 0; i < model.rowCount(); ++i) {
//             QString entry_type1 = model.record(i).value("entry_type").toString();
//             QString entry_name = model.record(i).value("entry_name").toString();
//             QString entry_phone = model.record(i).value("entry_phone").toString();
//             QString entry_city = model.record(i).value("entry_city").toString();
//             QString entry_email = model.record(i).value("entry_email").toString();
//             QString entry_vybor_id = model.record(i).value("entry_vybor_id").toString();
//             qDebug() << "   " << entry_type1 << "   " << entry_name << "   " << entry_phone << "   " << entry_vybor_id << "   " << entry_city << "   " << entry_email;

//         }

         //again try with where in failure case

}

