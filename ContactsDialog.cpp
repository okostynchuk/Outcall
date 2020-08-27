#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QGuiApplication>
#include <QScreen>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QDebug>

ContactsDialog::ContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

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

    queryModel = new QSqlQueryModel;

    queries.append(queryModel);

    queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    queryModel->insertColumn(2);
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Тип"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("ФИО / Название"));
    queryModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Телефон"));
    queryModel->setHeaderData(5, Qt::Horizontal, QObject::tr("Город"));
    queryModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Адрес"));
    queryModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Email"));
    queryModel->setHeaderData(8, Qt::Horizontal, QObject::tr("VyborID"));
    queryModel->insertColumn(10);
    queryModel->setHeaderData(10, Qt::Horizontal, QObject::tr("Заметка"));

    ui->tableView->setModel(queryModel);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->addPersonButton, &QPushButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->addOrgButton, &QPushButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton, &QPushButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->comboBox_list, SIGNAL(currentTextChanged(QString)), this, SLOT(onUpdate()));
    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showCard(const QModelIndex &)));

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        ui->tableView->setIndexWidget(queryModel->index(row_index, 2), addImageLabel(row_index));

        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(queryModel->data(queryModel->index(row_index, 9)).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addWidgetNote(row_index, "URL"));
        else
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addWidgetNote(row_index, ""));
    }

    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(9, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Stretch);

    onComboBoxSelected();

    ui->tableView->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

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
        queryModel->setQuery(queryModel->query().lastQuery());

        onUpdate();
    }
}

void ContactsDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    ui->lineEdit->setFocus();

    onUpdate();
}

void ContactsDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    ui->lineEdit->clear();

    ui->comboBox->setCurrentIndex(0);

    ui->comboBox_list->setCurrentIndex(0);

    go = "default";
    filter = false;
    page = "1";
}

void ContactsDialog::onAddPerson()
{
    addContactDialog = new AddContactDialog;
    connect(addContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addContactDialog->show();
    addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ContactsDialog::onAddOrg()
{
    addOrgContactDialog = new AddOrgContactDialog;
    connect(addOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addOrgContactDialog->show();
    addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ContactsDialog::showCard(const QModelIndex &index)
{
    QString updateID = queryModel->data(queryModel->index(index.row(), 0)).toString();
    int row = ui->tableView->currentIndex().row();

    if (queryModel->data(queryModel->index(row, 1)).toString() == "person")
    {
         viewContactDialog = new ViewContactDialog;
         viewContactDialog->setValuesContacts(updateID);
         connect(viewContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
         viewContactDialog->show();
         viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        viewOrgContactDialog->show();
        viewOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void ContactsDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
        widgets[i]->deleteLater();

    for (int i = 0; i < layouts.size(); ++i)
        layouts[i]->deleteLater();

    for (int i = 0; i < labels.size(); ++i)
        labels[i]->deleteLater();

    for (int i = 0; i < queries.size(); ++i)
        queries[i]->deleteLater();

    widgets.clear();
    layouts.clear();
    labels.clear();
    queries.clear();
}

void ContactsDialog::onUpdate()
{
    if (!queries.isEmpty())
        deleteObjects();

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

        queryModel = new QSqlQueryModel;

        if (ui->lineEdit_page->text() == "1")
            queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        else
            queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));

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

            queryModel = new QSqlQueryModel;

            if (ui->lineEdit_page->text() == "1")
                queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
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

            queryModel = new QSqlQueryModel;

            if (ui->lineEdit_page->text() == "1")
                queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }
        else if (entry_comment != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%'");
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

            queryModel = new QSqlQueryModel;

            if (ui->lineEdit_page->text() == "1")
                queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }

        go = "default";
    }

    queries.append(queryModel);

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    queryModel->insertColumn(2);
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Тип"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("ФИО / Название"));
    queryModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Телефон"));
    queryModel->setHeaderData(5, Qt::Horizontal, QObject::tr("Город"));
    queryModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Адрес"));
    queryModel->setHeaderData(7, Qt::Horizontal, QObject::tr("Email"));
    queryModel->setHeaderData(8, Qt::Horizontal, QObject::tr("VyborID"));
    queryModel->insertColumn(10);
    queryModel->setHeaderData(10, Qt::Horizontal, QObject::tr("Заметка"));

    ui->tableView->setModel(queryModel);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        ui->tableView->setIndexWidget(queryModel->index(row_index, 2), addImageLabel(row_index));

        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(queryModel->data(queryModel->index(row_index, 9)).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addWidgetNote(row_index, "URL"));
        else
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addWidgetNote(row_index, ""));
    }

    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(9, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Stretch);
}

QWidget* ContactsDialog::addImageLabel(int row_index)
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* imageLabel = new QLabel(wgt);

    layout->addWidget(imageLabel, 0, Qt::AlignCenter);

    if (queryModel->data(queryModel->index(row_index, 1)).toString() == "person")
        imageLabel->setPixmap(QPixmap(":/images/person.png").scaled(30, 30, Qt::KeepAspectRatio));
    else
        imageLabel->setPixmap(QPixmap(":/images/org.png").scaled(30, 30, Qt::KeepAspectRatio));

    wgt->setLayout(layout);

    widgets.append(wgt);
    layouts.append(layout);
    labels.append(imageLabel);

    return wgt;
}

QWidget* ContactsDialog::addWidgetNote(int row_index, QString url)
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* noteLabel = new QLabel(wgt);

    layout->addWidget(noteLabel);

    QString note = queryModel->data(queryModel->index(row_index, 9)).toString();

    if (url == "URL")
    {
        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
        QStringList hrefs;

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            hrefs << href;
        }

        for (int i = 0; i < hrefs.length(); ++i)
            note.replace(QRegularExpression("(^|\\s)" + QRegularExpression::escape(hrefs.at(i)) + "(\\s|$)"), QString(" <a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a> "));
    }

    noteLabel->setText(note);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    wgt->setLayout(layout);

    widgets.append(wgt);
    layouts.append(layout);
    labels.append(noteLabel);

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

    if (ui->comboBox->currentText() == tr("Поиск по ФИО / названию"))
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

        queryModel = new QSqlQueryModel;

        queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

        onUpdate();
    }
    else if (ui->comboBox->currentText() == tr("Поиск по номеру телефона"))
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

        queryModel = new QSqlQueryModel;

        queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

        onUpdate();
    }
    else if (ui->comboBox->currentText() == tr("Поиск по заметке"))
    {
        entry_comment = ui->lineEdit->text();

        query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%'");
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

        queryModel = new QSqlQueryModel;

        queryModel->setQuery("SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

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

void ContactsDialog::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}
