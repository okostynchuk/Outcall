/*
 * Класс служит для вывода списка всех контактов и различного взаимодействия с ним.
 */

#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

ContactsDialog::ContactsDialog(QWidget* parent) :
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

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &ContactsDialog::showCard);

    connect(ui->addOrgButton,    &QAbstractButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton,    &QAbstractButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, &ContactsDialog::currentIndexChanged);

    filter = false;

    page = "1";

    go = "default";

    loadContacts();
}

ContactsDialog::~ContactsDialog()
{
    delete ui;
}

/**
 * Получает запрос на обновление списка контактов из классов
 * AddContactDialog, AddOrgContactDialog, ViewContactDialog, ViewOrgContactDialog.
 */
void ContactsDialog::receiveData(bool update)
{
    if (update)
    {
        queryModel->setQuery(queryModel->query().lastQuery());

        go = "default";

        onUpdate();
    }
}

/**
 * Выполняет обработку появления окна.
 */
void ContactsDialog::showEvent(QShowEvent*)
{
    selectionModel = ui->tableView->selectionModel()->selectedRows();

    ui->lineEdit->setFocus();

    go = "default";

    loadContacts();
}

/**
 * Выполняет обработку закрытия окна.
 */
void ContactsDialog::closeEvent(QCloseEvent*)
{
    selectionModel.clear();

    ui->tableView->clearSelection();
    ui->tableView->scrollToTop();

    ui->lineEdit->clear();

    ui->comboBox->setCurrentIndex(0);

    ui->comboBox_list->setCurrentIndex(0);

    page = "1";

    go = "default";
}

/**
 * Выполняет операции для последующего обновления списка контактов.
 */
void ContactsDialog::onUpdate()
{
    selectionModel.clear();

    ui->tableView->clearSelection();

    loadContacts();
}

/**
 * Выполняет открытие окна добавления нового контакта (физ. лицо).
 */
void ContactsDialog::onAddPerson()
{
    addContactDialog = new AddContactDialog;
    connect(addContactDialog, &AddContactDialog::sendData, this, &ContactsDialog::receiveData);
    addContactDialog->show();
    addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна добавления нового контакта (организация).
 */
void ContactsDialog::onAddOrg()
{
    addOrgContactDialog = new AddOrgContactDialog;
    connect(addOrgContactDialog, &AddOrgContactDialog::sendData, this, &ContactsDialog::receiveData);
    addOrgContactDialog->show();
    addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна просмотра данных выбранного контакта.
 */
void ContactsDialog::showCard(const QModelIndex& index)
{
    QString contactId = queryModel->data(queryModel->index(index.row(), 0)).toString();
    qint32 row = ui->tableView->currentIndex().row();

    if (queryModel->data(queryModel->index(row, 1)).toString() == "person")
    {
         viewContactDialog = new ViewContactDialog;
         viewContactDialog->setValues(contactId);
         connect(viewContactDialog, &ViewContactDialog::sendData, this, &ContactsDialog::receiveData);
         viewContactDialog->show();
         viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setValues(contactId);
        connect(viewOrgContactDialog, &ViewOrgContactDialog::sendData, this, &ContactsDialog::receiveData);
        viewOrgContactDialog->show();
        viewOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет удаление объектов класса.
 */
void ContactsDialog::deleteObjects()
{
    if (!queryModel.isNull())
    {
        for (qint32 i = 0; i < widgets.size(); ++i)
            widgets[i]->deleteLater();

        widgets.clear();

        queryModel->deleteLater();
    }
}

/**
 * Выполняет вывод и обновление списка контактов с и без фильтра.
 */
void ContactsDialog::loadContacts()
{
    deleteObjects();

    queryModel = new QSqlQueryModel(this);

    QString queryString = "SELECT entry_id, entry_type, entry_name, GROUP_CONCAT(DISTINCT entry_phone "
                          "ORDER BY entry_id SEPARATOR '\n'), entry_city, entry_address, entry_email, "
                          "entry_vybor_id, entry_comment, entry_employee FROM entry_phone ";

    QString queryCountString = "SELECT COUNT(DISTINCT entry_id) FROM entry_phone ";

    QString searchString;

    if (filter)
    {
        if (ui->comboBox->currentIndex() == 0)
            searchString.append("WHERE entry_name LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
        else if (ui->comboBox->currentIndex() == 1)
            searchString.append("WHERE entry_phone LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
        else if (ui->comboBox->currentIndex() == 2)
            searchString.append("WHERE entry_comment LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
    }

    queryCountString.append(searchString);

    QSqlQuery query(db);

    query.prepare(queryCountString);
    query.exec();
    query.first();

    count = query.value(0).toInt();

    QString pages = ui->label_pages->text();

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

    queryModel->setQuery(queryString);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    queryModel->insertColumn(2);
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Тип"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("ФИО / Название"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Телефон"));
    queryModel->setHeaderData(5, Qt::Horizontal, tr("Город"));
    queryModel->setHeaderData(6, Qt::Horizontal, tr("Адрес"));
    queryModel->setHeaderData(7, Qt::Horizontal, tr("Email"));
    queryModel->setHeaderData(8, Qt::Horizontal, tr("VyborID"));
    queryModel->insertColumn(10);
    queryModel->setHeaderData(10, Qt::Horizontal, tr("Заметка"));
    queryModel->setHeaderData(11, Qt::Horizontal, tr("Менеджер"));

    ui->tableView->setModel(queryModel);

    for (qint32 row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        ui->tableView->setIndexWidget(queryModel->index(row_index, 2), addImageLabel(row_index));

        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(queryModel->data(queryModel->index(row_index, 9)).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addWidgetNote(row_index, true));
        else
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addWidgetNote(row_index, false));
    }

    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(9, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Stretch);
    }

    if (!selectionModel.isEmpty())
        for (qint32 i = 0; i < selectionModel.length(); ++i)
        {
            QModelIndex index = selectionModel.at(i);

            ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
}

/**
 * Выполняет добавление виджета для поля "Тип".
 */
QWidget* ContactsDialog::addImageLabel(qint32 row_index)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* imageLabel = new QLabel(widget);

    layout->addWidget(imageLabel, 0, Qt::AlignCenter);

    if (queryModel->data(queryModel->index(row_index, 1)).toString() == "person")
        imageLabel->setPixmap(QPixmap(":/images/person.png").scaled(30, 30, Qt::KeepAspectRatio));
    else
        imageLabel->setPixmap(QPixmap(":/images/org.png").scaled(30, 30, Qt::KeepAspectRatio));

    widgets.append(widget);

    return widget;
}

/**
 * Выполняет добавление виджета для поля "Заметка".
 */
QWidget* ContactsDialog::addWidgetNote(qint32 row_index, bool url)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* noteLabel = new QLabel(widget);

    layout->addWidget(noteLabel);

    QString note = queryModel->data(queryModel->index(row_index, 9)).toString();

    if (url)
    {
        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
        QStringList hrefs;

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            href.remove(QRegularExpression("[\\,\\.\\;\\:\\'\\\"\\-\\!\\?\\<\\>\\(\\)\\[\\]\\{\\}]+$"));

            hrefs << href;
        }

        note.replace(QRegularExpression("\\n"), QString(" <br> "));

        qint32 index = 0;
        QStringList strList;

        for (qint32 i = 0; i < hrefs.length(); ++i)
        {
            if (i == 0)
                index = note.indexOf(hrefs.at(i));
            else
                index = note.indexOf(hrefs.at(i), index + hrefs.at(i-1).size());

            if (index > 0)
                strList << note.at(index - 1);
            else
                strList << "";
        }

        for (qint32 i = 0; i < hrefs.length(); ++i)
            note.replace(note.indexOf(QRegularExpression("(^| |\\.|\\,|\\(|\\)|\\[|\\]|\\{|\\})" + QRegularExpression::escape(hrefs.at(i)) + "(|$)")),
                         hrefs.at(i).size()+1, QString(strList.at(i) + "<a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a>"));
    }

    noteLabel->setText(note);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    widgets.append(widget);

    return widget;
}

/**
 * Выполняет операции для последующего поиска по списку.
 */
void ContactsDialog::searchFunction()
{
    go = "default";

    if (ui->lineEdit->text().isEmpty())
    {
        if (filter)
            ui->tableView->scrollToTop();

        filter = false;

        onUpdate();

        return;
    }

    filter = true;

    page = "1";

    ui->tableView->scrollToTop();

    onUpdate();
}

/**
 * Выполняет обработку смены количества выводимых контактов на странице.
 */
void ContactsDialog::currentIndexChanged()
{
    go = "default";

    onUpdate();
}

/**
 * Выполняет операции для последующего обновления списка контактов.
 */
void ContactsDialog::on_updateButton_clicked()
{
    go = "default";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void ContactsDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previous";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void ContactsDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "next";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void ContactsDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previousStart";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void ContactsDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "nextEnd";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void ContactsDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    go = "enter";

    onUpdate();
}

/**
 * Выполняет поиск по списку при нажатии клавиши Enter,
 * находясь в поле поиска.
 */
void ContactsDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

/**
 * Выполняет поиск по списку при нажатии кнопки поиска.
 */
void ContactsDialog::on_searchButton_clicked()
{
    searchFunction();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void ContactsDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}
