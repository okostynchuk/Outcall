#include "AddPhoneNumberToContactDialog.h"
#include "ui_AddPhoneNumberToContactDialog.h"

AddPhoneNumberToContactDialog::AddPhoneNumberToContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPhoneNumberToContactDialog)
{
    ui->setupUi(this);

    QRegExp RegExp("^[0-9]*$");
    validator = new QRegExpValidator(RegExp, this);
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

    query1 = new QSqlQueryModel;
    query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Город"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(query1);
    queries.append(query1);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(setPhoneNumber(const QModelIndex &)));
    connect(ui->comboBox_list, SIGNAL(currentTextChanged(QString)), this, SLOT(onUpdate()));

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    onComboBoxSelected();

    go = "default";
    filter = false;
}

AddPhoneNumberToContactDialog::~AddPhoneNumberToContactDialog()
{
    deleteObjects();
    delete validator;
    delete ui;
}

void AddPhoneNumberToContactDialog::deleteObjects()
{
    qDeleteAll(queries);
    queries.clear();
}

void AddPhoneNumberToContactDialog::setPhoneNumberPopupWindow(QString &phoneNumberPopup)
{
    phoneNumber = phoneNumberPopup;
}

void AddPhoneNumberToContactDialog::setPhoneNumber(const QModelIndex &index)
{
    QString id = query1->data(query1->index(index.row(), 0)).toString();
    QSqlDatabase db;
    QSqlQuery query(db);
    QString sql = QString("SELECT entry_phone FROM entry_phone WHERE entry_id = %1").arg(id);
    query.prepare(sql);
    query.exec();

    int count = 0;
    while (query.next())
        count++;

    if (count < 5)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Добавление номера"));
        msgBox.setInformativeText(tr("Вы действительно хотите добавить номер к выбранному контакту?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, tr("Да"));
        msgBox.setButtonText(QMessageBox::No, tr("Нет"));
        int reply = msgBox.exec();
        switch (reply)
        {
            case QMessageBox::Yes:
                query.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query.addBindValue(id);
                query.addBindValue(phoneNumber);
                query.exec();

                emit sendData(true);
                close();
                msgBox.close();
                QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Номер успешно добавлен!"), QMessageBox::Ok);
                destroy(true);
                break;
            case QMessageBox::No:
                msgBox.close();
                break;
            default:
                break;
        }
    }
    else
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Контакту не может быть присвоено больше 5 номеров!"), QMessageBox::Ok);
}

void AddPhoneNumberToContactDialog::onUpdate()
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
        if (ui->lineEdit_page->text() == "1")
            query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        else
            query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));

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
            if (ui->lineEdit_page->text() == "1")
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
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
            if (ui->lineEdit_page->text() == "1")
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
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

            query1 = new QSqlQueryModel;
            if (ui->lineEdit_page->text() == "1")
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
            else
                query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
        }

        go = "default";
    }

    deleteObjects();

    query1->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    query1->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО / Название"));
    query1->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    query1->setHeaderData(3, Qt::Horizontal, QObject::tr("Город"));
    query1->setHeaderData(4, Qt::Horizontal, QObject::tr("Заметка"));
    ui->tableView->setModel(query1);
    queries.append(query1);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
}

void AddPhoneNumberToContactDialog::onComboBoxListSelected()
{
    ui->comboBox_list->addItem(tr("20"));
    ui->comboBox_list->addItem(tr("40"));
    ui->comboBox_list->addItem(tr("60"));
    ui->comboBox_list->addItem(tr("100"));
}

void AddPhoneNumberToContactDialog::onComboBoxSelected()
{
    ui->comboBox->addItem(tr("Поиск по ФИО / названию"));
    ui->comboBox->addItem(tr("Поиск по номеру телефона"));
    ui->comboBox->addItem(tr("Поиск по заметке"));
}

void AddPhoneNumberToContactDialog::searchFunction()
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
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_name LIKE '%" + entry_name + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
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
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_phone LIKE '%" + entry_phone + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
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
        query1->setQuery("SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone WHERE entry_comment LIKE '%" + entry_comment + "%' GROUP BY entry_id ORDER BY entry_name ASC LIMIT 0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()));
        onUpdate();
    }
}

void AddPhoneNumberToContactDialog::on_previousButton_clicked()
{
    go = "previous";
    onUpdate();
}

void AddPhoneNumberToContactDialog::on_nextButton_clicked()
{
    go = "next";
    onUpdate();
}

void AddPhoneNumberToContactDialog::on_previousStartButton_clicked()
{
    go = "previousStart";
    onUpdate();
}

void AddPhoneNumberToContactDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";
    onUpdate();
}

void AddPhoneNumberToContactDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";
    onUpdate();
}

void AddPhoneNumberToContactDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

void AddPhoneNumberToContactDialog::on_searchButton_clicked()
{
    searchFunction();
}
