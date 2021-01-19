/*
 * Класс служит для привязки организации к физ. лицу.
 */

#include "AddOrgToPerson.h"
#include "ui_AddOrgToPerson.h"

AddOrgToPerson::AddOrgToPerson(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddOrgToPerson)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &AddOrgToPerson::getOrgName);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, &AddOrgToPerson::currentIndexChanged);

    m_page = "1";

    m_go = "default";

    loadOrgs();
}

AddOrgToPerson::~AddOrgToPerson()
{
    delete ui;
}

/**
 * Выполняет отправку id и названия выбранной организации в классы
 * AddContactDialog и EditContactDialog.
 */
void AddOrgToPerson::getOrgName(const QModelIndex& index)
{
    QString id = m_queryModel->data(m_queryModel->index(index.row(), 0)).toString();
    QString name = m_queryModel->data(m_queryModel->index(index.row(), 1)).toString();

    emit sendOrg(id, name);

    close();
}

/**
 * Выполняет вывод и обновление списка организаций с и без фильтра.
 */
void AddOrgToPerson::loadOrgs()
{
    if (!m_queryModel.isNull())
        m_queryModel->deleteLater();

    m_queryModel = new QSqlQueryModel(this);

    QString queryString = "SELECT entry_id, entry_name, entry_city, entry_address FROM entry_phone WHERE entry_type = 'org' ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'org' ";

    QString searchString;

    if (m_filter == true)
    {
        switch (ui->comboBox->currentIndex())
        {
        case 0:
            searchString.append("AND entry_name LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
            break;
        case 1:
            searchString.append("AND entry_city LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
            break;
        }
    }

    queryCountString.append(searchString);

    QSqlQuery query(m_db);

    query.prepare(queryCountString);
    query.exec();
    query.first();

    qint32 count = query.value(0).toInt();

    QString pages = ui->label_pages->text();

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        qint32 remainder = count % ui->comboBox_list->currentText().toInt();

        if (remainder)
            remainder = 1;
        else
            remainder = 0;

        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }

    if (m_go == "previous" && m_page != "1")
        m_page = QString::number(m_page.toInt() - 1);
    else if (m_go == "previousStart" && m_page != "1")
        m_page = "1";
    else if (m_go == "next" && m_page.toInt() < pages.toInt())
        m_page = QString::number(m_page.toInt() + 1);
    else if (m_go == "next" && m_page.toInt() >= pages.toInt())
        m_page = pages;
    else if (m_go == "nextEnd" && m_page.toInt() < pages.toInt())
        m_page = pages;
    else if (m_go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        m_page = ui->lineEdit_page->text();
    else if (m_go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (m_go == "default" && m_page.toInt() >= pages.toInt())
        m_page = pages;

    ui->lineEdit_page->setText(m_page);

    ui->label_pages->setText(tr("из ") + pages);

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

    m_queryModel->setQuery(queryString);

    m_queryModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_queryModel->setHeaderData(1, Qt::Horizontal, tr("Название"));
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("Город"));
    m_queryModel->setHeaderData(3, Qt::Horizontal, tr("Адрес"));

    ui->tableView->setModel(m_queryModel);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    }
}

/**
 * Выполняет операции для последующего поиска по списку.
 */
void AddOrgToPerson::searchFunction()
{
    m_go = "default";

    if (ui->lineEdit->text().isEmpty())
    {
        if (m_filter)
            ui->tableView->scrollToTop();

        m_filter = false;

        loadOrgs();

        return;
    }

    m_filter = true;

    m_page = "1";

    ui->tableView->scrollToTop();

    loadOrgs();
}

/**
 * Выполняет обработку смены количества выводимых организаций на странице.
 */
void AddOrgToPerson::currentIndexChanged()
{
    m_go = "default";

    loadOrgs();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void AddOrgToPerson::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previous";

    loadOrgs();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void AddOrgToPerson::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "next";

    loadOrgs();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void AddOrgToPerson::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previousStart";

    loadOrgs();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void AddOrgToPerson::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "nextEnd";

    loadOrgs();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void AddOrgToPerson::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    m_go = "enter";

    loadOrgs();
}

/**
 * Выполняет поиск по списку при нажатии клавиши Enter,
 * находясь в поле поиска.
 */
void AddOrgToPerson::on_lineEdit_returnPressed()
{
    searchFunction();
}

/**
 * Выполняет поиск по списку при нажатии кнопки поиска.
 */
void AddOrgToPerson::on_searchButton_clicked()
{
    searchFunction();
}
