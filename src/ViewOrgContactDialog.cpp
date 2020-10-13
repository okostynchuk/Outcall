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

ViewOrgContactDialog::ViewOrgContactDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ViewOrgContactDialog)
{
    ui->setupUi(this);

    userId = global::getSettingsValue("user_login", "settings").toString();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tabWidget,   &QTabWidget::currentChanged, this, &ViewOrgContactDialog::callTabSelected);
    connect(ui->tabWidget_2, &QTabWidget::currentChanged, this, &ViewOrgContactDialog::tabSelected);

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

    my_number = global::getExtensionNumber("extensions");

    if (!ordersDbOpened)
        ui->openAccessButton->hide();

    ui->comboBox_list->setVisible(false);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    phonesList = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    deleteObjects();

    delete ui;
}

/**
 * Получает id контакта из классов ContactsDialog, PopupWindow.
 */
void ViewOrgContactDialog::setValues(const QString& id)
{
    contactId = id;

    QSqlQuery query(db);

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + contactId);
    query.exec();

    while (query.next())
         numbersList.append(query.value(0).toString());

    for (qint32 i = 0; i < numbersList.length(); ++i)
        phonesList.at(i)->setText(numbersList.at(i));

    query.prepare("SELECT DISTINCT entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = " + contactId);
    query.exec();
    query.next();

    ui->orgName->setText(query.value(0).toString());

    ui->city->setText(query.value(1).toString());
    ui->city->QWidget::setToolTip(query.value(1).toString());
    ui->city->setCursorPosition(0);

    ui->address->setText(query.value(2).toString());
    ui->address->QWidget::setToolTip(query.value(2).toString());
    ui->address->setCursorPosition(0);

    ui->email->setText(query.value(3).toString());
    ui->email->QWidget::setToolTip(query.value(3).toString());
    ui->email->setCursorPosition(0);

    ui->vyborId->setText(query.value(4).toString());
    ui->comment->setText(query.value(5).toString());

    if (ui->vyborId->text() == "0")
        ui->openAccessButton->hide();
}

/**
 * Выполняет обработку смены основной вкладки.
 */
void ViewOrgContactDialog::tabSelected()
{
    if (ui->tabWidget_2->currentIndex() == 1)
        onUpdateEmployees();
    else if (ui->tabWidget_2->currentIndex() == 2)
    {
        page = "1";

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
        if (filter)
            ui->tableView->scrollToTop();

        filter = false;
    }
    else
    {
        filter = true;

        ui->tableView->scrollToTop();
    }

    onUpdateEmployees();
}

/**
 * Выполняет вывод и обновление списка сотрудников организации.
 */
void ViewOrgContactDialog::onUpdateEmployees()
{
    query_model = new QSqlQueryModel;

    QString queryString = "SELECT ep.entry_id, ep.entry_name, GROUP_CONCAT(DISTINCT ep.entry_phone ORDER BY ep.entry_id SEPARATOR '\n'), ep.entry_comment "
                          "FROM entry_phone ep WHERE ep.entry_type = 'person' AND ep.entry_person_org_id = '" + contactId + "' ";

    if (filter)
    {
        if (ui->comboBox->currentIndex() == 0)
             queryString.append("AND ep.entry_name LIKE '%" + ui->lineEdit->text() + "%' ");
        else if (ui->comboBox->currentIndex() == 1)
            queryString.append("AND ep.entry_phone LIKE '%" + ui->lineEdit->text() + "%' ");
        else if (ui->comboBox->currentIndex() == 2)
            queryString.append("AND ep.entry_comment LIKE '%" + ui->lineEdit->text() + "%' ");
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

    viewContactDialog = new ViewContactDialog;
    viewContactDialog->setValues(id);
    connect(viewContactDialog, &ViewContactDialog::sendData, this, &ViewOrgContactDialog::receiveDataPerson);
    viewContactDialog->show();
    viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Получает запрос на обновление списка сотрудников организации.
 */
void ViewOrgContactDialog::receiveDataPerson(bool update)
{
    if (update)
    {
        emit sendData(true);

        onUpdateEmployees();
    }
}

/**
 * Выполняет вывод и обновление истории звонков данной организации.
 */
void ViewOrgContactDialog::loadCalls()
{
    deleteObjects();

    queryModel = new QSqlQueryModel;

    queries.append(queryModel);

    setPage();

    QString queryString;

    if (ui->tabWidget->currentIndex() == 0)
    {
        queryString = "SELECT IF(";

        for (qint32 i = 0; i < numbersList.length(); ++i)
        {
            if (i > 0)
                queryString.append(" || ");

            queryString.append("src = '"+numbersList[i]+"'");
        }

        queryString.append(", extfield2, extfield1), ");
    }
    else if (ui->tabWidget->currentIndex() == 3)
        queryString = "SELECT extfield1, ";
    else
        queryString = "SELECT extfield2, ";

    queryString.append("src, dst, disposition, datetime, uniqueid, recordpath FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + ui->comboBox_days->currentText() + "' DAY) ");

    if (ui->tabWidget->currentIndex() == 0)
        queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') ");
    else if (ui->tabWidget->currentIndex() == 1)
        queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') ");
    else if (ui->tabWidget->currentIndex() == 2)
        queryString.append("AND disposition = 'ANSWERED' ");

    queryString.append("AND ( ");

    for (qint32 i = 0; i < numbersList.length(); ++i)
    {
        if (ui->tabWidget->currentIndex() == 0)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
        }
        else if (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");
        }
        else if (ui->tabWidget->currentIndex() == 3)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "'");
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

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Кому"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Статус"));
    queryModel->setHeaderData(5, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(6);
    queryModel->setHeaderData(6, Qt::Horizontal, tr("Заметка"));

    ui->tableView_2->setModel(queryModel);

    ui->tableView_2->setColumnHidden(3,true);

    if (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2)
        ui->tableView_2->setColumnHidden(4, true);

    ui->tableView_2->setColumnHidden(7, true);
    ui->tableView_2->setColumnHidden(8, true);

    for (qint32 row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        QString extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        QString src = queryModel->data(queryModel->index(row_index, 1)).toString();
        QString dst = queryModel->data(queryModel->index(row_index, 2)).toString();
        QString uniqueid = queryModel->data(queryModel->index(row_index, 7)).toString();
        QString dialogStatus = queryModel->data(queryModel->index(row_index, 3)).toString();

        if (extfield.isEmpty())
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 0), loadName(src, dst));

        if (ui->tabWidget->currentIndex() == 0 || ui->tabWidget->currentIndex() == 3)
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 4), loadStatus(dialogStatus));

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 6), loadNote(uniqueid));

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

    if (countRecords <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        qint32 remainder = countRecords % ui->comboBox_list->currentText().toInt();

        if (remainder)
            remainder = 1;
        else
            remainder = 0;

        pages = QString::number(countRecords / ui->comboBox_list->currentText().toInt() + remainder);
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
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages->setText(tr("из ") + pages);
}

/**
 * Выполняет установку виджета для поля "Заметка".
 */
QWidget* ViewOrgContactDialog::loadNote(const QString& uniqueid)
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* noteLabel = new QLabel(wgt);

    layout->addWidget(noteLabel);

    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
    query.exec();
    query.first();

    QString note = query.value(0).toString();

    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);

    if (hrefIterator.hasNext())
    {
        QStringList hrefs;

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            hrefs << href;
        }

        note.replace(QRegularExpression("\\n"), QString(" <br> "));

        for (qint32 i = 0; i < hrefs.length(); ++i)
            note.replace(QRegularExpression("(^| )" + QRegularExpression::escape(hrefs.at(i)) + "( |$)"), QString(" <a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a> "));
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

/**
 * Выполняет установку виджета для поля "Статус".
 */
QWidget* ViewOrgContactDialog::loadStatus(const QString& dialogStatus)
{
    QHBoxLayout* statusLayout = new QHBoxLayout;
    QWidget* statusWgt = new QWidget;
    QLabel* statusLabel = new QLabel(statusWgt);

    if (dialogStatus == "NO ANSWER")
        statusLabel->setText(tr("Пропущенный "));
    else if (dialogStatus == "BUSY")
        statusLabel->setText(tr("Занято "));
    else if (dialogStatus == "CANCEL")
        statusLabel->setText(tr("Отклонено "));
    else if (dialogStatus == "ANSWERED")
        statusLabel->setText(tr("Принятый "));

    statusLayout->addWidget(statusLabel);

    statusLayout->setContentsMargins(3, 0, 0, 0);

    statusWgt->setLayout(statusLayout);

    layouts.append(statusLayout);
    widgets.append(statusWgt);
    labels.append(statusLabel);

    return statusWgt;
}

/**
 * Выполняет установку виджета для поля "Имя".
 */
QWidget* ViewOrgContactDialog::loadName(const QString& src, const QString& dst)
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);

    qint32 counter = 0;

    for (qint32 i = 0; i < numbersList.length(); ++i)
    {
        if (src == numbersList[i])
        {
            nameLabel->setText(dst);
            counter++;
        }
    }

    if (counter == 0)
        nameLabel->setText(src);

    nameLayout->addWidget(nameLabel);

    nameLayout->setContentsMargins(3, 0, 0, 0);

    nameWgt->setLayout(nameLayout);

    layouts.append(nameLayout);
    widgets.append(nameWgt);
    labels.append(nameLabel);

    return nameWgt;
}

/**
 * Выполняет удаление объектов класса.
 */
void ViewOrgContactDialog::deleteObjects()
{
    if (!widgets.isEmpty())
        for (qint32 i = 0; i < widgets.size(); ++i)
            widgets[i]->deleteLater();

    if (!layouts.isEmpty())
        for (qint32 i = 0; i < layouts.size(); ++i)
            layouts[i]->deleteLater();

    if (!labels.isEmpty())
        for (qint32 i = 0; i < labels.size(); ++i)
            labels[i]->deleteLater();

    if (!queries.isEmpty())
        for (qint32 i = 0; i < queries.size(); ++i)
            queries[i]->deleteLater();

    widgets.clear();
    layouts.clear();
    labels.clear();
    queries.clear();
}

/**
 * Выполняет вытягивание значений полей из записи.
 */
void ViewOrgContactDialog::getData(const QModelIndex& index)
{
    recordpath = queryModel->data(queryModel->index(index.row(), 8)).toString();

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    if (!recordpath.isEmpty())
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
     go = "default";

     updateCount();
}

/**
 * Выполняет обработку смены вкладки в истории звонков.
 */
void ViewOrgContactDialog::callTabSelected()
{
    ui->tableView_2->setModel(NULL);

    page = "1";

    onUpdate();
}

/**
 * Выполняет обновление количества записей в истории звонков.
 */
void ViewOrgContactDialog::updateCount()
{
    QSqlQuery query(dbCalls);

    QString queryString = "SELECT COUNT(*) FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + ui->comboBox_days->currentText() + "' DAY) ";

    if (ui->tabWidget->currentIndex() == 0)
        queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') ");
    else if (ui->tabWidget->currentIndex() == 1)
        queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') ");
    else if (ui->tabWidget->currentIndex() == 2)
        queryString.append("AND disposition = 'ANSWERED' ");

    queryString.append("AND ( ");

    for (qint32 i = 0; i < numbersList.length(); ++i)
    {
        if (ui->tabWidget->currentIndex() == 0)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
        }
        else if (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");
        }
        else if (ui->tabWidget->currentIndex() == 3)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "'");
        }
    }

    queryString.append(" ) ");

    query.prepare(queryString);
    query.exec();
    query.first();

    countRecords = query.value(0).toInt();

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
        addPersonToOrg.data()->close();

    addPersonToOrg = new AddPersonToOrg;
    addPersonToOrg.data()->setOrgId(contactId);
    connect(addPersonToOrg.data(), &AddPersonToOrg::newPerson, this, &ViewOrgContactDialog::onUpdateEmployees);
    addPersonToOrg.data()->show();
    addPersonToOrg.data()->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна с заметками для их просмотра.
 */
void ViewOrgContactDialog::viewNotes(const QModelIndex& index)
{
    QString uniqueid = queryModel->data(queryModel->index(index.row(), 7)).toString();

    notesDialog = new NotesDialog;
    notesDialog->setValues(uniqueid, "");
    notesDialog->hideAddNote();
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
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
        QMessageBox::critical(this, tr("Ошибка"), tr("Отсутствует подключение к базе заказов!"), QMessageBox::Ok);
}

/**
 * Выполняет открытие окна с медиапроигрывателем для прослушивания записи звонка.
 */
void ViewOrgContactDialog::onPlayAudio()
{
    if (ui->tableView_2->selectionModel()->selectedRows().count() != 1)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Выберите одну запись!"), QMessageBox::Ok);

        return;
    }

    if (!recordpath.isEmpty())
    {
        if (!playAudioDialog.isNull())
            playAudioDialog.data()->close();

        playAudioDialog = new PlayAudioDialog;
        playAudioDialog.data()->openMedia(recordpath);
        playAudioDialog.data()->show();
        playAudioDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет операции для последующего прослушивания записи звонка через телефон.
 */
void ViewOrgContactDialog::onPlayAudioPhone()
{
    if (ui->tableView_2->selectionModel()->selectedRows().count() != 1)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Выберите одну запись!"), QMessageBox::Ok);

        return;
    }

    if (!recordpath.isEmpty())
    {
        QString protocol = global::getSettingsValue(my_number, "extensions").toString();

        g_pAsteriskManager->originateAudio(my_number, protocol, recordpath);
    }
}

/**
 * Выполняет операции для последующего выбора номера контакта и совершения звонка.
 */
void ViewOrgContactDialog::onCall()
{
    QSqlQuery query(db);

    query.prepare("SELECT fone FROM fones WHERE entry_id = ?");
    query.addBindValue(contactId);
    query.exec();

    if (query.size() == 1)
    {
        query.next();

        QString number = query.value(0).toString();
        QString protocol = global::getSettingsValue(my_number, "extensions").toString();

        g_pAsteriskManager->originateCall(my_number, number, protocol, my_number);
    }
    else
    {
        if (!chooseNumber.isNull())
            chooseNumber.data()->close();

        chooseNumber = new ChooseNumber;
        chooseNumber.data()->setValues(contactId);
        chooseNumber.data()->show();
        chooseNumber.data()->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет скрытие текущего окна и открытие окна редактирования.
 */
void ViewOrgContactDialog::onEdit()
{
    hide();

    editOrgContactDialog = new EditOrgContactDialog;
    editOrgContactDialog->setValues(contactId);
    connect(editOrgContactDialog, &EditOrgContactDialog::sendData, this, &ViewOrgContactDialog::receiveDataOrg);
    connect(this, &ViewOrgContactDialog::getPos, editOrgContactDialog, &EditOrgContactDialog::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    editOrgContactDialog->show();
    editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void ViewOrgContactDialog::onAddReminder()
{
    if (!addReminderDialog.isNull())
        addReminderDialog.data()->close();

    addReminderDialog = new AddReminderDialog;
    addReminderDialog.data()->setCallId(contactId);
    addReminderDialog.data()->show();
    addReminderDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void ViewOrgContactDialog::on_previousButton_clicked()
{
    ui->tableView_2->scrollToTop();

    go = "previous";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void ViewOrgContactDialog::on_nextButton_clicked()
{
    ui->tableView_2->scrollToTop();

    go = "next";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void ViewOrgContactDialog::on_previousStartButton_clicked()
{
    ui->tableView_2->scrollToTop();

    go = "previousStart";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void ViewOrgContactDialog::on_nextEndButton_clicked()
{
    ui->tableView_2->scrollToTop();

    go = "nextEnd";

    loadCalls();;
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void ViewOrgContactDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView_2->scrollToTop();

    go = "enter";

    loadCalls();
}

/**
 * Получает запрос на обновление состояния окна.
 */
void ViewOrgContactDialog::receiveDataOrg(bool update, qint32 x, qint32 y)
{
    qint32 nDesktopHeight;
    qint32 nDesktopWidth;
    qint32 nWidgetHeight = QWidget::height();
    qint32 nWidgetWidth = QWidget::width();

    QDesktopWidget desktop;
    QRect rcDesktop = desktop.availableGeometry(this);

    nDesktopWidth = rcDesktop.width();
    nDesktopHeight = rcDesktop.height();

    if (update)
    {
        emit sendData(true);

        close();
    }
    else
    {
        if (x < 0 && (nDesktopHeight - y) > nWidgetHeight)
        {
            x = 0;
            this->move(x, y);
        }
        else if (x < 0 && ((nDesktopHeight - y) < nWidgetHeight))
        {
            x = 0;
            y = nWidgetHeight;
            this->move(x, y);
        }
        else if ((nDesktopWidth - x) < nWidgetWidth && (nDesktopHeight - y) > nWidgetHeight)
        {
            x = nWidgetWidth * 0.9;
            this->move(x, y);
        }
        else if ((nDesktopWidth - x) < nWidgetWidth && ((nDesktopHeight - y) < nWidgetHeight))
        {
            x = nWidgetWidth * 0.9;
            y = nWidgetHeight * 0.9;
            this->move(x, y);
        }
        else if (x > 0 && ((nDesktopHeight - y) < nWidgetHeight))
        {
            y = nWidgetHeight * 0.9;
            this->move(x, y);
        }
        else
        {
            this->move(x, y);
        }

        show();
    }
}
