#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

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

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->addOrgButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton, &QAbstractButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ContactsDialog::currentIndexChanged);
    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &ContactsDialog::showCard);

    filter = false;

    page = "1";

    go = "default";

    loadContacts();
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

void ContactsDialog::showEvent(QShowEvent*)
{
    selectionModel = ui->tableView->selectionModel()->selectedRows();

    ui->lineEdit->setFocus();

    go = "default";

    loadContacts();
}

void ContactsDialog::closeEvent(QCloseEvent*)
{
    selectionModel.clear();

    ui->tableView->clearSelection();

    ui->lineEdit->clear();

    ui->comboBox->setCurrentIndex(0);

    ui->comboBox_list->setCurrentIndex(0);

    page = "1";

    go = "default";
}

void ContactsDialog::onUpdate()
{
    selectionModel.clear();

    ui->tableView->clearSelection();

    loadContacts();
}

void ContactsDialog::onAddPerson()
{
    addContactDialog = new AddContactDialog;
    connect(addContactDialog, &AddContactDialog::sendData, this, &ContactsDialog::receiveData);
    addContactDialog->show();
    addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ContactsDialog::onAddOrg()
{
    addOrgContactDialog = new AddOrgContactDialog;
    connect(addOrgContactDialog, &AddOrgContactDialog::sendData, this, &ContactsDialog::receiveData);
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
         connect(viewContactDialog, &ViewContactDialog::sendData, this, &ContactsDialog::receiveData);
         viewContactDialog->show();
         viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog, &ViewOrgContactDialog::sendData, this, &ContactsDialog::receiveData);
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

void ContactsDialog::loadContacts()
{
    if (!queries.isEmpty())
        deleteObjects();

    QString queryString = "SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone "
                          "ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, "
                          "entry_vybor_id, entry_comment FROM entry_phone ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone ";

    QString searchString;

    if (filter)
    {
        if (ui->comboBox->currentIndex() == 0)
            searchString.append("WHERE entry_name LIKE '%" + ui->lineEdit->text() + "%' ");
        else if (ui->comboBox->currentIndex() == 1)
            searchString.append("WHERE entry_phone LIKE '%" + ui->lineEdit->text() + "%' ");
        else if (ui->comboBox->currentIndex() == 2)
            searchString.append("WHERE entry_comment LIKE '%" + ui->lineEdit->text() + "%' ");
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

    queryModel = new QSqlQueryModel;

    queryModel->setQuery(queryString);

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

    if (!selectionModel.isEmpty())
        for (int i = 0; i < selectionModel.length(); ++i)
        {
            QModelIndex index = selectionModel.at(i);

            ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
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

void ContactsDialog::searchFunction()
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

void ContactsDialog::currentIndexChanged()
{
    go = "default";

    onUpdate();
}

void ContactsDialog::on_updateButton_clicked()
{
    go = "default";

    onUpdate();
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
