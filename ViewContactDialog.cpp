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
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_4->verticalHeader()->setSectionsClickable(false);
    ui->tableView_4->horizontalHeader()->setSectionsClickable(false);

    connect(ui->comboBox,    &QComboBox::currentTextChanged, this, &ViewContactDialog::daysChanged);
    connect(ui->tabWidget_2, &QTabWidget::currentChanged, this, &ViewContactDialog::tabSelected);

    connect(ui->openAccessButton,  &QAbstractButton::clicked, this, &ViewContactDialog::onOpenAccess);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &ViewContactDialog::onAddReminder);
    connect(ui->editButton,        &QAbstractButton::clicked, this, &ViewContactDialog::onEdit);
    connect(ui->callButton,        &QAbstractButton::clicked, this, &ViewContactDialog::onCall);
    connect(ui->playAudio,         &QAbstractButton::clicked, this, &ViewContactDialog::onPlayAudio);
    connect(ui->playAudioPhone,    &QAbstractButton::clicked, this, &ViewContactDialog::onPlayAudioPhone);

    connect(ui->tableView,   &QAbstractItemView::doubleClicked, this, &ViewContactDialog::viewNotes);
    connect(ui->tableView_2, &QAbstractItemView::doubleClicked, this, &ViewContactDialog::viewNotes);
    connect(ui->tableView_3, &QAbstractItemView::doubleClicked, this, &ViewContactDialog::viewNotes);
    connect(ui->tableView_4, &QAbstractItemView::doubleClicked, this, &ViewContactDialog::viewNotes);

    connect(ui->tableView,   &QAbstractItemView::clicked, this, &ViewContactDialog::getData);
    connect(ui->tableView_2, &QAbstractItemView::clicked, this, &ViewContactDialog::getData);
    connect(ui->tableView_3, &QAbstractItemView::clicked, this, &ViewContactDialog::getData);
    connect(ui->tableView_4, &QAbstractItemView::clicked, this, &ViewContactDialog::getData);

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

void ViewContactDialog::onAddReminder()
{
    if (!addReminderDialog.isNull())
        addReminderDialog.data()->close();

    addReminderDialog = new AddReminderDialog;
    addReminderDialog.data()->setCallId(contactId);
    addReminderDialog.data()->show();
    addReminderDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

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

void ViewContactDialog::receiveData(bool updating, int x, int y)
{
    int nDesktopHeight;
    int nDesktopWidth;
    int nWidgetHeight = QWidget::height();
    int nWidgetWidth = QWidget::width();

    QDesktopWidget desktop;
    QRect rcDesktop = desktop.availableGeometry(this);

    nDesktopWidth = rcDesktop.width();
    nDesktopHeight = rcDesktop.height();

    if (updating)
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

void ViewContactDialog::onEdit()
{
    hide();

    editContactDialog = new EditContactDialog();
    editContactDialog->setValues(contactId);
    connect(editContactDialog, &EditContactDialog::sendData, this, &ViewContactDialog::receiveData);
    connect(this, &ViewContactDialog::getPos, editContactDialog, &EditContactDialog::setPos);
    emit getPos(this->pos().x(), this->pos().y());
    editContactDialog->show();
    editContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

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

    days = ui->comboBox->currentText();

    page = "1";

    updateCount();
}

void ViewContactDialog::loadAllCalls()
{
    if (!queriesAll.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesAll.append(queryModel);

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
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT IF(";

    for (int i = 0; i < numbersList.length(); ++i)
    {
        if (i == 0)
            queryString.append("src = '"+numbersList[i]+"'");
        else
            queryString.append(" || src = '"+numbersList[i]+"'");
    }

    queryString.append(", extfield2, extfield1), src, dst, disposition, datetime, uniqueid, recordpath FROM cdr "
                          "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL'"
                          " OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                          "'" + days + "' DAY) AND (");

    for (int i = 0; i < numbersList.length(); ++i)
    {
        if (i == 0)
            queryString.append(" src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
        else
            queryString.append(" OR src = '" + numbersList[i] + "' OR dst = '" + numbersList[i] + "'");
    }

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append(") ORDER BY datetime DESC LIMIT 0,"
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
       queryString.append(") ORDER BY datetime DESC LIMIT "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
    }

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

    ui->tableView->setColumnHidden(3,true);
    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();
        dialogStatus = queryModel->data(queryModel->index(row_index, 3)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 7)).toString();

        ui->tableView->setIndexWidget(queryModel->index(row_index, 4), loadStatus());

        if (extfield.isEmpty())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 0), loadName());

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView->setIndexWidget(queryModel->index(row_index, 6), loadNote());

            ui->tableView->resizeRowToContents(row_index);
        }
        else
            ui->tableView->setRowHeight(row_index, 34);
    }

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewContactDialog::loadMissedCalls()
{
    if (!queriesMissed.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesMissed.append(queryModel);

    QSqlQuery query(db);

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
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT extfield2, src, dst, datetime, uniqueid FROM cdr WHERE ("
                          "disposition = 'NO ANSWER' OR disposition = 'BUSY' "
                          "OR disposition = 'CANCEL') AND (";

    for (int i = 0; i < numbersList.length(); ++i)
    {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");
    }

    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) ORDER BY datetime ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("DESC LIMIT 0,"
                              + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("DESC LIMIT "
                           + QString::number(ui->lineEdit_page->text().toInt()
                                             * ui->comboBox_list->currentText().toInt() -
                                             ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_2->setModel(queryModel);

    ui->tableView_2->setColumnHidden(5, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();

        if (extfield.isEmpty())
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 0), loadName());

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid =" + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_2->resizeRowToContents(row_index);
        }
        else
            ui->tableView_2->setRowHeight(row_index, 34);
    }

    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_2->resizeColumnsToContents();

    ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewContactDialog::loadReceivedCalls()
{
    if (!queriesReceived.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesReceived.append(queryModel);

    QSqlQuery query(db);

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
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT extfield2, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE "
                          "disposition = 'ANSWERED' AND (";

    for (int i = 0; i < numbersList.length(); ++i)
    {
        if (i == 0)
            queryString.append(" src = '" + numbersList[i] + "'");
        else
            queryString.append(" OR src = '" + numbersList[i] + "'");
    }

    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("DESC LIMIT 0,"
                              + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("DESC LIMIT "
                         + QString::number(ui->lineEdit_page->text().toInt()
                                           * ui->comboBox_list->currentText().toInt() -
                                           ui->comboBox_list->currentText().toInt()) + " , " +
                         QString::number(ui->comboBox_list->currentText().toInt()));

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_3->setModel(queryModel);

    ui->tableView_3->setColumnHidden(5, true);
    ui->tableView_3->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();

        if (extfield.isEmpty())
            ui->tableView_3->setIndexWidget(queryModel->index(row_index, 0), loadName());

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_3->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_3->resizeRowToContents(row_index);
        }
        else
            ui->tableView_3->setRowHeight(row_index, 34);
    }

    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_3->resizeColumnsToContents();

    ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void ViewContactDialog::loadPlacedCalls()
{
    if (!queriesPlaced.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesPlaced.append(queryModel);

    QSqlQuery query(db);

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
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages_2->setText(tr("из ") + pages);

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE (";

    for (int i = 0; i < numbersList.length(); ++i)
    {
        if (i == 0)
            queryString.append(" dst = '" + numbersList[i] + "'");
        else
            queryString.append(" OR dst = '" + numbersList[i] + "'");
    }

    queryString.append(") AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) ORDER BY datetime ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("DESC LIMIT 0,"
                              + QString::number(ui->lineEdit_page->text().toInt() *
                                                ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("DESC LIMIT "
                           + QString::number(ui->lineEdit_page->text().toInt()
                                             * ui->comboBox_list->currentText().toInt() -
                                             ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_4->setModel(queryModel);

    ui->tableView_4->setColumnHidden(5, true);
    ui->tableView_4->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();

        if (extfield.isEmpty())
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 0), loadName());

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = " + uniqueid + ")");
        query.exec();
        query.first();

        if (query.value(0) != 0)
        {
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_4->resizeRowToContents(row_index);
        }
        else
            ui->tableView_4->setRowHeight(row_index, 34);
    }

    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_4->resizeColumnsToContents();

    ui->tableView_4->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

QWidget* ViewContactDialog::loadNote()
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

        for (int i = 0; i < hrefs.length(); ++i)
            note.replace(QRegularExpression("(^|\\s)" + QRegularExpression::escape(hrefs.at(i)) + "(\\s|$)"), QString(" <a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a> "));
    }

    noteLabel->setText(note);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    wgt->setLayout(layout);

    if (ui->tabWidget_2->currentIndex() == 0)
    {
        widgetsAllNotes.append(wgt);
        layoutsAllNotes.append(layout);
        notesAll.append(noteLabel);
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        widgetsMissedNotes.append(wgt);
        layoutsMissedNotes.append(layout);
        notesMissed.append(noteLabel);
    }
    else if (ui->tabWidget_2->currentIndex() == 2)
    {
        widgetsReceivedNotes.append(wgt);
        layoutsReceivedNotes.append(layout);
        notesReceived.append(noteLabel);
    }
    else if (ui->tabWidget_2->currentIndex() == 3)
    {
        widgetsPlacedNotes.append(wgt);
        layoutsPlacedNotes.append(layout);
        notesPlaced.append(noteLabel);
    }

    return wgt;
}

QWidget* ViewContactDialog::loadStatus()
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

    layoutsStatus.append(statusLayout);
    widgetsStatus.append(statusWgt);
    labelsStatus.append(statusLabel);

    return statusWgt;
}

QWidget* ViewContactDialog::loadName()
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

    if (ui->tabWidget_2->currentIndex() == 0)
    {
        layoutsAllName.append(nameLayout);
        widgetsAllName.append(nameWgt);
        labelsAllName.append(nameLabel);
    }
    if (ui->tabWidget_2->currentIndex() == 1)
    {
        layoutsMissedName.append(nameLayout);
        widgetsMissedName.append(nameWgt);
        labelsMissedName.append(nameLabel);
    }
    if (ui->tabWidget_2->currentIndex() == 2)
    {
        layoutsReceivedName.append(nameLayout);
        widgetsReceivedName.append(nameWgt);
        labelsReceivedName.append(nameLabel);
    }
    if (ui->tabWidget_2->currentIndex() == 3)
    {
        layoutsPlacedName.append(nameLayout);
        widgetsPlacedName.append(nameWgt);
        labelsPlacedName.append(nameLabel);
    }

    return nameWgt;
}

void ViewContactDialog::deleteObjects()
{
    if (ui->tabWidget_2->currentIndex() == 0)
    {
        if (!widgetsAllNotes.isEmpty())
        {
            for (int i = 0; i < widgetsAllNotes.size(); ++i)
                widgetsAllNotes[i]->deleteLater();

            for (int i = 0; i < layoutsAllNotes.size(); ++i)
                layoutsAllNotes[i]->deleteLater();

            for (int i = 0; i < notesAll.size(); ++i)
                notesAll[i]->deleteLater();

            widgetsAllNotes.clear();
            layoutsAllNotes.clear();
            notesAll.clear();
        }

        if (!widgetsAllName.isEmpty())
        {
            for (int i = 0; i < layoutsAllName.size(); ++i)
                layoutsAllName[i]->deleteLater();

            for (int i = 0; i < widgetsAllName.size(); ++i)
                widgetsAllName[i]->deleteLater();

            for (int i = 0; i < labelsAllName.size(); ++i)
                labelsAllName[i]->deleteLater();

            layoutsAllName.clear();
            widgetsAllName.clear();
            labelsAllName.clear();
        }

        if (!widgetsStatus.isEmpty())
        {
            for (int i = 0; i < layoutsStatus.size(); ++i)
                layoutsStatus[i]->deleteLater();

            for (int i = 0; i < widgetsStatus.size(); ++i)
                widgetsStatus[i]->deleteLater();

            for (int i = 0; i < labelsStatus.size(); ++i)
                labelsStatus[i]->deleteLater();

            layoutsStatus.clear();
            widgetsStatus.clear();
            labelsStatus.clear();
        }

        for (int i = 0; i < queriesAll.size(); ++i)
            queriesAll[i]->deleteLater();

        queriesAll.clear();
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        if (!widgetsMissedNotes.isEmpty())
        {
            for (int i = 0; i < widgetsMissedNotes.size(); ++i)
                widgetsMissedNotes[i]->deleteLater();

            for (int i = 0; i < layoutsMissedNotes.size(); ++i)
                layoutsMissedNotes[i]->deleteLater();

            for (int i = 0; i < notesMissed.size(); ++i)
                notesMissed[i]->deleteLater();

            widgetsMissedNotes.clear();
            layoutsMissedNotes.clear();
            notesMissed.clear();
        }

        if (!widgetsMissedName.isEmpty())
        {
            for (int i = 0; i < layoutsMissedName.size(); ++i)
                layoutsMissedName[i]->deleteLater();

            for (int i = 0; i < widgetsMissedName.size(); ++i)
                widgetsMissedName[i]->deleteLater();

            for (int i = 0; i < labelsMissedName.size(); ++i)
                labelsMissedName[i]->deleteLater();

            layoutsMissedName.clear();
            widgetsMissedName.clear();
            labelsMissedName.clear();
        }

        for (int i = 0; i < queriesMissed.size(); ++i)
            queriesMissed[i]->deleteLater();

        queriesMissed.clear();
    }
    else if (ui->tabWidget_2->currentIndex() == 2)
    {
        if (!widgetsReceivedNotes.isEmpty())
        {
            for (int i = 0; i < widgetsReceivedNotes.size(); ++i)
                widgetsReceivedNotes[i]->deleteLater();

            for (int i = 0; i < layoutsReceivedNotes.size(); ++i)
                layoutsReceivedNotes[i]->deleteLater();

            for (int i = 0; i < notesReceived.size(); ++i)
                notesReceived[i]->deleteLater();

            widgetsReceivedNotes.clear();
            layoutsReceivedNotes.clear();
            notesReceived.clear();
        }

        if (!widgetsReceivedName.isEmpty())
        {
            for (int i = 0; i < layoutsReceivedName.size(); ++i)
                layoutsReceivedName[i]->deleteLater();

            for (int i = 0; i < widgetsReceivedName.size(); ++i)
                widgetsReceivedName[i]->deleteLater();

            for (int i = 0; i < labelsReceivedName.size(); ++i)
                labelsReceivedName[i]->deleteLater();

            layoutsReceivedName.clear();
            widgetsReceivedName.clear();
            labelsReceivedName.clear();
        }

        for (int i = 0; i < queriesReceived.size(); ++i)
            queriesReceived[i]->deleteLater();

        queriesReceived.clear();
    }
    else if (ui->tabWidget_2->currentIndex() == 3)
    {
        if (!widgetsPlacedNotes.isEmpty())
        {
            for (int i = 0; i < widgetsPlacedNotes.size(); ++i)
                widgetsPlacedNotes[i]->deleteLater();

            for (int i = 0; i < layoutsPlacedNotes.size(); ++i)
                layoutsPlacedNotes[i]->deleteLater();

            for (int i = 0; i < notesPlaced.size(); ++i)
                notesPlaced[i]->deleteLater();

            widgetsPlacedNotes.clear();
            layoutsPlacedNotes.clear();
            notesPlaced.clear();
        }

        if (!widgetsPlacedName.isEmpty())
        {
            for (int i = 0; i < layoutsPlacedName.size(); ++i)
                layoutsPlacedName[i]->deleteLater();

            for (int i = 0; i < widgetsPlacedName.size(); ++i)
                widgetsPlacedName[i]->deleteLater();

            for (int i = 0; i < labelsPlacedName.size(); ++i)
                labelsPlacedName[i]->deleteLater();

            layoutsPlacedName.clear();
            widgetsPlacedName.clear();
            labelsPlacedName.clear();
        }

        for (int i = 0; i < queriesPlaced.size(); ++i)
            queriesPlaced[i]->deleteLater();

        queriesPlaced.clear();
    }
}

void ViewContactDialog::daysChanged()
{
     days = ui->comboBox->currentText();

     go = "default";

     updateCount();
}

void ViewContactDialog::tabSelected()
{
    go = "default";

    page = "1";

    updateCount();
}

void ViewContactDialog::updateCount()
{
    QSqlQuery query(dbCalls);

    if (ui->tabWidget_2->currentIndex() == 0)
    {
        QString queryString = "SELECT COUNT(*) FROM cdr "
                              "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                              "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                              "'" + days + "' DAY) AND ( ";

        for (int i = 0; i < numbersList.length(); ++i)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "' OR src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "' OR src = '" + numbersList[i] + "'");

            if (i == numbersList.length() - 1)
                 queryString.append(")");
        }

        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadAllCalls();
    }
    else if (ui->tabWidget_2->currentIndex() == 1)
    {
        QString queryString = ("SELECT COUNT(*) FROM cdr WHERE (disposition = 'NO ANSWER'"
                      " OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < numbersList.length(); ++i)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");

            if (i == numbersList.length() - 1)
                 queryString.append(")");
        }

        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadMissedCalls();
    }
    else if (ui->tabWidget_2->currentIndex() == 2)
    {
        QString queryString = ("SELECT COUNT(*) FROM cdr WHERE disposition = 'ANSWERED' "
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < numbersList.length(); ++i)
        {
            if (i == 0)
                queryString.append(" src = '" + numbersList[i] + "'");
            else
                queryString.append(" OR src = '" + numbersList[i] + "'");

            if (i == numbersList.length() - 1)
                 queryString.append(")");
        }

        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadReceivedCalls();
    }
    else if (ui->tabWidget_2->currentIndex() == 3)
    {
        QString queryString = ("SELECT COUNT(*) FROM cdr WHERE "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + days + "' DAY) AND (");

        for (int i = 0; i < numbersList.length(); ++i)
        {
            if (i == 0)
                queryString.append(" dst = '" + numbersList[i] + "'");
            else
                queryString.append(" OR dst = '" + numbersList[i] + "'");

            if (i == numbersList.length()-1)
                 queryString.append(")");
        }

        query.prepare(queryString);
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadPlacedCalls();
    }
}

void ViewContactDialog::viewNotes(const QModelIndex &index)
{
    QString phone;

    if (ui->tabWidget_2->currentIndex() == 0)
        uniqueid = queryModel->data(queryModel->index(index.row(), 7)).toString();
    else
        uniqueid = queryModel->data(queryModel->index(index.row(), 5)).toString();

    notesDialog = new NotesDialog;
    notesDialog->receiveData(uniqueid, phone, "byId");
    notesDialog->hideAddNote();
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void ViewContactDialog::onUpdate()
{
    if (ui->tabWidget_2->currentIndex() == 0)
        loadAllCalls();
    else if (ui->tabWidget_2->currentIndex() == 1)
        loadMissedCalls();
    else if (ui->tabWidget_2->currentIndex() == 2)
        loadReceivedCalls();
    else if (ui->tabWidget_2->currentIndex() == 3)
        loadPlacedCalls();
}

void ViewContactDialog::on_previousButton_clicked()
{
    go = "previous";

    onUpdate();
}

void ViewContactDialog::on_nextButton_clicked()
{
    go = "next";

    onUpdate();
}

void ViewContactDialog::on_previousStartButton_clicked()
{
    go = "previousStart";

    onUpdate();
}

void ViewContactDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";

    onUpdate();;
}

void ViewContactDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";

    onUpdate();
}

void ViewContactDialog::onPlayAudio()
{
    if ((ui->tabWidget_2->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_2->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_2->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_2->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
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

void ViewContactDialog::onPlayAudioPhone()
{
    if ((ui->tabWidget_2->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_2->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_2->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget_2->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
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

void ViewContactDialog::getData(const QModelIndex &index)
{
    if (ui->tabWidget_2->currentIndex() == 0)
        recordpath = queryModel->data(queryModel->index(index.row(), 8)).toString();
    if (ui->tabWidget_2->currentIndex() == 1)
        recordpath = "";
    if (ui->tabWidget_2->currentIndex() == 2 || ui->tabWidget_2->currentIndex() == 3)
        recordpath = queryModel->data(queryModel->index(index.row(), 6)).toString();

    if (!recordpath.isEmpty())
    {
        ui->playAudio->setDisabled(false);
        ui->playAudioPhone->setDisabled(false);
    }
    else
    {
        ui->playAudio->setDisabled(true);
        ui->playAudioPhone->setDisabled(true);
    }
}
