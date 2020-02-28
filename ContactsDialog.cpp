#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QGuiApplication>
#include <QScreen>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QSqlDatabase>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);

    QRegExp RegExp("^[0-9]*$");
    validator = new QRegExpValidator(RegExp, this);
    ui->lineEdit_page->setValidator(validator);

    this->showMaximized();
    ui->widget->move(0, 0);
    ui->widget->resize(QGuiApplication::screens().at(0)->geometry().width(), QGuiApplication::screens().at(0)->geometry().height());

    onComboBoxListSelected();
    query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone");
    query.exec();
    query.first();
    count = query.value(0).toInt();
    page = "1";
    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        remainder = count % ui->comboBox_list->currentText().toInt();
        if (remainder)
            remainder = 1;
        else
            remainder = 0;
        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }
    ui->lineEdit_page->setText(page);
    ui->label_pages->setText(tr("из ") + pages);

    query1 = new QSqlQueryModel;
    query2 = new QSqlQueryModel;
    query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
    query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

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
    ui->tableView->setModel(query1);
    queries.append(query1);
    queries.append(query2);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->addOrgButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton, &QAbstractButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->comboBox_list, SIGNAL(currentTextChanged(QString)), this, SLOT(onUpdate()));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query1->index(row_index, 1), addImageLabel(row_index));

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);

    onComboBoxSelected();

    go = "default";
    filter = false;
}

ContactsDialog::~ContactsDialog()
{
    deleteObjects();
    delete validator;
    delete ui;
}

void ContactsDialog::receiveData(bool updating)
{
    if (updating)
    {
        query1->setQuery(query1->query().lastQuery());
        query2->setQuery(query2->query().lastQuery());
        onUpdate();
    }
}

void ContactsDialog::showCard(const QModelIndex &index)
{
    QString updateID = query1->data(query1->index(index.row(), 0)).toString();
    int row = ui->tableView->currentIndex().row();
    if (query2->data(query2->index(row, 0)).toString() == "person")
    {
         viewContactDialog = new ViewContactDialog;
         viewContactDialog->setValuesContacts(updateID);
         connect(viewContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
         viewContactDialog->exec();
         viewContactDialog->deleteLater();
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        viewOrgContactDialog->exec();
        viewOrgContactDialog->deleteLater();
    }
}

void ContactsDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
    {
        widgets[i]->deleteLater();
    }
    qDeleteAll(layouts);
    qDeleteAll(labels);
    qDeleteAll(queries);
    widgets.clear();
    layouts.clear();
    labels.clear();
    queries.clear();
}

void ContactsDialog::onUpdate()
{
    if (filter == false)
    {
        query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        if (count <= ui->comboBox_list->currentText().toInt())
            pages = "1";
        else
        {
            remainder = count % ui->comboBox_list->currentText().toInt();
            if (remainder)
                remainder = 1;
            else
                remainder = 0;
            pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
        }
        if (go == "previous" && page != "1")
            page = QString::number(page.toInt() - 1);
        else if (go == "previousStart" && page != "1")
            page = "1";
        else if (go == "next" && page.toInt() < pages.toInt())
            page = QString::number(page.toInt() + 1);
        else if (go == "next" && page.toInt() >= pages.toInt())
            page = pages;
        else if (go == "nextEnd" && page.toInt() < pages.toInt())
            page = pages;
        else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
            page = ui->lineEdit_page->text();
        else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
        else if (go == "default" && page.toInt() >= pages.toInt())
            page = pages;
        ui->lineEdit_page->setText(page);
        ui->label_pages->setText(tr("из ") + pages);

        query1 = new QSqlQueryModel;
        query2 = new QSqlQueryModel;
        if (ui->lineEdit_page->text() == "1")
        {
            query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        }
        else
        {
            query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
            query2->setQuery("SELECT entry_type FROM entry_phone GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }

        go = "default";
    }
    else if (filter == true)
    {
        if (entry_name != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%'");
            query.exec();
            query.first();
            count = query.value(0).toInt();
            if (count <= ui->comboBox_list->currentText().toInt())
                pages = "1";
            else
            {
                remainder = count % ui->comboBox_list->currentText().toInt();
                if (remainder)
                    remainder = 1;
                else
                    remainder = 0;
                pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
            }
            if (go == "previous" && page != "1")
                page = QString::number(page.toInt() - 1);
            else if (go == "previousStart" && page != "1")
                page = "1";
            else if (go == "next" && page.toInt() < pages.toInt())
                page = QString::number(page.toInt() + 1);
            else if (go == "next" && page.toInt() >= pages.toInt())
                page = pages;
            else if (go == "nextEnd" && page.toInt() < pages.toInt())
                page = pages;
            else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
                page = ui->lineEdit_page->text();
            else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
            else if (go == "default" && page.toInt() >= pages.toInt())
                page = pages;
            ui->lineEdit_page->setText(page);
            ui->label_pages->setText(tr("из ") + pages);

            query1 = new QSqlQueryModel;
            query2 = new QSqlQueryModel;
            if (ui->lineEdit_page->text() == "1")
            {
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
                query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            }
            else
            {
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
                query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
            }
        }
        else if (entry_phone != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%'");
            query.exec();
            query.first();
            count = query.value(0).toInt();
            if (count <= ui->comboBox_list->currentText().toInt())
                pages = "1";
            else
            {
                remainder = count % ui->comboBox_list->currentText().toInt();
                if (remainder)
                    remainder = 1;
                else
                    remainder = 0;
                pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
            }
            if (go == "previous" && page != "1")
                page = QString::number(page.toInt() - 1);
            else if (go == "previousStart" && page != "1")
                page = "1";
            else if (go == "next" && page.toInt() < pages.toInt())
                page = QString::number(page.toInt() + 1);
            else if (go == "next" && page.toInt() >= pages.toInt())
                page = pages;
            else if (go == "nextEnd" && page.toInt() < pages.toInt())
                page = pages;
            else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
                page = ui->lineEdit_page->text();
            else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
            else if (go == "default" && page.toInt() >= pages.toInt())
                page = pages;
            ui->lineEdit_page->setText(page);
            ui->label_pages->setText(tr("из ") + pages);

            query1 = new QSqlQueryModel;
            query2 = new QSqlQueryModel;
            if (ui->lineEdit_page->text() == "1")
            {
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
                query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            }
            else
            {
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
                query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
            }
        }
        else if (entry_comment != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%'");
            query.exec();
            query.first();
            count = query.value(0).toInt();
            pages = nullptr;
            if (count <= ui->comboBox_list->currentText().toInt())
                pages = "1";
            else
            {
                remainder = count % ui->comboBox_list->currentText().toInt();
                if (remainder)
                    remainder = 1;
                else
                    remainder = 0;
                pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
            }
            if (go == "previous" && page != "1")
                page = QString::number(page.toInt() - 1);
            else if (go == "previousStart" && page != "1")
                page = "1";
            else if (go == "next" && page.toInt() < pages.toInt())
                page = QString::number(page.toInt() + 1);
            else if (go == "next" && page.toInt() >= pages.toInt())
                page = pages;
            else if (go == "nextEnd" && page.toInt() < pages.toInt())
                page = pages;
            else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
                page = ui->lineEdit_page->text();
            else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
            else if (go == "default" && page.toInt() >= pages.toInt())
                page = pages;
            ui->lineEdit_page->setText(page);
            ui->label_pages->setText(tr("из ") + pages);

            query1 = new QSqlQueryModel;
            query2 = new QSqlQueryModel;
            if (ui->lineEdit_page->text() == "1")
            {
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
                query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            }
            else
            {
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
                query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
            }
        }

        go = "default";
    }

    deleteObjects();

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
    ui->tableView->setModel(query1);
    queries.append(query1);
    queries.append(query2);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query1->index(row_index, 1), addImageLabel(row_index));

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
}

void ContactsDialog::onAddPerson()
{
    addContactDialog = new AddContactDialog;
    connect(addContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addContactDialog->exec();
    addContactDialog->deleteLater();
}

void ContactsDialog::onAddOrg()
{
    addOrgContactDialog = new AddOrgContactDialog;
    connect(addOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addOrgContactDialog->exec();
    addOrgContactDialog->deleteLater();
}

QWidget* ContactsDialog::addImageLabel(int &row_index)
{
    QWidget* wgt = new QWidget;
    QBoxLayout* l = new QHBoxLayout;
    QLabel *imageLabel = new QLabel(wgt);
    l->addWidget(imageLabel);
    if (query2->data(query2->index(row_index, 0)).toString() == "person")
    {
        imageLabel->setPixmap(QPixmap(":/images/person.png").scaled(30, 30, Qt::KeepAspectRatio));
    }
    else
    {
        imageLabel->setPixmap(QPixmap(":/images/org.png").scaled(30, 30, Qt::KeepAspectRatio));
    }
    wgt->setLayout(l);
    widgets.append(wgt);
    layouts.append(l);
    labels.append(imageLabel);
    return wgt;
}

void ContactsDialog::onComboBoxListSelected()
{
    ui->comboBox_list->addItem(tr("20"));
    ui->comboBox_list->addItem(tr("40"));
    ui->comboBox_list->addItem(tr("60"));
    ui->comboBox_list->addItem(tr("100"));
}

void ContactsDialog::onComboBoxSelected()
{
    ui->comboBox->addItem(tr("Поиск по ФИО / названию"));
    ui->comboBox->addItem(tr("Поиск по номеру телефона"));
    ui->comboBox->addItem(tr("Поиск по заметке"));
}

void ContactsDialog::searchFunction()
{
    if (ui->lineEdit->text().isEmpty())
    {
        filter = false;
        onUpdate();
        return;
    }

    go = "default";
    filter = true;

    entry_name = nullptr;
    entry_phone = nullptr;
    entry_comment = nullptr;

    if (ui->comboBox->currentText() == "Поиск по ФИО / названию" || ui->comboBox->currentText() == "Пошук по ПІБ / назві" || ui->comboBox->currentText() == "Search by full name / name")
    {
        entry_name = ui->lineEdit->text();

        query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%'");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        if (count <= ui->comboBox_list->currentText().toInt())
            pages = "1";
        else
        {
            remainder = count % ui->comboBox_list->currentText().toInt();
            if (remainder)
                remainder = 1;
            else
                remainder = 0;
            pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
        }
        page = "1";
        ui->lineEdit_page->setText(page);
        ui->label_pages->setText(tr("из ") + pages);

        query1 = new QSqlQueryModel;
        query2 = new QSqlQueryModel;
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по номеру телефона" || ui->comboBox->currentText() == "Search by phone number" || ui->comboBox->currentText() == "Пошук за номером телефона")
    {
        entry_phone = ui->lineEdit->text();

        query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%'");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        if (count <= ui->comboBox_list->currentText().toInt())
            pages = "1";
        else
        {
            remainder = count % ui->comboBox_list->currentText().toInt();
            if (remainder)
                remainder = 1;
            else
                remainder = 0;
            pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
        }
        page = "1";
        ui->lineEdit_page->setText(page);
        ui->label_pages->setText(tr("из ") + pages);

        query1 = new QSqlQueryModel;
        query2 = new QSqlQueryModel;
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по заметке" || ui->comboBox->currentText() == "Search by the note" || ui->comboBox->currentText() == "Пошук за коментарем")
    {
        entry_comment = ui->lineEdit->text();

        query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%'");
        query.exec();
        query.first();
        count = query.value(0).toInt();
        pages = nullptr;
        if (count <= ui->comboBox_list->currentText().toInt())
            pages = "1";
        else
        {
            remainder = count % ui->comboBox_list->currentText().toInt();
            if (remainder)
                remainder = 1;
            else
                remainder = 0;
            pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
        }
        page = "1";
        ui->lineEdit_page->setText(page);
        ui->label_pages->setText(tr("из ") + pages);

        query1 = new QSqlQueryModel;
        query2 = new QSqlQueryModel;
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        query2->setQuery("SELECT entry_type FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        onUpdate();
    }
}

void ContactsDialog::on_previousButton_clicked()
{
    go = "previous";
    onUpdate();
}

void ContactsDialog::on_nextButton_clicked()
{
    go = "next";
    onUpdate();
}

void ContactsDialog::on_previousStartButton_clicked()
{
    go = "previousStart";
    onUpdate();
}

void ContactsDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";
    onUpdate();
}

void ContactsDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";
    onUpdate();
}

void ContactsDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

void ContactsDialog::on_searchButton_clicked()
{
    searchFunction();
}
