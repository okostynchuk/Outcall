/*
 * Класс служит для добавления физ. лиц к организации.
 */

#include "AddPersonToOrg.h"
#include "ui_AddPersonToOrg.h"

#include "Global.h"

#include <QMessageBox>

AddPersonToOrg::AddPersonToOrg(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddPersonToOrg)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &AddPersonToOrg::addPerson);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, &AddPersonToOrg::currentIndexChanged);

    m_page = "1";

    m_go = "default";

    loadPersons();
}

AddPersonToOrg::~AddPersonToOrg()
{
    delete ui;
}

/**
 * Получает id организации из класса ViewOrgContactDialog.
 */
void AddPersonToOrg::setOrgId(const QString& orgId)
{
    m_orgId = orgId;

    QSqlQuery query(m_db);

    query.prepare("SELECT entry_name FROM entry WHERE id = ?");
    query.addBindValue(m_orgId);
    query.exec();
    query.next();

    QString orgName = query.value(0).toString();

    setWindowTitle(tr("Добавление сотрудников к организации") + " \"" + orgName + "\"");
}

/**
 * Выполняет привязку выбранного физ. лица к организации.
 */
void AddPersonToOrg::addPerson(const QModelIndex &index)
{
    QString id = m_queryModel->data(m_queryModel->index(index.row(), 0)).toString();

    QSqlQuery query(m_db);

    query.prepare("SELECT EXISTS (SELECT id FROM entry WHERE id = ?)");
    query.addBindValue(m_orgId);
    query.exec();
    query.next();

    if (query.value(0) == 0)
    {
        MsgBoxError(tr("Организации не существует или она была изменена!"));

        return;
    }

    query.prepare("UPDATE entry SET entry_person_org_id = ? WHERE id = ?");
    query.addBindValue(m_orgId);
    query.addBindValue(id);
    query.exec();

    emit newPerson();

    loadPersons();

    MsgBoxInformation(tr("Сотрудник успешно добавлен!"));
}

/**
 * Выполняет вывод и обновление списка физ. лиц,
 * не привязанных к какой-либо организации, с и без фильтра.
 */
void AddPersonToOrg::loadPersons()
{
    if (!m_queryModel.isNull())
        m_queryModel->deleteLater();

    m_queryModel = new QSqlQueryModel(this);

    QString queryString =  "SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone "
                               "ORDER BY entry_id SEPARATOR '\n'), entry_comment FROM entry_phone "
                               "WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR "
                               "entry_person_org_id is NULL) ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone WHERE entry_type = 'person' AND (entry_person_org_id = 0 OR entry_person_org_id is NULL) ";

    QString searchString;

    if (m_filter == true)
    {
        switch (ui->comboBox->currentIndex())
        {
        case 0:
            searchString = "AND entry_name LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ";
            break;
        case 1:
            searchString = "AND entry_phone LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ";
            break;
        case 2:
            searchString = "AND entry_comment LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ";
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
    m_queryModel->setHeaderData(1, Qt::Horizontal, tr("ФИО"));
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("Телефон"));
    m_queryModel->setHeaderData(3, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(m_queryModel);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    }
}

/**
 * Выполняет операции для последующего поиска по списку.
 */
void AddPersonToOrg::searchFunction()
{
    m_go = "default";

    if (ui->lineEdit->text().isEmpty())
    {
        if (m_filter)
            ui->tableView->scrollToTop();

        m_filter = false;

        loadPersons();

        return;
    }

    m_filter = true;

    m_page = "1";

    ui->tableView->scrollToTop();

    loadPersons();
}

/**
 * Выполняет обработку смены количества выводимых физ. лиц на странице.
 */
void AddPersonToOrg::currentIndexChanged()
{
    m_go = "default";

    loadPersons();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void AddPersonToOrg::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previous";

    loadPersons();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void AddPersonToOrg::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "next";

    loadPersons();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void AddPersonToOrg::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previousStart";

    loadPersons();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void AddPersonToOrg::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "nextEnd";

    loadPersons();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void AddPersonToOrg::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    m_go = "enter";

    loadPersons();
}

/**
 * Выполняет поиск по списку при нажатии клавиши Enter,
 * находясь в поле поиска.
 */
void AddPersonToOrg::on_lineEdit_returnPressed()
{
    searchFunction();
}

/**
 * Выполняет поиск по списку при нажатии кнопки поиска.
 */
void AddPersonToOrg::on_searchButton_clicked()
{
    searchFunction();
}
