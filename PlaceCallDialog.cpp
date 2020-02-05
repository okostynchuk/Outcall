#include "PlaceCallDialog.h"
#include "ui_PlaceCallDialog.h"
#include "Global.h"
#include "ContactManager.h"
#include "SearchBox.h"
#include "AsteriskManager.h"
#include "Notifier.h"

#include <QHash>
#include <QString>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QTableView>
#include <QBoxLayout>
#include <QClipboard>
#include <QSqlDatabase>
#include <QHeaderView>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QGuiApplication>
#include <QScreen>
#include <QLabel>
#include <QModelIndex>
#include <QScrollBar>
#include <QStringList>

PlaceCallDialog::PlaceCallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceCallDialog)
{
    ui->setupUi(this);

    connect(ui->callButton,    &QPushButton::clicked,           this, &PlaceCallDialog::onCallButton);
    connect(ui->cancelButton,  &QPushButton::clicked,           this, &PlaceCallDialog::onCancelButton);
    //connect(ui->searchLine,    &SearchBox::selected,            this, &PlaceCallDialog::onChangeContact);
    //connect(ui->treeWidget,    &QTreeWidget::itemClicked,       this, &PlaceCallDialog::onItemClicked);
    //connect(ui->treeWidget,    &QTreeWidget::itemDoubleClicked, this, &PlaceCallDialog::onItemDoubleClicked);
    connect(g_pContactManager, &ContactManager::contactsLoaded, this, &PlaceCallDialog::onContactsLoaded);
    connect(g_Notifier,        &Notifier::settingsChanged,      this, &PlaceCallDialog::onSettingsChange);

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlaceCallDialog::clearEditText);
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showNumber(const QModelIndex &)));

    //void (QComboBox:: *sig)(const QString&) = &QComboBox::currentIndexChanged;
    //connect(ui->contactBox, sig, this, &PlaceCallDialog::onContactIndexChange);

    QStringList extensions = global::getSettingKeys("extensions");

    for (int i = 0; i < extensions.size(); ++i)
        ui->fromBox->addItem(extensions[i]);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_contacts = g_pContactManager->getContacts();
    QStringList data;

    foreach(Contact *contact, m_contacts)
    {
        data << contact->name;
       // ui->contactBox->addItem(contact->name);
    }
    //ui->searchLine->setData(data);

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;

    onComboBoxSelected();
}

PlaceCallDialog::~PlaceCallDialog()
{
    delete ui;
    delete query1;
    delete query2;
}

void PlaceCallDialog::showNumber(const QModelIndex &index)
{
    QString updateID = query1->data(query1->index(index.row(), 0)).toString();
    int row = ui->tableView->currentIndex().row();
    if (query2->data(query2->index(row, 0)).toString() == "person")
    {
         chooseNumber = new ChooseNumber;
         chooseNumber->setValuesNumber(updateID);
         chooseNumber->exec();
         chooseNumber->deleteLater();
    }
    else
    {
        chooseNumber = new ChooseNumber;
        chooseNumber->setValuesNumber(updateID);
        chooseNumber->exec();
        chooseNumber->deleteLater();
    }
}

void PlaceCallDialog::getValuesNumber(const QString &number)
{
    ui->phoneLine->setText(number);
    qDebug()<<"norm";
}

void PlaceCallDialog::onUpdate()
{
    if (update == "default")
    {
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n') FROM entry_phone ep GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
    }

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));

    ui->tableView->setModel(NULL);
    ui->tableView->setModel(query1);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    update = "default";
}

void PlaceCallDialog::onComboBoxSelected()
{
    ui->comboBox->addItem("Поиск по ФИО / названию");
    ui->comboBox->addItem("Поиск по номеру телефона");
}

void PlaceCallDialog::on_lineEdit_returnPressed()
{
    update = "filter";
    QComboBox::AdjustToContents;

    if(ui->comboBox->currentText() == "Поиск по ФИО / названию")
    {
        QString entry_name = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n') FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

        onUpdate();
    }
    if(ui->comboBox->currentText() == "Поиск по номеру телефона")
    {
        QString entry_phone = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n') FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

        onUpdate();
    }
}



void PlaceCallDialog::clearEditText(){
    ui->lineEdit->clear();
}

void PlaceCallDialog::show()
{
    //ui->contactBox->setCurrentIndex(0);
    //ui->searchLine->clear();
    QDialog::show();
}

void PlaceCallDialog::onCallButton()
{
    if (!ui->phoneLine->text().isEmpty())
    {
        const QString number   = ui->phoneLine->text();
        const QString from     = ui->fromBox->currentText();
        const QString protocol = global::getSettingsValue(from, "extensions").toString();

        g_pAsteriskManager->originateCall(from, number, protocol, from);

        QDialog::close();
    }
}

void PlaceCallDialog::onCancelButton()
{
    QDialog::close();
}

void PlaceCallDialog::onChangeContact(QString name)
{
//    Contact *contact = g_pContactManager->findContact(name);

//    clearCallTree();

//    if (contact != NULL)
//    {
//        QList<QString> numbers = contact->numbers.keys();

//        QString number, type;

//        for (int i = 0; i < numbers.size(); ++i)
//        {
//            type   = numbers[i];
//            number = contact->numbers.value(type);

//            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
//            item->setText(0, number);
//            item->setText(1, type);
//        }
//    }
//    QTreeWidgetItem *firstItem = ui->treeWidget->topLevelItem(0);
//    if (firstItem)
//    {
//        firstItem->setSelected(true);
//        QString number = firstItem->text(0);
//        ui->phoneLine->setText(number);
//    }
//    else
//    {
//        ui->phoneLine->clear();
//    }
}

void PlaceCallDialog::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
//    QString number = item->text(0);
//    const QString from = ui->fromBox->currentText();
//    const QString protocol = global::getSettingsValue(from, "extensions").toString();
//    g_pAsteriskManager->originateCall(from, number, protocol, from);

//    QDialog::close();
}

void PlaceCallDialog::onItemClicked(QTreeWidgetItem *item, int)
{
//    QString number = item->text(0);
//    ui->phoneLine->setText(number);
}

void PlaceCallDialog::onContactIndexChange(const QString &name)
{
//    ui->searchLine->setText(name);
//    onChangeContact(name);
}

void PlaceCallDialog::onContactsLoaded(QList<Contact *> &contacts)
{
//    QStringList data;
//    ui->contactBox->clear();

//    foreach(Contact *contact, contacts)
//    {
//        data << contact->name;
//        ui->contactBox->addItem(contact->name);
//    }

//    ui->searchLine->setData(data);
}

void PlaceCallDialog::clearCallTree()
{
//    int n = ui->treeWidget->topLevelItemCount();
//    for (int i = 0; i < n; ++i)
//        ui->treeWidget->takeTopLevelItem(i);

//    if (n > 0)
//        clearCallTree();
}

void PlaceCallDialog::onSettingsChange()
{
//    ui->fromBox->clear();

//    QStringList extensions = global::getSettingKeys("extensions");

//    for (int i = 0; i < extensions.size(); ++i)
//        ui->fromBox->addItem(extensions[i]);
}

