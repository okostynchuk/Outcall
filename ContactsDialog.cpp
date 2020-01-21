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
    query3 = new QSqlQueryModel;
    query4 = new QSqlQueryModel;
    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
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
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);

    QComboBox::AdjustToContents;
    onComboBoxSelected();
}

ContactsDialog::~ContactsDialog()
{
    delete ui;
}

void ContactsDialog::onUpdate()
{
    query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep GROUP BY ep.entry_id");
    query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
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
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
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
        addContactDialog->setValuesContacts(i);
        addContactDialog->exec();
    }
    else
    {
        g_Switch = "updateOrg";
        AddOrgContactDialog *addOrgContactDialog = new AddOrgContactDialog;
        addOrgContactDialog->setWindowTitle("Редактирование организации");
        addOrgContactDialog->setOrgValuesContacts(i);
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

void ContactsDialog::onComboBoxSelected(){

    ui->comboBox->setCurrentIndex(0); ui->comboBox->addItem("Поиск по ФИО / названию" );
    ui->comboBox->addItem("Поиск по номеру телефона");
    ui->comboBox->addItem("Поиск по заметке");

}

void ContactsDialog::tableUpdate(){

}

void ContactsDialog::on_lineEdit_returnPressed()
{
//    if(ui->comboBox->setProperty("currentIndex", 0)){
//        QSqlTableModel *model = new QSqlTableModel(ui->tableView);
//        model->setTable("entry_phone");
//        QString entry_name = ui->lineEdit->text();
//        QString strFilterName = QString("entry_name LIKE '%%1%'").arg(entry_name);
//        model->setFilter(strFilterName);
//        model->setSort(0, Qt::DescendingOrder);
//        model->select();
//        ui->tableView->setModel(NULL);
//        ui->tableView->setModel(model);


//         for (int i = 0; i < model->rowCount(); ++i) {
//             QString entry_type1 = model->record(i).value("entry_type").toString();
//             QString entry_name = model->record(i).value("entry_name").toString();
//             QString entry_phone = model->record(i).value("entry_phone").toString();
//             QString entry_city = model->record(i).value("entry_city").toString();
//             QString entry_email = model->record(i).value("entry_email").toString();
//             QString entry_vybor_id = model->record(i).value("entry_vybor_id").toString();
//             qDebug() << "   " << entry_type1 << "   " << entry_name << "   " << entry_phone << "   " << entry_vybor_id << "   " << entry_city << "   " << entry_email;

//         }
 //}

    if(ui->comboBox->setProperty("currentIndex", 0)){

        QString entry_name = ui->lineEdit->text();
        query3->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
        query4->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

        query3->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
        query3->insertColumn(1);
        query3->setHeaderData(1, Qt::Horizontal, tr("Тип"));
        query3->setHeaderData(2, Qt::Horizontal, QObject::tr("ФИО / Название"));
        query3->setHeaderData(3, Qt::Horizontal, QObject::tr("Телефон"));
        query3->setHeaderData(4, Qt::Horizontal, QObject::tr("Город"));
        query3->setHeaderData(5, Qt::Horizontal, QObject::tr("Адрес"));
        query3->setHeaderData(6, Qt::Horizontal, QObject::tr("Email"));
        query3->setHeaderData(7, Qt::Horizontal, QObject::tr("VyborID"));
        query3->setHeaderData(8, Qt::Horizontal, QObject::tr("Заметка"));
        query3->insertColumn(9);
        query3->setHeaderData(9, Qt::Horizontal, tr("Редактирование"));

        ui->tableView->setModel(NULL);
        ui->tableView->setModel(query3);
        for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
        {
            ui->tableView->setIndexWidget(query3->index(i, 1), addImageLabel(i));
            ui->tableView->setIndexWidget(query3->index(i, 9), createEditButton(i));
        }
        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
        //ui->tableView->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setDefaultSectionSize(115);

        ui->tableView->setColumnHidden(0, true);
        ui->tableView->resizeRowsToContents();
        //ui->tableView->resizeColumnsToContents();
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);


}

//    if(ui->comboBox->setProperty("currentIndex", 1)){

//        ui->widget->move(0, 0);
//        ui->widget->resize(QGuiApplication::screens().at(0)->geometry().width(), QGuiApplication::screens().at(0)->geometry().height());
//        QString entry_phone = ui->lineEdit->text();
//        query3->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_city, ep.entry_address, ep.entry_email, ep.entry_vybor_id, ep.entry_comment FROM entry_phone ep where entry_phone like '%" + entry_phone + "%' GROUP BY ep.entry_id");
//        query4->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

//        query3->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
//        query3->insertColumn(1);
//        query3->setHeaderData(1, Qt::Horizontal, tr("Тип"));
//        query3->setHeaderData(2, Qt::Horizontal, QObject::tr("ФИО / Название"));
//        query3->setHeaderData(3, Qt::Horizontal, QObject::tr("Телефон"));
//        query3->setHeaderData(4, Qt::Horizontal, QObject::tr("Город"));
//        query3->setHeaderData(5, Qt::Horizontal, QObject::tr("Адрес"));
//        query3->setHeaderData(6, Qt::Horizontal, QObject::tr("Email"));
//        query3->setHeaderData(7, Qt::Horizontal, QObject::tr("VyborID"));
//        query3->setHeaderData(8, Qt::Horizontal, QObject::tr("Заметка"));
//        query3->insertColumn(9);
//        query3->setHeaderData(9, Qt::Horizontal, tr("Редактирование"));

//        ui->tableView->setModel(NULL);
//        ui->tableView->setModel(query3);
//        for(int i = 0; i < ui->tableView->model()->rowCount(); ++i)
//        {
//            ui->tableView->setIndexWidget(query3->index(i, 1), addImageLabel(i));
//            ui->tableView->setIndexWidget(query3->index(i, 9), createEditButton(i));
//        }
//        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
//        ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
//        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
//        ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
//        ui->tableView->setColumnHidden(0, true);
//        ui->tableView->resizeRowsToContents();
//        ui->tableView->resizeColumnsToContents();
//        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);


//}

//        if(ui->comboBox->setProperty("currentIndex", 2)){
//         QSqlTableModel model;
//         model.setTable("entry_comment");
//         QString entry_comment = ui->lineEdit->text();
//         QString strFilterComment = QString("entry_phone LIKE '%%1%'").arg(entry_comment);
//         model.setFilter(strFilterComment);
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
//    }
}
