#include "AddPhoneNumberToContactDialog.h"
#include "ui_AddPhoneNumberToContactDialog.h"

#include <QMessageBox>

AddPhoneNumberToContactDialog::AddPhoneNumberToContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPhoneNumberToContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &AddPhoneNumberToContactDialog::addPhoneNumber);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &AddPhoneNumberToContactDialog::currentIndexChanged);

    page = "1";

    go = "default";

    onUpdate();
}

AddPhoneNumberToContactDialog::~AddPhoneNumberToContactDialog()
{
    deleteObjects();

    delete validator;
    delete ui;
}

void AddPhoneNumberToContactDialog::deleteObjects()
{
    for (int i = 0; i < queries.size(); ++i)
        queries[i]->deleteLater();

    queries.clear();
}

void AddPhoneNumberToContactDialog::setPhoneNumber(QString receivedPhoneNumber)
{
    phoneNumber = receivedPhoneNumber;
}

void AddPhoneNumberToContactDialog::addPhoneNumber(const QModelIndex &index)
{
    QSqlQuery query(db);

    query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = ?)");
    query.addBindValue(phoneNumber);
    query.exec();
    query.next();

    if (query.value(0) != 0)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Данный номер уже привязан к контакту!"), QMessageBox::Ok);

        return;
    }

    QString id = queryModel->data(queryModel->index(index.row(), 0)).toString();

    query.prepare("SELECT COUNT(entry_phone) FROM entry_phone WHERE entry_id = ?");
    query.addBindValue(id);
    query.exec();
    query.first();

    if (query.value(0).toInt() < 5)
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

                QMessageBox::information(this, tr("Уведомление"), tr("Номер успешно добавлен!"), QMessageBox::Ok);
                break;
            case QMessageBox::No:
                msgBox.close();
                break;
            default:
                break;
        }
    }
    else
        QMessageBox::critical(this, tr("Ошибка"), tr("Контакту не может быть присвоено больше 5 номеров!"), QMessageBox::Ok);
}

void AddPhoneNumberToContactDialog::onUpdate()
{
    if(!queries.isEmpty())
        deleteObjects();

    QString queryString = "SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone "
                          "ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_comment FROM entry_phone ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone ";

    QString searchString;

    if (filter == true)
    {
        if (ui->comboBox->currentIndex() == 0)
            searchString = "WHERE entry_name LIKE '%" + ui->lineEdit->text() + "%' ";
        else if (ui->comboBox->currentIndex() == 1)
            searchString = "WHERE entry_phone LIKE '%" + ui->lineEdit->text() + "%' ";
        else if (ui->comboBox->currentIndex() == 2)
            searchString = "WHERE entry_comment LIKE '%" + ui->lineEdit->text() + "%' ";
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

    queries.append(queryModel);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("ФИО / Название"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Телефон"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Город"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(queryModel);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    }
}

void AddPhoneNumberToContactDialog::searchFunction()
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

void AddPhoneNumberToContactDialog::currentIndexChanged()
{
    go = "default";

    onUpdate();
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
