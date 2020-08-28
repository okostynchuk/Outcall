#include "AddPersonToOrg.h"
#include "ui_AddPersonToOrg.h"

#include <QMessageBox>

AddPersonToOrg::AddPersonToOrg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPersonToOrg)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &AddPersonToOrg::getPersonID);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddPersonToOrg::currentIndexChanged);

    page = "1";

    go = "default";

    onUpdate();
}

AddPersonToOrg::~AddPersonToOrg()
{
    deleteObjects();

    delete validator;
    delete ui;
}

void AddPersonToOrg::deleteObjects()
{
    for (int i = 0; i < queries.size(); ++i)
        queries[i]->deleteLater();

    queries.clear();
}

void AddPersonToOrg::setOrgId(QString orgId)
{
    this->orgId = orgId;

    query.prepare("SELECT entry_name FROM entry WHERE id = ?");
    query.addBindValue(orgId);
    query.exec();
    query.next();

    orgName = query.value(0).toString();

    setWindowTitle(tr("Добавление сотрудников к организации") + " \"" + orgName + "\"");
}

void AddPersonToOrg::getPersonID(const QModelIndex &index)
{
    QString id = queryModel->data(queryModel->index(index.row(), 0)).toString();

    query.prepare("SELECT EXISTS (SELECT id FROM entry WHERE id = ?)");
    query.addBindValue(orgId);
    query.exec();
    query.next();

    if (query.value(0) == 0)
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Организации не существует или она была изменена!"), QMessageBox::Ok);

        return;
    }

    query.prepare("UPDATE entry SET entry_person_org_id = ? WHERE id = ?");
    query.addBindValue(orgId);
    query.addBindValue(id);
    query.exec();

    emit newPerson();

    onUpdate();

    QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Сотрудник успешно добавлен!"), QMessageBox::Ok);
}

void AddPersonToOrg::onUpdate()
{
    deleteObjects();

    QString queryString =  "SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone "
                               "ORDER BY entry_id SEPARATOR '\n'), entry_comment FROM entry_phone "
                               "WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR "
                               "entry_person_org_id is NULL) ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) ";

    QString searchString;

    if (filter == true)
    {
        if (ui->comboBox->currentIndex() == 0)
            searchString = "AND entry_name LIKE '%" + ui->lineEdit->text() + "%' ";
        else if (ui->comboBox->currentIndex() == 1)
            searchString = "AND entry_phone LIKE '%" + ui->lineEdit->text() + "%' ";
        else if (ui->comboBox->currentIndex() == 2)
            searchString = "AND entry_comment LIKE '%" + ui->lineEdit->text() + "%' ";
    }

    queryCountString.append(searchString);

    query.prepare(queryCountString);
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

    queryString.append(searchString);
    queryString.append("GROUP BY entry_id ORDER BY entry_name ASC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0," + QString::number(ui->lineEdit_page->text().toInt() *
                                                  ui->comboBox_list->currentText().toInt()));
    else
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt() -
                                                ui->comboBox_list->currentText().toInt()) + " , "
                           + QString::number(ui->comboBox_list->currentText().toInt()));

    queryModel->setQuery(queryString);

    queryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("ФИО"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Телефон"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Заметка"));

    ui->tableView->setModel(queryModel);

    queries.append(queryModel);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void AddPersonToOrg::searchFunction()
{
    go = "default";

    if (ui->lineEdit->text().isEmpty())
    {
        filter = false;

        onUpdate();

        return;
    }

    filter = true;

    page = "1";

    onUpdate();
}

void AddPersonToOrg::currentIndexChanged()
{
    go = "default";

    onUpdate();
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

