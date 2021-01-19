/*
 * Класс служит для вывода списка всех контактов и различного взаимодействия с ним.
 */

#include "ContactsDialog.h"
#include "ui_ContactsDialog.h"

#include <QDesktopWidget>

ContactsDialog::ContactsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ContactsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    m_geometry = saveGeometry();

    QRegularExpression regExp("^[0-9]*$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &ContactsDialog::showCard);

    connect(ui->addOrgButton,    &QAbstractButton::clicked, this, &ContactsDialog::onAddOrg);
    connect(ui->updateButton,    &QAbstractButton::clicked, this, &ContactsDialog::onUpdate);
    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &ContactsDialog::onAddPerson);
    connect(ui->comboBox_list, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, &ContactsDialog::currentIndexChanged);

    m_filter = false;

    m_page = "1";

    m_go = "default";

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
        m_queryModel->setQuery(m_queryModel->query().lastQuery());

        m_go = "default";

        onUpdate();
    }
}

/**
 * Выполняет обработку появления окна.
 */
void ContactsDialog::showEvent(QShowEvent*)
{
    m_selections = ui->tableView->selectionModel()->selectedRows();

    ui->lineEdit->setFocus();

    m_go = "default";

    loadContacts();
}

/**
 * Выполняет обработку закрытия окна.
 */
void ContactsDialog::closeEvent(QCloseEvent*)
{
    hide();

    m_selections.clear();

    ui->tableView->clearSelection();
    ui->tableView->scrollToTop();

    ui->lineEdit->clear();

    ui->comboBox->setCurrentIndex(0);

    ui->comboBox_list->setCurrentIndex(0);

    m_page = "1";

    m_go = "default";

    restoreGeometry(m_geometry);

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);

    move(screen.center() - rect().center());
}

/**
 * Выполняет операции для последующего обновления списка контактов.
 */
void ContactsDialog::onUpdate()
{
    m_selections.clear();

    ui->tableView->clearSelection();

    loadContacts();
}

/**
 * Выполняет открытие окна добавления нового контакта (физ. лицо).
 */
void ContactsDialog::onAddPerson()
{
    m_addContactDialog = new AddContactDialog;
    connect(m_addContactDialog, &AddContactDialog::sendData, this, &ContactsDialog::receiveData);
    m_addContactDialog->show();
    m_addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна добавления нового контакта (организация).
 */
void ContactsDialog::onAddOrg()
{
    m_addOrgContactDialog = new AddOrgContactDialog;
    connect(m_addOrgContactDialog, &AddOrgContactDialog::sendData, this, &ContactsDialog::receiveData);
    m_addOrgContactDialog->show();
    m_addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна просмотра данных выбранного контакта.
 */
void ContactsDialog::showCard(const QModelIndex& index)
{
    QString contactId = m_queryModel->data(m_queryModel->index(index.row(), 0)).toString();
    qint32 row = ui->tableView->currentIndex().row();

    if (m_queryModel->data(m_queryModel->index(row, 1)).toString() == "person")
    {
         m_viewContactDialog = new ViewContactDialog;
         m_viewContactDialog->setValues(contactId);
         connect(m_viewContactDialog, &ViewContactDialog::sendData, this, &ContactsDialog::receiveData);
         m_viewContactDialog->show();
         m_viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        m_viewOrgContactDialog = new ViewOrgContactDialog;
        m_viewOrgContactDialog->setValues(contactId);
        connect(m_viewOrgContactDialog, &ViewOrgContactDialog::sendData, this, &ContactsDialog::receiveData);
        m_viewOrgContactDialog->show();
        m_viewOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет удаление объектов класса.
 */
void ContactsDialog::deleteObjects()
{
    if (!m_queryModel.isNull())
    {
        for (qint32 i = 0; i < m_widgets.size(); ++i)
            m_widgets[i]->deleteLater();

        m_widgets.clear();

        m_queryModel->deleteLater();
    }
}

/**
 * Выполняет вывод и обновление списка контактов с и без фильтра.
 */
void ContactsDialog::loadContacts()
{
    deleteObjects();

    m_queryModel = new QSqlQueryModel(this);

    QString queryString = "SELECT e.entry_id, e.entry_type, e.entry_name, GROUP_CONCAT(DISTINCT e.entry_phone "
                          "ORDER BY e.entry_id SEPARATOR '\n'), e.entry_region, e.entry_city, e.entry_address, e.entry_email, "
                          "e.entry_vybor_id, e.entry_comment, (SELECT GROUP_CONCAT(manager_number SEPARATOR '\n') FROM managers m "
                          "WHERE m.entry_id = e.entry_id) FROM entry_phone e ";

    QString queryCountString = "SELECT COUNT(DISTINCT e.entry_id) FROM entry_phone e ";

    QString searchString;

    if (m_filter)
    {
        switch (ui->comboBox->currentIndex())
        {
        case 0:
            searchString.append("WHERE e.entry_name LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
            break;
        case 1:
            searchString.append("WHERE e.entry_phone LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
            break;
        case 2:
            searchString.append("WHERE e.entry_region LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
            break;
        case 3:
            searchString.append("WHERE e.entry_comment LIKE '%" + ui->lineEdit->text().replace(QRegularExpression("\'"), "\'\'") + "%' ");
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
    queryString.append("GROUP BY e.entry_id ORDER BY e.entry_name ASC LIMIT ");

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
    m_queryModel->insertColumn(2);
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("Тип"));
    m_queryModel->setHeaderData(3, Qt::Horizontal, tr("ФИО / Название"));
    m_queryModel->setHeaderData(4, Qt::Horizontal, tr("Телефон"));
    m_queryModel->setHeaderData(5, Qt::Horizontal, tr("Область"));
    m_queryModel->setHeaderData(6, Qt::Horizontal, tr("Город"));
    m_queryModel->setHeaderData(7, Qt::Horizontal, tr("Адрес"));
    m_queryModel->setHeaderData(8, Qt::Horizontal, tr("Email"));
    m_queryModel->setHeaderData(9, Qt::Horizontal, tr("VyborID"));
    m_queryModel->insertColumn(11);
    m_queryModel->setHeaderData(11, Qt::Horizontal, tr("Заметка"));
    m_queryModel->setHeaderData(12, Qt::Horizontal, tr("Менеджеры"));

    ui->tableView->setModel(m_queryModel);

    for (qint32 row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        ui->tableView->setIndexWidget(m_queryModel->index(row_index, 2), addImageLabel(row_index));

        QRegularExpressionMatchIterator hrefIterator = m_hrefRegExp.globalMatch(m_queryModel->data(m_queryModel->index(row_index, 10)).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 11), addWidgetNote(row_index, true));
        else
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 11), addWidgetNote(row_index, false));
    }

    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(10, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Stretch);
    }

    if (!m_selections.isEmpty())
        for (qint32 i = 0; i < m_selections.length(); ++i)
        {
            QModelIndex index = m_selections.at(i);

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

    if (m_queryModel->data(m_queryModel->index(row_index, 1)).toString() == "person")
        imageLabel->setPixmap(QPixmap(":/images/person.png").scaled(30, 30, Qt::KeepAspectRatio));
    else
        imageLabel->setPixmap(QPixmap(":/images/org.png").scaled(30, 30, Qt::KeepAspectRatio));

    m_widgets.append(widget);

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

    QString note = m_queryModel->data(m_queryModel->index(row_index, 10)).toString();

    if (url)
    {
        QRegularExpressionMatchIterator hrefIterator = m_hrefRegExp.globalMatch(note);
        QStringList hrefs, hrefsNoCharacters, hrefsReplaceCharacters;

        note.replace("<", "&lt;").replace(">", "&gt;");

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            hrefs << href;
            href.remove(QRegularExpression("[\\,\\.\\;\\:\\'\\\"\\-\\!\\?\\^\\`\\~\\*\\№\\%\\&\\$\\#\\<\\>\\(\\)\\[\\]\\{\\}]+$"));
            hrefsNoCharacters << href;
        }

        QStringList firstCharList, lastCharList;

        for (qint32 i = 0; i < hrefs.length(); ++i)
        {
            QString hrefReplaceCharacters = QString(hrefs.at(i)).replace("<", "&lt;").replace(">", "&gt;");
            hrefsReplaceCharacters << hrefReplaceCharacters;
            hrefReplaceCharacters = hrefReplaceCharacters.remove(hrefsNoCharacters.at(i));

            if (hrefReplaceCharacters.isEmpty())
                lastCharList << " ";
            else
                lastCharList << hrefReplaceCharacters;
        }

        note.replace(QRegularExpression("\\n"), QString(" <br> "));

        qint32 index = 0;

        for (qint32 i = 0; i < hrefsReplaceCharacters.length(); ++i)
        {
            if (i == 0)
                index = note.indexOf(hrefsReplaceCharacters.at(i));
            else
                index = note.indexOf(hrefsReplaceCharacters.at(i), index + hrefsReplaceCharacters.at(i - 1).size());

            if (index > 0)
                firstCharList << note.at(index - 1);
            else
                firstCharList << "";
        }

        for (qint32 i = 0; i < hrefs.length(); ++i)
        {
            qint32 size;

            if (firstCharList.at(i) == "")
                size = hrefsReplaceCharacters.at(i).size();
            else
                size = hrefsReplaceCharacters.at(i).size() + 1;

            note.replace(note.indexOf(QRegularExpression("( |^|\\^|\\.|\\,|\\(|\\)|\\[|\\]|\\{|\\}|\\;|\\'|\\\"|[a-zA-Z0-9а-яА-Я]|\\`|\\~|\\%|\\$|\\#|\\№|\\@|\\&|\\/|\\\\|\\!|\\*)" + QRegularExpression::escape(hrefsReplaceCharacters.at(i)) + "( |$)")),
                        size, QString(firstCharList.at(i) + "<a href='" + hrefsNoCharacters.at(i) + "'>" + hrefsNoCharacters.at(i) + "</a>" + lastCharList.at(i)));
        }
    }

    noteLabel->setText(note);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет операции для последующего поиска по списку.
 */
void ContactsDialog::searchFunction()
{
    m_go = "default";

    if (ui->lineEdit->text().isEmpty())
    {
        if (m_filter)
            ui->tableView->scrollToTop();

        m_filter = false;

        onUpdate();

        return;
    }

    m_filter = true;

    m_page = "1";

    ui->tableView->scrollToTop();

    onUpdate();
}

/**
 * Выполняет обработку смены количества выводимых контактов на странице.
 */
void ContactsDialog::currentIndexChanged()
{
    m_go = "default";

    onUpdate();
}

/**
 * Выполняет операции для последующего обновления списка контактов.
 */
void ContactsDialog::on_updateButton_clicked()
{
    m_go = "default";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void ContactsDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previous";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void ContactsDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "next";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void ContactsDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previousStart";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void ContactsDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "nextEnd";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void ContactsDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    m_go = "enter";

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
