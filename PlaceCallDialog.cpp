#include "PlaceCallDialog.h"
#include "ui_PlaceCallDialog.h"
#include "Global.h"
#include "AsteriskManager.h"
#include "Notifier.h"

#include <QHeaderView>
#include <QTableView>
#include <QBoxLayout>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QModelIndex>

PlaceCallDialog::PlaceCallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceCallDialog)
{
    ui->setupUi(this);

    QRegExp RegExp("^[\\+]?[0-9]*$");
    validator = new QRegExpValidator(RegExp, this);
    ui->phoneLine->setValidator(validator);

    connect(ui->callButton,    &QPushButton::clicked,           this, &PlaceCallDialog::onCallButton);
    connect(ui->cancelButton,  &QPushButton::clicked,           this, &PlaceCallDialog::onCancelButton);
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlaceCallDialog::clearEditText);
    connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(showNumber(const QModelIndex &)));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;

    ui->lineEdit_2->hide();
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    if(!opened)
        ui->lineEdit->setReadOnly(true);
    else
        ui->lineEdit->setReadOnly(false);

    onComboBoxSelected();

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();
    ui->my_Number->setText(my_number);
}

PlaceCallDialog::~PlaceCallDialog()
{
    delete ui;
    delete query1;
    delete query2;
    delete validator;
}

void PlaceCallDialog::showNumber(const QModelIndex &index)
{
    QString updateID = query1->data(query1->index(index.row(), 0)).toString();
    int row = ui->tableView->currentIndex().row();
    if (query2->data(query2->index(row, 0)).toString() == "person" || query2->data(query2->index(row, 0)).toString() == "org")
    {
         chooseNumber = new ChooseNumber;
         chooseNumber->setValuesNumber(updateID);
         connect(chooseNumber, SIGNAL(sendNumber(QString &)), this, SLOT(receiveNumber(QString &)));
         chooseNumber->show();
         chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void PlaceCallDialog::receiveNumber(QString &number)
{
    ui->phoneLine->setText(number);
}

void PlaceCallDialog::modelNull()
{
    ui->tableView->setModel(NULL);
    ui->lineEdit->clear();
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
    ui->comboBox->addItem(trUtf8("Поиск по ФИО / названию"));
    ui->comboBox->addItem(trUtf8("Поиск по номеру телефона"));
    ui->comboBox->addItem(trUtf8("Поиск сотрудников по организации"));
}

void PlaceCallDialog::on_lineEdit_returnPressed()
{
    update = "filter";
    ui->phoneLine->clear();

    if (QString(ui->lineEdit->text()).isEmpty())
    {
        ui->lineEdit_2->clear();
        ui->lineEdit_2->hide();
        return;
    }
    else if(ui->comboBox->currentText() == "Поиск по ФИО / названию" || ui->comboBox->currentText() == "Search by full name / name" || ui->comboBox->currentText() == "Пошук по ПІБ / назві")
    {
        QString entry_name = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n') FROM entry_phone ep WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");

        onUpdate();
    }
    else if(ui->comboBox->currentText() == "Поиск по номеру телефона" || ui->comboBox->currentText() == "Search by telephone" || ui->comboBox->currentText() == "Пошук по номеру телефона")
    {
        QString entry_phone = ui->lineEdit->text();
        query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n') FROM entry_phone ep WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY ep.entry_id");
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
        onUpdate();
    }
    else if(ui->comboBox->currentText() == "Поиск сотрудников по организации" || ui->comboBox->currentText() == "Search of staff by the org" || ui->comboBox->currentText() == "Пошук за співробітниками організації")
    {
        QString entry_org = ui->lineEdit->text();
        QSqlDatabase db;
        QSqlQuery query_org(db);
        query_org.prepare("SELECT entry_id, entry_name FROM entry_phone WHERE entry_type = 'org' AND entry_name LIKE '%" + entry_org + "%'");
        query_org.exec();
        QString orgID = NULL;
        QString orgName = NULL;

        while (query_org.next())
        {
            orgID = query_org.value(0).toString();
            orgName = query_org.value(1).toString();
        }
        if (orgID != NULL)
        {
            query1->setQuery("SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n') FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + orgID + "' GROUP BY ep.entry_id");
            ui->lineEdit_2->show();
            ui->lineEdit_2->setText(tr("Сотрудники организации \"") + orgName + tr("\""));
            onUpdate();

        }
        else
        {
            ui->tableView->setModel(NULL);
            ui->lineEdit_2->clear();
        }
        query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id");
    }
}

void PlaceCallDialog::clearEditText()
{
    ui->phoneLine->clear();
    ui->lineEdit->clear();
    ui->lineEdit_2->hide();
}

void PlaceCallDialog::show()
{
    QDialog::show();
    ui->phoneLine->clear();
    modelNull();
}

void PlaceCallDialog::onCallButton()
{
    if (!ui->phoneLine->text().isEmpty())
    {
        const QString number   = ui->phoneLine->text();
        const QString from     = my_number;
        const QString protocol = global::getSettingsValue(from, "extensions").toString();

        g_pAsteriskManager->originateCall(from, number, protocol, from);
    }
}

void PlaceCallDialog::onCancelButton()
{
    QDialog::close();
}
