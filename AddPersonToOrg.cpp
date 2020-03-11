#include "AddPersonToOrg.h"
#include "ui_AddPersonToOrg.h"

AddPersonToOrg::AddPersonToOrg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPersonToOrg)
{
    ui->setupUi(this);

    QRegExp RegExp("^[0-9]*$");
    validator = new QRegExpValidator(RegExp, this);
    ui->lineEdit_page->setValidator(validator);

    onComboBoxListSelected();
    query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person'");
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
    query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(query1);
    queries.append(query1);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(getPersonID(const QModelIndex &)));
    connect(ui->comboBox_list, SIGNAL(currentTextChanged(QString)), this, SLOT(onUpdate()));

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    onComboBoxSelected();

    go = "default";
    filter = false;
}

AddPersonToOrg::~AddPersonToOrg()
{
    deleteObjects();
    delete validator;
    delete ui;
}

void AddPersonToOrg::deleteObjects()
{
    qDeleteAll(queries);
    queries.clear();
}

void AddPersonToOrg::getPersonID(const QModelIndex &index)
{
    QString id = query1->data(query1->index(index.row(), 0)).toString();
    emit sendPersonID(id);
    onUpdate();
    QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Сотрудник успешно добавлен!"), QMessageBox::Ok);
}

void AddPersonToOrg::onUpdate()
{
    if (filter == false)
    {
        query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person'");
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
        if (ui->lineEdit_page->text() == "1")
            query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        else
            query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));

        go = "default";
    }
    else if (filter == true)
    {
        if (entry_name != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person' AND entry_name LIKE '%" + entry_name + "%'");
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
            if (ui->lineEdit_page->text() == "1")
                query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) AND entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) AND entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }
        else if (entry_phone != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person' AND entry_phone LIKE '%" + entry_phone + "%'");
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
            if (ui->lineEdit_page->text() == "1")
                query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) AND entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) AND entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }
        else if (entry_comment != nullptr)
        {
            query.prepare("SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person' AND entry_comment LIKE '%" + entry_comment + "%'");
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
            if (ui->lineEdit_page->text() == "1")
                query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) AND entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                query1->setQuery("SELECT entry_id, entry_name, entry_phone, entry_comment FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) AND entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }

        go = "default";
    }

    deleteObjects();

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(query1);
    queries.append(query1);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void AddPersonToOrg::onComboBoxListSelected()
{
    ui->comboBox_list->addItem(tr("20"));
    ui->comboBox_list->addItem(tr("40"));
    ui->comboBox_list->addItem(tr("60"));
    ui->comboBox_list->addItem(tr("100"));
}

void AddPersonToOrg::onComboBoxSelected()
{
    ui->comboBox->addItem(tr("Поиск по ФИО"));
    ui->comboBox->addItem(tr("Поиск по номеру телефона"));
    ui->comboBox->addItem(tr("Поиск по заметке"));
}

void AddPersonToOrg::searchFunction()
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

    if (ui->comboBox->currentText() == "Поиск по ФИО" || ui->comboBox->currentText() == "Пошук по ПІБ" || ui->comboBox->currentText() == "Search by full name")
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
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
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
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        onUpdate();
    }
    else if (ui->comboBox->currentText() == "Поиск по заметке" || ui->comboBox->currentText() == "Search by the note" || ui->comboBox->currentText() == "Пошук за коментарем")
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

        query1 = new QSqlQueryModel;
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        onUpdate();
    }
}

void AddPersonToOrg::on_previousButton_clicked()
{
    go = "previous";
    onUpdate();
}

void AddPersonToOrg::on_nextButton_clicked()
{
    go = "next";
    onUpdate();
}

void AddPersonToOrg::on_previousStartButton_clicked()
{
    go = "previousStart";
    onUpdate();
}

void AddPersonToOrg::on_nextEndButton_clicked()
{
    go = "nextEnd";
    onUpdate();
}

void AddPersonToOrg::on_lineEdit_page_returnPressed()
{
    go = "enter";
    onUpdate();
}

void AddPersonToOrg::on_lineEdit_returnPressed()
{
    searchFunction();
}

void AddPersonToOrg::on_searchButton_clicked()
{
    searchFunction();
}

