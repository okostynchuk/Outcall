#include "AddOrgToPerson.h"
#include "ui_AddOrgToPerson.h"

AddOrgToPerson::AddOrgToPerson(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOrgToPerson)
{
    ui->setupUi(this);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    connect(ui->tableView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(getOrgID(const QModelIndex &)));

    connect(ui->comboBox_list, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged()));

    page = "1";

    go = "default";

    onUpdate();
}

AddOrgToPerson::~AddOrgToPerson()
{
    deleteObjects();

    delete validator;
    delete ui;
}

void AddOrgToPerson::deleteObjects()
{
    for (int i = 0; i < queries.size(); ++i)
        queries[i]->deleteLater();

    queries.clear();
}

void AddOrgToPerson::getOrgID(const QModelIndex &index)
{
    QString id = queryModel->data(queryModel->index(index.row(), 0)).toString();

    emit sendOrgID(id);

    close();
}

void AddOrgToPerson::onUpdate()
{
    if (!queries.isEmpty())
        deleteObjects();

    QString queryString = "SELECT entry_id, entry_name, entry_city, entry_address FROM entry_phone WHERE entry_type = 'org' ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'org' ";

    QString searchString;

    if (filter == true)
    {
        if (ui->comboBox->currentIndex() == 0)
             searchString.append("AND entry_name LIKE '%" + ui->lineEdit->text() + "%' ");
        else if (ui->comboBox->currentIndex() == 1)
             searchString.append("AND entry_city LIKE '%" + ui->lineEdit->text() + "%' ");
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
    queryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Название"));
    queryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Город"));
    queryModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Адрес"));

    ui->tableView->setModel(queryModel);

    queries.append(queryModel);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
}

void AddOrgToPerson::searchFunction()
{
    if (ui->lineEdit->text().isEmpty())
    {
        filter = false;

        onUpdate();

        return;
    }

    filter = true;

    page = "1";

    go = "default";

    onUpdate();
}

void AddOrgToPerson::currentIndexChanged()
{
    go = "default";

    onUpdate();
}

void AddOrgToPerson::on_previousButton_clicked()
{
    go = "previous";

    onUpdate();
}

void AddOrgToPerson::on_nextButton_clicked()
{
    go = "next";

    onUpdate();
}

void AddOrgToPerson::on_previousStartButton_clicked()
{
    go = "previousStart";

    onUpdate();
}

void AddOrgToPerson::on_nextEndButton_clicked()
{
    go = "nextEnd";

    onUpdate();
}

void AddOrgToPerson::on_lineEdit_page_returnPressed()
{
    go = "enter";

    onUpdate();
}

void AddOrgToPerson::on_lineEdit_returnPressed()
{
    searchFunction();
}

void AddOrgToPerson::on_searchButton_clicked()
{
    searchFunction();
}
