#include "PlaceCallDialog.h"
#include "ui_PlaceCallDialog.h"

#include "Global.h"
#include "AsteriskManager.h"

#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QModelIndex>

PlaceCallDialog::PlaceCallDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceCallDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[\\+]?[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->phoneLine->setValidator(validator);

    connect(ui->callButton,    &QAbstractButton::clicked,           this, &PlaceCallDialog::onCallButton);
    connect(ui->cancelButton,  &QAbstractButton::clicked,           this, &PlaceCallDialog::onCancelButton);
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &PlaceCallDialog::clearEditText);
    connect(ui->tableView, &QAbstractItemView::clicked, this, &PlaceCallDialog::showNumber);

    queryModel = new QSqlQueryModel;

    ui->lineEdit_2->hide();

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    my_number = global::getExtensionNumber("extensions");

    ui->my_Number->setText(my_number);
}

PlaceCallDialog::~PlaceCallDialog()
{
    delete ui;
    delete queryModel;
    delete validator;
}

void PlaceCallDialog::showNumber(const QModelIndex &index)
{
    QString id = queryModel->data(queryModel->index(index.row(), 0)).toString();
    int row = ui->tableView->currentIndex().row();

    if (queryModel->data(queryModel->index(row, 3)).toString() == "person" || queryModel->data(queryModel->index(row, 3)).toString() == "org")
    {
        QSqlDatabase db;
        QSqlQuery query(db);

        query.prepare("SELECT fone FROM fones WHERE entry_id = ?");
        query.addBindValue(id);
        query.exec();

        if (query.size() == 1)
        {
            query.next();

            QString number = query.value(0).toString();
            QString protocol = global::getSettingsValue(my_number, "extensions").toString();

            g_pAsteriskManager->originateCall(my_number, number, protocol, my_number);

            ui->phoneLine->setText(number);
        }
        else
        {
            chooseNumber = new ChooseNumber;
            chooseNumber->setValuesNumber(id);
            chooseNumber->fromPlaceDialog = true;
            connect(chooseNumber, &ChooseNumber::sendNumber, this, &PlaceCallDialog::receiveNumber);
            chooseNumber->show();
            chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
        }
    }
}

void PlaceCallDialog::receiveNumber(QString number)
{
    ui->phoneLine->setText(number);
}

void PlaceCallDialog::onUpdate()
{
    if (update == "default")
        queryModel->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_type FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC");

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО / Название"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));

    ui->tableView->setModel(NULL);
    ui->tableView->setModel(queryModel);

    ui->tableView->setColumnHidden(3, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    update = "default";
}

void PlaceCallDialog::on_lineEdit_returnPressed()
{
    update = "filter";

    ui->phoneLine->clear();

    if (QString(ui->lineEdit->text()).isEmpty())
    {
        ui->lineEdit->clear();

        ui->lineEdit_2->clear();        
        ui->lineEdit_2->hide();

        ui->tableView->setModel(NULL);

        return;
    }
    else if (ui->comboBox->currentText() == tr("Поиск по ФИО / названию"))
    {
        QString entry_name = ui->lineEdit->text();

        queryModel->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC");

        onUpdate();
    }
    else if (ui->comboBox->currentText() == tr("Поиск по номеру телефона"))
    {
        QString entry_phone = ui->lineEdit->text();

        queryModel->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC");

        onUpdate();
    }
    else if (ui->comboBox->currentText() == tr("Поиск сотрудников по организации"))
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
            queryModel->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_type FROM entry_phone WHERE entry_type = 'person' AND entry_person_org_id = '" + orgID + "' GROUP BY entry_id ORDER BY entry_name ASC");

            ui->lineEdit_2->show();
            ui->lineEdit_2->setText(tr("Сотрудники организации") + " \"" + orgName + "\"");

            onUpdate();
        }
        else
        {
            ui->tableView->setModel(NULL);

            ui->lineEdit_2->clear();
        }
    }
}

void PlaceCallDialog::clearEditText()
{
    ui->phoneLine->clear();

    ui->lineEdit_2->hide();
}

void PlaceCallDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    ui->lineEdit->setFocus();
}

void PlaceCallDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    ui->comboBox->setCurrentIndex(0);

    ui->lineEdit->clear();

    ui->lineEdit_2->hide();

    ui->phoneLine->clear();

    ui->tableView->setModel(NULL);
}

void PlaceCallDialog::onCallButton()
{
    if (!ui->phoneLine->text().isEmpty())
    {
        QString to = ui->phoneLine->text();
        QString from = my_number;
        QString protocol = global::getSettingsValue(from, "extensions").toString();

        g_pAsteriskManager->originateCall(from, to, protocol, from);
    }
}

void PlaceCallDialog::onCancelButton()
{
    QDialog::close();
}

void PlaceCallDialog::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}

void PlaceCallDialog::on_phoneLine_returnPressed()
{
    onCallButton();
}
