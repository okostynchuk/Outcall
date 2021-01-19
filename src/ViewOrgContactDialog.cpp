/*
 * Класс служит для просмотра и взаимодействия с данными организации.
 */

#include "ViewOrgContactDialog.h"
#include "ui_ViewOrgContactDialog.h"

#include "AsteriskManager.h"
#include "Global.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QCompleter>

ViewOrgContactDialog::ViewOrgContactDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ViewOrgContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);

    connect(ui->callsTabWidget, &QTabWidget::currentChanged, this, &ViewOrgContactDialog::callTabSelected);
    connect(ui->mainTabWidget,  &QTabWidget::currentChanged, this, &ViewOrgContactDialog::tabSelected);

    connect(ui->comboBox_days,  &QComboBox::currentTextChanged, this, &ViewOrgContactDialog::onUpdate);

    connect(ui->playAudio,         &QAbstractButton::clicked, this, &ViewOrgContactDialog::onPlayAudio);
    connect(ui->callButton,        &QAbstractButton::clicked, this, &ViewOrgContactDialog::onCall);
    connect(ui->editButton,        &QAbstractButton::clicked, this, &ViewOrgContactDialog::onEdit);
    connect(ui->playAudioPhone,    &QAbstractButton::clicked, this, &ViewOrgContactDialog::onPlayAudioPhone);
    connect(ui->openAccessButton,  &QAbstractButton::clicked, this, &ViewOrgContactDialog::onOpenAccess);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &ViewOrgContactDialog::onAddReminder);

    connect(ui->tableView,   &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::showCard);

    connect(ui->tableView_2, &QAbstractItemView::clicked,       this, &ViewOrgContactDialog::getData);
    connect(ui->tableView_2, &QAbstractItemView::doubleClicked, this, &ViewOrgContactDialog::viewNotes);

    if (!g_ordersDbOpened)
        ui->openAccessButton->hide();

    ui->comboBox_list->setVisible(false);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    m_phones = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    QSqlQuery query(m_db);

    query.prepare(QueryStringGetGroups());
    query.exec();

    while(query.next())
    {
        QLineEdit* line = new QLineEdit(this);
        line->setReadOnly(true);
        line->setStyleSheet("*{ background-color: #fffff8;}");

        QLabel* label = new QLabel(this);

        m_managers.insert(query.value(0).toString(), line);
        label->setText(query.value(1).toString() + " (" + query.value(0).toString() + "):");

        ui->gridLayout->addWidget(label);
        ui->gridLayout->addWidget(line);
    }
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    delete ui;
}

/**
 * Получает id контакта из классов ContactsDialog, PopupWindow.
 */
void ViewOrgContactDialog::setValues(const QString& id)
{
    m_contactId = id;

    QSqlQuery query(m_db);

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + m_contactId);
    query.exec();

    while (query.next())
         m_numbers.append(query.value(0).toString());

    for (qint32 i = 0; i < m_numbers.length(); ++i)
        m_phones.at(i)->setText(m_numbers.at(i));

    query.prepare("SELECT group_number, manager_number FROM managers WHERE entry_id = " + m_contactId);
    query.exec();

    while (query.next())
        if (m_managers.keys().contains(query.value(0).toString()))
            m_managers.value(query.value(0).toString())->setText(query.value(1).toString());

    query.prepare("SELECT DISTINCT entry_org_name, entry_region, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = " + m_contactId);
    query.exec();
    query.next();

    ui->orgName->setText(query.value(0).toString());
    ui->region->setText(query.value(1).toString());

    ui->city->setText(query.value(2).toString());
    ui->city->QWidget::setToolTip(query.value(2).toString());
    ui->city->setCursorPosition(0);

    ui->address->setText(query.value(3).toString());
    ui->address->QWidget::setToolTip(query.value(3).toString());
    ui->address->setCursorPosition(0);

    ui->email->setText(query.value(4).toString());
    ui->email->QWidget::setToolTip(query.value(4).toString());
    ui->email->setCursorPosition(0);

    ui->vyborId->setText(query.value(5).toString());
    ui->comment->setText(query.value(6).toString());

    if (ui->vyborId->text() == "0")
        ui->openAccessButton->hide();
}

/**
 * Выполняет обработку смены основной вкладки.
 */
void ViewOrgContactDialog::tabSelected()
{
    if (ui->mainTabWidget->currentWidget()->objectName() == "employees")
        loadEmployees();
    else if (ui->mainTabWidget->currentWidget()->objectName() == "calls")
    {
        m_page = "1";

        onUpdate();
    }
}

/**
 * Выполняет операции для последующего поиска
 * по списку сотрудников организации.
 */
void ViewOrgContactDialog::searchFunction()
{
    if (ui->lineEdit->text().isEmpty())
    {
        if (m_filter)
            ui->tableView->scrollToTop();

        m_filter = false;
    }
    else
    {
        m_filter = true;

        ui->tableView->scrollToTop();
    }

    loadEmployees();
}

/**
 * Выполняет вывод и обновление списка сотрудников организации.
 */
void ViewOrgContactDialog::loadEmployees()
{
    deleteObjects();

    query_model = new QSqlQueryModel;

    QString queryString = "SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment "
                          "FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + m_contactId + "' ";

    if (m_filter)
    {
        switch (ui->comboBox->currentIndex())
        {
        case 0:
            queryString.append("AND ep.entry_name LIKE '%" + ui->lineEdit->text() + "%' ");
            break;
        case 1:
            queryString.append("AND ep.entry_phone LIKE '%" + ui->lineEdit->text() + "%' ");
            break;
        case 2:
            queryString.append("AND ep.entry_comment LIKE '%" + ui->lineEdit->text() + "%' ");
            break;
        }
    }

    queryString.append("GROUP BY ep.entry_id ORDER BY entry_name ASC");

    query_model->setQuery(queryString);

    query_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
    query_model->setHeaderData(1, Qt::Horizontal, tr("ФИО"));
    query_model->setHeaderData(2, Qt::Horizontal, tr("Телефон"));
    query_model->setHeaderData(3, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(query_model);

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
 * Выполняет открытие окна с данными сотрудника организации.
 */
void ViewOrgContactDialog::showCard(const QModelIndex& index)
{
    QString id = query_model->data(query_model->index(index.row(), 0)).toString();

    m_viewContactDialog = new ViewContactDialog;
    m_viewContactDialog->setValues(id);
    connect(m_viewContactDialog, &ViewContactDialog::sendData, this, &ViewOrgContactDialog::receiveDataPerson);
    m_viewContactDialog->show();
    m_viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Получает запрос на обновление списка сотрудников организации.
 */
void ViewOrgContactDialog::receiveDataPerson(bool update)
{
    if (update)
    {
        emit sendData(true);

        loadEmployees();
    }
}

/**
 * Выполняет вывод и обновление истории звонков данной организации.
 */
void ViewOrgContactDialog::loadCalls()
{
    deleteObjects();

    setPage();

    m_queryModel = new QSqlQueryModel(this);

    QString queryString;

    if (ui->callsTabWidget->currentWidget()->objectName() == "allCalls")
    {
        queryString = "SELECT IF(";

        for (qint32 i = 0; i < m_numbers.length(); ++i)
        {
            if (i > 0)
                queryString.append(" || ");

            queryString.append("src = '"+m_numbers[i]+"'");
        }

        queryString.append(", extfield2, extfield1), ");
    }
    else if (ui->callsTabWidget->currentWidget()->objectName() == "placedCalls")
        queryString = "SELECT extfield1, ";
    else
        queryString = "SELECT extfield2, ";

    queryString.append("src, dst, disposition, datetime, uniqueid, recordpath FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + ui->comboBox_days->currentText() + "' DAY) ");

    if (ui->callsTabWidget->currentWidget()->objectName() == "missedCalls")
        queryString.append("AND disposition <> 'ANSWERED' ");
    else if (ui->callsTabWidget->currentWidget()->objectName() == "answeredCalls")
        queryString.append("AND disposition = 'ANSWERED' ");

    queryString.append("AND ( ");

    for (qint32 i = 0; i < m_numbers.length(); ++i)
    {
        if (ui->callsTabWidget->currentWidget()->objectName() == "allCalls")
        {
            if (i == 0)
                queryString.append(" src = '" + m_numbers[i] + "' OR dst = '" + m_numbers[i] + "'");
            else
                queryString.append(" OR src = '" + m_numbers[i] + "' OR dst = '" + m_numbers[i] + "'");
        }
        else if (ui->callsTabWidget->currentWidget()->objectName() == "missedCalls" || ui->callsTabWidget->currentWidget()->objectName() == "answeredCalls")
        {
            if (i == 0)
                queryString.append(" src = '" + m_numbers[i] + "'");
            else
                queryString.append(" OR src = '" + m_numbers[i] + "'");
        }
        else if (ui->callsTabWidget->currentWidget()->objectName() == "placedCalls")
        {
            if (i == 0)
                queryString.append(" dst = '" + m_numbers[i] + "'");
            else
                queryString.append(" OR dst = '" + m_numbers[i] + "'");
        }
    }

    queryString.append(" ) ");

    queryString.append("ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0," + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    else
       queryString.append(QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() -
                                          ui->comboBox_list->currentText().toInt()) + " , " +
                          QString::number(ui->comboBox_list->currentText().toInt()));

    m_queryModel->setQuery(queryString, m_dbCalls);

    m_queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    m_queryModel->setHeaderData(1, Qt::Horizontal, tr("Откуда"));
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("Кому"));
    m_queryModel->insertColumn(4);
    m_queryModel->setHeaderData(4, Qt::Horizontal, tr("Статус"));
    m_queryModel->setHeaderData(5, Qt::Horizontal, tr("Дата и время"));
    m_queryModel->insertColumn(6);
    m_queryModel->setHeaderData(6, Qt::Horizontal, tr("Заметка"));

    ui->tableView_2->setModel(m_queryModel);

    ui->tableView_2->setColumnHidden(3,true);

    if (ui->callsTabWidget->currentWidget()->objectName() == "missedCalls" || ui->callsTabWidget->currentWidget()->objectName() == "anweredCalls")
        ui->tableView_2->setColumnHidden(4, true);

    ui->tableView_2->setColumnHidden(7, true);
    ui->tableView_2->setColumnHidden(8, true);

    for (qint32 row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        QString extfield = m_queryModel->data(m_queryModel->index(row_index, 0)).toString();
        QString src = m_queryModel->data(m_queryModel->index(row_index, 1)).toString();
        QString dst = m_queryModel->data(m_queryModel->index(row_index, 2)).toString();
        QString uniqueid = m_queryModel->data(m_queryModel->index(row_index, 7)).toString();
        QString dialogStatus = m_queryModel->data(m_queryModel->index(row_index, 3)).toString();

        if (extfield.isEmpty())
            ui->tableView_2->setIndexWidget(m_queryModel->index(row_index, 0), loadName(src, dst));

        if (ui->callsTabWidget->currentWidget()->objectName() == "allCalls" || ui->callsTabWidget->currentWidget()->objectName() == "placedCalls")
            ui->tableView_2->setIndexWidget(m_queryModel->index(row_index, 4), loadStatus(dialogStatus));

        QSqlQuery query(m_db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_2->setIndexWidget(m_queryModel->index(row_index, 6), loadNote(uniqueid));

            ui->tableView_2->resizeRowToContents(row_index);
        }
        else
            ui->tableView_2->setRowHeight(row_index, 34);
    }

    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_2->resizeColumnsToContents();

    if (ui->tableView_2->model()->columnCount() != 0)
        ui->tableView_2->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

/**
 * Выполняет установку страницы для перехода.
 */
void ViewOrgContactDialog::setPage()
{
    QString pages = ui->label_pages->text();

    if (m_countRecords <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        qint32 remainder = m_countRecords % ui->comboBox_list->currentText().toInt();

        if (remainder)
            remainder = 1;
        else
            remainder = 0;

        pages = QString::number(m_countRecords / ui->comboBox_list->currentText().toInt() + remainder);
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
    else if (m_go == "default" && m_page == "1")
        m_page = "1";

    ui->lineEdit_page->setText(m_page);
    ui->label_pages->setText(tr("из ") + pages);
}

/**
 * Выполняет установку виджета для поля "Заметка".
 */
QWidget* ViewOrgContactDialog::loadNote(const QString& uniqueid)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* noteLabel = new QLabel(widget);

    layout->addWidget(noteLabel);

    QSqlQuery query(m_db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
    query.exec();
    query.first();

    QString note = query.value(0).toString();

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

    noteLabel->setText(note);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Статус".
 */
QWidget* ViewOrgContactDialog::loadStatus(const QString& dialogStatus)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* statusLabel = new QLabel(widget);

    if (dialogStatus == "NO ANSWER")
        statusLabel->setText(tr("Пропущенный") + " ");
    else if (dialogStatus == "BUSY")
        statusLabel->setText(tr("Занято") + " ");
    else if (dialogStatus == "CANCEL")
        statusLabel->setText(tr("Отколено") + " ");
    else if (dialogStatus == "ANSWERED")
        statusLabel->setText(tr("Принятый") + " ");

    layout->addWidget(statusLabel);
    layout->setContentsMargins(3, 0, 0, 0);

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Имя".
 */
QWidget* ViewOrgContactDialog::loadName(const QString& src, const QString& dst)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* nameLabel = new QLabel(widget);

    qint32 counter = 0;

    for (qint32 i = 0; i < m_numbers.length(); ++i)
    {
        if (src == m_numbers[i])
        {
            nameLabel->setText(dst);
            counter++;
        }
    }

    if (counter == 0)
        nameLabel->setText(src);

    layout->addWidget(nameLabel);
    layout->setContentsMargins(3, 0, 0, 0);

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет удаление объектов класса.
 */
void ViewOrgContactDialog::deleteObjects()
{
    if (ui->mainTabWidget->currentWidget()->objectName() == "employees" && !query_model.isNull())
        query_model->deleteLater();
    else if (ui->mainTabWidget->currentWidget()->objectName() == "calls" && !m_queryModel.isNull())
    {
        for (qint32 i = 0; i < m_widgets.size(); ++i)
            m_widgets[i]->deleteLater();

        m_widgets.clear();

        m_queryModel->deleteLater();
    }
}

/**
 * Выполняет вытягивание значений полей из записи.
 */
void ViewOrgContactDialog::getData(const QModelIndex& index)
{
    m_recordpath = m_queryModel->data(m_queryModel->index(index.row(), 8)).toString();

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    if (!m_recordpath.isEmpty())
    {
        ui->playAudio->setDisabled(false);
        ui->playAudioPhone->setDisabled(false);
    }
}

/**
 * Выполняет операции для последующего обновления истории звонков
 * (с количеством записей).
 */
void ViewOrgContactDialog::onUpdate()
{
     m_go = "default";

     updateCount();
}

/**
 * Выполняет обработку смены вкладки в истории звонков.
 */
void ViewOrgContactDialog::callTabSelected()
{
    ui->tableView_2->setModel(NULL);

    m_page = "1";

    onUpdate();
}

/**
 * Выполняет обновление количества записей в истории звонков.
 */
void ViewOrgContactDialog::updateCount()
{
    QSqlQuery query(m_dbCalls);

    QString queryString = "SELECT COUNT(*) FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + ui->comboBox_days->currentText() + "' DAY) ";

    if (ui->callsTabWidget->currentWidget()->objectName() == "missedCalls")
        queryString.append("AND disposition <> 'ANSWERED' ");
    else if (ui->callsTabWidget->currentWidget()->objectName() == "answeredCalls")
        queryString.append("AND disposition = 'ANSWERED' ");

    queryString.append("AND ( ");

    for (qint32 i = 0; i < m_numbers.length(); ++i)
    {
        if (ui->callsTabWidget->currentWidget()->objectName() == "allCalls")
        {
            if (i == 0)
                queryString.append(" src = '" + m_numbers[i] + "' OR dst = '" + m_numbers[i] + "'");
            else
                queryString.append(" OR src = '" + m_numbers[i] + "' OR dst = '" + m_numbers[i] + "'");
        }
        else if (ui->callsTabWidget->currentWidget()->objectName() == "missedCalls" || ui->callsTabWidget->currentWidget()->objectName() == "answeredCalls")
        {
            if (i == 0)
                queryString.append(" src = '" + m_numbers[i] + "'");
            else
                queryString.append(" OR src = '" + m_numbers[i] + "'");
        }
        else if (ui->callsTabWidget->currentWidget()->objectName() == "placedCalls")
        {
            if (i == 0)
                queryString.append(" dst = '" + m_numbers[i] + "'");
            else
                queryString.append(" OR dst = '" + m_numbers[i] + "'");
        }
    }

    queryString.append(" ) ");

    query.prepare(queryString);
    query.exec();
    query.first();

    m_countRecords = query.value(0).toInt();

    loadCalls();
}

/**
 * Выполняет поиск по списку при нажатии кнопки поиска.
 */
void ViewOrgContactDialog::on_searchButton_clicked()
{
    searchFunction();
}

/**
 * Выполняет поиск по списку при нажатии клавиши Enter,
 * находясь в поле поиска.
 */
void ViewOrgContactDialog::on_lineEdit_returnPressed()
{
    searchFunction();
}

/**
 * Выполняет открытие окна привязки физ. лица к организации.
 */
void ViewOrgContactDialog::on_addPersonToOrg_clicked()
{
    if (!addPersonToOrg.isNull())
        addPersonToOrg->close();

    addPersonToOrg = new AddPersonToOrg;
    addPersonToOrg->setOrgId(m_contactId);
    connect(addPersonToOrg, &AddPersonToOrg::newPerson, this, &ViewOrgContactDialog::loadEmployees);
    addPersonToOrg->show();
    addPersonToOrg->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна с заметками для их просмотра.
 */
void ViewOrgContactDialog::viewNotes(const QModelIndex& index)
{
    QString uniqueid = m_queryModel->data(m_queryModel->index(index.row(), 7)).toString();

    m_notesDialog = new NotesDialog;
    m_notesDialog->setValues(uniqueid, "");
    m_notesDialog->hideAddNote();
    m_notesDialog->show();
    m_notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие базы заказов.
 */
void ViewOrgContactDialog::onOpenAccess()
{
    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray password3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(password3));
    QString port_3 = global::getSettingsValue("port_3", "settings").toString();

    QSqlDatabase dbOrders = QSqlDatabase::addDatabase("QODBC", "Orders");

    dbOrders.setDatabaseName("DRIVER={SQL Server Native Client 10.0};"
                            "Server="+hostName_3+","+port_3+";"
                            "Database="+databaseName_3+";"
                            "Uid="+userName_3+";"
                            "Pwd="+password_3);
    dbOrders.open();

    if (dbOrders.isOpen())
    {
        QString userId = global::getSettingsValue("user_login", "settings").toString();

        QSqlQuery query(dbOrders);

        query.prepare("INSERT INTO CallTable (UserID, ClientID)"
                    "VALUES (?, ?)");
        query.addBindValue(userId);
        query.addBindValue(ui->vyborId->text().toInt());
        query.exec();

        ui->openAccessButton->setDisabled(true);

        dbOrders.close();
    }
    else
        MsgBoxError(tr("Отсутствует подключение к базе заказов!"));
}

/**
 * Выполняет открытие окна с медиапроигрывателем для прослушивания записи звонка.
 */
void ViewOrgContactDialog::onPlayAudio()
{
    if (ui->tableView_2->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

        return;
    }

    if (!m_recordpath.isEmpty())
    {
        if (!m_player.isNull())
            m_player->close();

        m_player = new Player;
        m_player->openMedia(m_recordpath);
        m_player->show();
        m_player->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет операции для последующего прослушивания записи звонка через телефон.
 */
void ViewOrgContactDialog::onPlayAudioPhone()
{
    if (ui->tableView_2->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

        return;
    }

    if (!m_recordpath.isEmpty())
    {
        QString protocol = global::getSettingsValue(g_personalNumber, "extensions").toString();

        g_asteriskManager->originateAudio(g_personalNumber, protocol, m_recordpath);
    }
}

/**
 * Выполняет операции для последующего выбора номера контакта и совершения звонка.
 */
void ViewOrgContactDialog::onCall()
{
    if (m_numbers.count() == 1)
    {
        QString number = m_numbers.at(0);
        QString protocol = global::getSettingsValue(g_personalNumber, "extensions").toString();

        g_asteriskManager->originateCall(g_personalNumber, number, protocol, g_personalNumber);
    }
    else
    {
        if (!m_chooseNumber.isNull())
            m_chooseNumber->close();

        m_chooseNumber = new ChooseNumber;
        m_chooseNumber->setValues(m_numbers);
        m_chooseNumber->show();
        m_chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет скрытие текущего окна и открытие окна редактирования.
 */
void ViewOrgContactDialog::onEdit()
{
    hide();

    m_editOrgContactDialog = new EditOrgContactDialog;
    m_editOrgContactDialog->setValues(m_contactId);
    connect(m_editOrgContactDialog, &EditOrgContactDialog::sendData, this, &ViewOrgContactDialog::receiveDataOrg);
    connect(this, &ViewOrgContactDialog::getPos, m_editOrgContactDialog, &EditOrgContactDialog::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    m_editOrgContactDialog->show();
    m_editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void ViewOrgContactDialog::onAddReminder()
{
    if (!m_addReminderDialog.isNull())
        m_addReminderDialog->close();

    m_addReminderDialog = new AddReminderDialog;
    m_addReminderDialog->setCallId(m_contactId);
    m_addReminderDialog->show();
    m_addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void ViewOrgContactDialog::on_previousButton_clicked()
{
    ui->tableView_2->scrollToTop();

    m_go = "previous";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void ViewOrgContactDialog::on_nextButton_clicked()
{
    ui->tableView_2->scrollToTop();

    m_go = "next";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void ViewOrgContactDialog::on_previousStartButton_clicked()
{
    ui->tableView_2->scrollToTop();

    m_go = "previousStart";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void ViewOrgContactDialog::on_nextEndButton_clicked()
{
    ui->tableView_2->scrollToTop();

    m_go = "nextEnd";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void ViewOrgContactDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView_2->scrollToTop();

    m_go = "enter";

    loadCalls();
}

/**
 * Получает запрос на обновление состояния окна.
 */
void ViewOrgContactDialog::receiveDataOrg(bool update, qint32 x, qint32 y)
{
    qint32 desktopHeight;
    qint32 desktopWidth;
    qint32 widgetHeight = QWidget::height();
    qint32 widgetWidth = QWidget::width();

    QDesktopWidget desktopWidget;
    QRect desktop = desktopWidget.availableGeometry(this);

    desktopWidth = desktop.width();
    desktopHeight = desktop.height();

    if (update)
    {
        emit sendData(true);

        close();
    }
    else
    {
        if (x < 0 && (desktopHeight - y) > widgetHeight)
        {
            x = 0;
            this->move(x, y);
        }
        else if (x < 0 && ((desktopHeight - y) < widgetHeight))
        {
            x = 0;
            y = widgetHeight;
            this->move(x, y);
        }
        else if ((desktopWidth - x) < widgetWidth && (desktopHeight - y) > widgetHeight)
        {
            x = widgetWidth * 0.9;
            this->move(x, y);
        }
        else if ((desktopWidth - x) < widgetWidth && ((desktopHeight - y) < widgetHeight))
        {
            x = widgetWidth * 0.9;
            y = widgetHeight * 0.9;
            this->move(x, y);
        }
        else if (x > 0 && ((desktopHeight - y) < widgetHeight))
        {
            y = widgetHeight * 0.9;
            this->move(x, y);
        }
        else
        {
            this->move(x, y);
        }

        show();
    }
}
