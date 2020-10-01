/*
 * Класс служит для просмотра и взаимодействия с данными физ. лица.
 */

#include "ViewContactDialog.h"
#include "ui_ViewContactDialog.h"

#include "AsteriskManager.h"
#include "Global.h"

#include <QMessageBox>
#include <QDesktopWidget>

ViewContactDialog::ViewContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewContactDialog)
{
    ui->setupUi(this);

    userId = global::getSettingsValue("user_login", "settings").toString();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->tabWidget, &QTabWidget::currentChanged,    this, &ViewContactDialog::tabSelected);
    connect(ui->comboBox_days,  &QComboBox::currentTextChanged, this, &ViewContactDialog::onUpdate);

    connect(ui->playAudio,         &QAbstractButton::clicked, this, &ViewContactDialog::onPlayAudio);
    connect(ui->editButton,        &QAbstractButton::clicked, this, &ViewContactDialog::onEdit);
    connect(ui->callButton,        &QAbstractButton::clicked, this, &ViewContactDialog::onCall);
    connect(ui->playAudioPhone,    &QAbstractButton::clicked, this, &ViewContactDialog::onPlayAudioPhone);
    connect(ui->openAccessButton,  &QAbstractButton::clicked, this, &ViewContactDialog::onOpenAccess);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &ViewContactDialog::onAddReminder);

    connect(ui->tableView, &QAbstractItemView::clicked,       this, &ViewContactDialog::getData);
    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &ViewContactDialog::viewNotes);

    my_number = global::getExtensionNumber("extensions");

    if (!MSSQLopened)
        ui->openAccessButton->hide();

    ui->comboBox_list->setVisible(false);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    phonesList = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };
}

ViewContactDialog::~ViewContactDialog()
{
    deleteObjects();

    delete ui;
}

/**
 * Выполняет обработку появления окна.
 */
void ViewContactDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QSqlQuery query(db);

    query.prepare("SELECT DISTINCT entry_org_name FROM entry WHERE id = "
                  "(SELECT DISTINCT entry_person_org_id FROM entry WHERE id = " + contactId + ")");
    query.exec();

    if (query.first())
        ui->organization->setText(query.value(0).toString());
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void ViewContactDialog::onAddReminder()
{
    if (!addReminderDialog.isNull())
        addReminderDialog.data()->close();

    addReminderDialog = new AddReminderDialog;
    addReminderDialog.data()->setCallId(contactId);
    addReminderDialog.data()->show();
    addReminderDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие базы заказов.
 */
void ViewContactDialog::onOpenAccess()
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
 * Выполняет операции для последующего выбора номера контакта и совершения звонка.
 */
void ViewContactDialog::onCall()
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
void ViewContactDialog::onEdit()
{
    hide();

    editContactDialog = new EditContactDialog;
    editContactDialog->setValues(contactId);
    connect(editContactDialog, &EditContactDialog::sendData, this, &ViewContactDialog::receiveData);
    connect(this, &ViewContactDialog::getPos, editContactDialog, &EditContactDialog::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    editContactDialog->show();
    editContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Получает id контакта из классов ContactsDialog,
 * ViewOrgContactDialog, PopupWindow.
 */
void ViewContactDialog::setValues(QString id)
{
    contactId = id;

    QSqlQuery query(db);

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + contactId);
    query.exec();

    while (query.next())
         numbersList.append(query.value(0).toString());

    for (int i = 0; i < numbersList.length(); ++i)
        phonesList.at(i)->setText(numbersList.at(i));

    query.prepare("SELECT DISTINCT entry_person_fname, entry_person_mname, entry_person_lname, entry_city, "
                  "entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = " + contactId);
    query.exec();
    query.next();

    ui->firstName->setText(query.value(0).toString());
    ui->patronymic->setText(query.value(1).toString());
    ui->lastName->setText(query.value(2).toString());
    ui->city->setText(query.value(3).toString());
    ui->city->QWidget::setToolTip(query.value(3).toString());
    ui->address->setText(query.value(4).toString());
    ui->address->QWidget::setToolTip(query.value(4).toString());
    ui->email->setText(query.value(5).toString());
    ui->email->QWidget::setToolTip(query.value(5).toString());
    ui->vyborId->setText(query.value(6).toString());
    ui->comment->setText(query.value(7).toString());

    if (ui->vyborId->text() == "0")
        ui->openAccessButton->hide();

    page = "1";

    updateCount();
}

/**
 * Выполняет вывод и обновление истории звонков данного физ. лица.
 */
void ViewContactDialog::loadCalls()
{
    deleteObjects();

    queryModel = new QSqlQueryModel;

    queries.append(queryModel);

    setPage();

    QString queryString;

    if (ui->tabWidget->currentIndex() == 0)
    {
        queryString = "SELECT IF(";

        for (int i = 0; i < numbersList.length(); ++i)
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

    for (int i = 0; i < numbersList.length(); ++i)
    {
        if (ui->tabWidget->currentIndex() == 0)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "' ");
            else
                queryString.append(" OR src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "' ");
        }
        else if (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "' ");
            else
                queryString.append(" OR src = '" + numbersList[i] + "' ");
        }
        else if (ui->tabWidget->currentIndex() == 3)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "' ");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "' ");
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

    ui->tableView->setModel(queryModel);

    ui->tableView->setColumnHidden(3, true);

    if (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2)
        ui->tableView->setColumnHidden(4, true);

    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        QString extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        QString src = queryModel->data(queryModel->index(row_index, 1)).toString();
        QString dst = queryModel->data(queryModel->index(row_index, 2)).toString();
        QString dialogStatus = queryModel->data(queryModel->index(row_index, 3)).toString();
        QString uniqueid = queryModel->data(queryModel->index(row_index, 7)).toString();

        if (extfield.isEmpty())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 0), loadName(src, dst));

        if (ui->tabWidget->currentIndex() == 0 || ui->tabWidget->currentIndex() == 3)
            ui->tableView->setIndexWidget(queryModel->index(row_index, 4), loadStatus(dialogStatus));

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toBool())
        {
            ui->tableView->setIndexWidget(queryModel->index(row_index, 6), loadNote(uniqueid));

            ui->tableView->resizeRowToContents(row_index);
        }
        else
            ui->tableView->setRowHeight(row_index, 34);
    }

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

/**
 * Выполняет установку страницы для перехода.
 */
void ViewContactDialog::setPage()
{
    QString pages = ui->label_pages->text();

    if (countRecords <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        int remainder = countRecords % ui->comboBox_list->currentText().toInt();

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
QWidget* ViewContactDialog::loadNote(QString uniqueid)
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

        for (int i = 0; i < hrefs.length(); ++i)
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
QWidget* ViewContactDialog::loadStatus(QString dialogStatus)
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
QWidget* ViewContactDialog::loadName(QString src, QString dst)
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);

    int counter = 0;

    for (int i = 0; i < numbersList.length(); ++i)
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
void ViewContactDialog::deleteObjects()
{
    if (!widgets.isEmpty())
        for (int i = 0; i < widgets.size(); ++i)
            widgets[i]->deleteLater();

    if (!layouts.isEmpty())
        for (int i = 0; i < layouts.size(); ++i)
            layouts[i]->deleteLater();

    if (!labels.isEmpty())
        for (int i = 0; i < labels.size(); ++i)
            labels[i]->deleteLater();

    if (!queries.isEmpty())
        for (int i = 0; i < queries.size(); ++i)
            queries[i]->deleteLater();

    widgets.clear();
    layouts.clear();
    labels.clear();
    queries.clear();
}

/**
 * Выполняет обновление количества записей в истории звонков.
 */
void ViewContactDialog::updateCount()
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

    for (int i = 0; i < numbersList.length(); ++i)
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
 * Выполняет открытие окна с заметками для их просмотра.
 */
void ViewContactDialog::viewNotes(const QModelIndex &index)
{
    QString uniqueid = queryModel->data(queryModel->index(index.row(), 7)).toString();

    notesDialog = new NotesDialog;
    notesDialog->setValues(uniqueid, "");
    notesDialog->hideAddNote();
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет обработку смены вкладки.
 */
void ViewContactDialog::tabSelected()
{
    ui->tableView->setModel(NULL);

    page = "1";

    onUpdate();
}

/**
 * Выполняет операции для последующего обновления истории звонков
 * (с количеством записей).
 */
void ViewContactDialog::onUpdate()
{
    go = "default";

    updateCount();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void ViewContactDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previous";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void ViewContactDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "next";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void ViewContactDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previousStart";

    loadCalls();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void ViewContactDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "nextEnd";

    loadCalls();;
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void ViewContactDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    go = "enter";

    loadCalls();
}

/**
 * Выполняет открытие окна с медиапроигрывателем для прослушивания записи звонка.
 */
void ViewContactDialog::onPlayAudio()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
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
void ViewContactDialog::onPlayAudioPhone()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
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
 * Выполняет вытягивание значений полей из записи.
 */
void ViewContactDialog::getData(const QModelIndex &index)
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
 * Получает запрос на обновление состояния окна.
 */
void ViewContactDialog::receiveData(bool update, int x, int y)
{
    int nDesktopHeight;
    int nDesktopWidth;
    int nWidgetHeight = QWidget::height();
    int nWidgetWidth = QWidget::width();

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
