#include "CallHistoryDialog.h"
#include "ui_CallHistoryDialog.h"

#include <QMessageBox>

CallHistoryDialog::CallHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CallHistoryDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    my_number = global::getExtensionNumber("extensions");
    my_group = global::getGroupExtensionNumber("group_extensions");

    setWindowTitle(tr("История звонков по номеру:") + " " + my_number);

    setHeadersNonClickable();

    ui->comboBox_list->setVisible(false);

    connect(ui->addPhoneNumberButton, &QAbstractButton::clicked, this, &CallHistoryDialog::onAddPhoneNumberToContact);
    connect(ui->playAudio,            &QAbstractButton::clicked, this, &CallHistoryDialog::onPlayAudio);
    connect(ui->playAudioPhone,       &QAbstractButton::clicked, this, &CallHistoryDialog::onPlayAudioPhone);
    connect(ui->callButton,           &QAbstractButton::clicked, this, &CallHistoryDialog::onCallClicked);
    connect(ui->addContactButton,     &QAbstractButton::clicked, this, &CallHistoryDialog::onAddContact);
    connect(ui->addOrgContactButton,  &QAbstractButton::clicked, this, &CallHistoryDialog::onAddOrgContact);
    connect(ui->updateButton,         &QAbstractButton::clicked, this, &CallHistoryDialog::onUpdateClick);

    connect(ui->comboBox_2, &QComboBox::currentTextChanged, this, &CallHistoryDialog::daysChanged);
    connect(ui->tabWidget,  &QTabWidget::currentChanged, this, &CallHistoryDialog::tabSelected);

    connect(ui->tableView,   &QAbstractItemView::doubleClicked, this, &CallHistoryDialog::addNote);
    connect(ui->tableView_2, &QAbstractItemView::doubleClicked, this, &CallHistoryDialog::addNote);
    connect(ui->tableView_3, &QAbstractItemView::doubleClicked, this, &CallHistoryDialog::addNote);
    connect(ui->tableView_4, &QAbstractItemView::doubleClicked, this, &CallHistoryDialog::addNote);

    connect(ui->tableView,   &QAbstractItemView::clicked, this, &CallHistoryDialog::getData);
    connect(ui->tableView_2, &QAbstractItemView::clicked, this, &CallHistoryDialog::getData);
    connect(ui->tableView_3, &QAbstractItemView::clicked, this, &CallHistoryDialog::getData);
    connect(ui->tableView_4, &QAbstractItemView::clicked, this, &CallHistoryDialog::getData);

    ui->tabWidget->setCurrentIndex(0);

    go="default";

    page = "1";

    days = ui->comboBox_2->currentText();

    loadAllCalls();
}

CallHistoryDialog::~CallHistoryDialog()
{
    deleteObjects();

    delete ui;
}

void CallHistoryDialog::showEvent(QShowEvent*)
{
    if (ui->tabWidget->currentIndex() == 0)
        selectionAll = ui->tableView->selectionModel()->selectedRows();
    else if (ui->tabWidget->currentIndex() == 1)
        selectionMissed = ui->tableView_2->selectionModel()->selectedRows();
    else if (ui->tabWidget->currentIndex() == 2)
        selectionReceived = ui->tableView_3->selectionModel()->selectedRows();
    else if (ui->tabWidget->currentIndex() == 3)
        selectionPlaced = ui->tableView_4->selectionModel()->selectedRows();

    go = "default";

    updateCount();
}

void CallHistoryDialog::closeEvent(QCloseEvent*)
{
    ui->comboBox_2->setCurrentIndex(0);

    ui->tabWidget->setCurrentIndex(0);

    clearSelections();

    go = "default";

    page = "1";
}

void CallHistoryDialog::loadAllCalls()
{
    if (!queriesAll.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesAll.append(queryModel);

    setPage();

    QString queryString = "SELECT IF(src = '"+my_number+"', extfield2, extfield1), src, dst, disposition, datetime, uniqueid, recordpath FROM cdr "
                                                      "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL'"
                                                      " OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                                                      "'"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+""
                                                      "[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                                                      "'^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"' "
                                                      "OR src = '"+my_number+"') ORDER BY datetime DESC LIMIT ";

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append("0, "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
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

    ui->tableView->setColumnHidden(3, true);
    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();
        dst = queryModel->data(queryModel->index(row_index, 2)).toString();
        uniqueid = queryModel->data(queryModel->index(row_index, 7)).toString();
        dialogStatus = queryModel->data(queryModel->index(row_index, 3)).toString();

        ui->tableView->setIndexWidget(queryModel->index(row_index, 4), loadStatus());

        if (extfield.isEmpty())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 0), loadName());

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toInt() != 0)
        {
            ui->tableView->setIndexWidget(queryModel->index(row_index, 6), loadNote());

            ui->tableView->resizeRowToContents(row_index);
        }
        else
            ui->tableView->setRowHeight(row_index, 34);
    }

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    if (!selectionAll.isEmpty())
        for (int i = 0; i < selectionAll.length(); ++i)
        {
            QModelIndex index = selectionAll.at(i);

            ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }

    disableButtons();
}

void CallHistoryDialog::loadMissedCalls()
{
    if (!queriesMissed.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesMissed.append(queryModel);

    setPage();

    QString queryString =  "SELECT extfield1, src, dst, datetime, uniqueid FROM cdr WHERE "
                                                 "(disposition = 'NO ANSWER' "
                                                 "OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                                                 "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                                                 "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                                                 "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' "
                                                 "OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' "
                                                 "OR dst = '"+my_group+"') ORDER BY datetime DESC LIMIT ";

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append("0, "
                          + QString::number(ui->lineEdit_page->text().toInt() *
                                            ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt()
                                            * ui->comboBox_list->currentText().toInt() -
                                            ui->comboBox_list->currentText().toInt()) + " , " +
                          QString::number(ui->comboBox_list->currentText().toInt()));
    }

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Откуда"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Кому"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_2->setModel(queryModel);

    ui->tableView_2->setColumnHidden(5, true);

    if (my_group.isEmpty())
        ui->tableView_2->setColumnHidden(2, true);

    for (int row_index = 0; row_index < ui->tableView_2->model()->rowCount(); ++row_index)
    {
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();

        if (extfield.isEmpty())
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 0), loadName());

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toInt() != 0)
        {
            ui->tableView_2->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_2->resizeRowToContents(row_index);
        }
        else
            ui->tableView_2->setRowHeight(row_index, 34);
    }
    ui->tableView_2->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_2->resizeColumnsToContents();

    if (ui->tableView_2->model()->columnCount() != 0)
        ui->tableView_2->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    if (!selectionMissed.isEmpty())
        for (int i = 0; i < selectionMissed.length(); ++i)
        {
            QModelIndex index = selectionMissed.at(i);

            ui->tableView_2->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }

    disableButtons();
}

void CallHistoryDialog::loadReceivedCalls()
{
    if (!queriesReceived.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesReceived.append(queryModel);

    setPage();

    QString queryString = "SELECT extfield1, src, dst, datetime, uniqueid, recordpath FROM cdr WHERE disposition = 'ANSWERED'"
                                               " AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                                               "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                                               "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR "
                                               "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$') "
                                               "ORDER BY datetime DESC LIMIT ";

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append("0, "
                         + QString::number(ui->lineEdit_page->text().toInt() *
                                           ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
       queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() -
                                               ui->comboBox_list->currentText().toInt()) + " , " +
                         QString::number(ui->comboBox_list->currentText().toInt()));
    }

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

    if (my_group.isEmpty())
        ui->tableView_3->setColumnHidden(2, true);

    for (int row_index = 0; row_index < ui->tableView_3->model()->rowCount(); ++row_index)
    {
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        src = queryModel->data(queryModel->index(row_index, 1)).toString();

        if (extfield.isEmpty())
            ui->tableView_3->setIndexWidget(queryModel->index(row_index, 0), loadName());

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toInt() != 0)
        {
            ui->tableView_3->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_3->resizeRowToContents(row_index);
        }
        else
            ui->tableView_3->setRowHeight(row_index, 34);
    }

    ui->tableView_3->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_3->resizeColumnsToContents();

    if (ui->tableView_3->model()->columnCount() != 0)
        ui->tableView_3->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    if (!selectionReceived.isEmpty())
        for (int i = 0; i < selectionReceived.length(); ++i)
        {
            QModelIndex index = selectionReceived.at(i);

            ui->tableView_3->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }

    disableButtons();
}

void CallHistoryDialog::loadPlacedCalls()
{
    if (!queriesPlaced.isEmpty())
        deleteObjects();

    queryModel = new QSqlQueryModel;

    queriesPlaced.append(queryModel);

    setPage();

    QString queryString = "SELECT extfield2, dst, src, datetime, uniqueid, recordpath FROM cdr WHERE "
                     "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                     "src = '"+my_number+"' ORDER BY datetime DESC LIMIT ";

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append("0, "
                         + QString::number(ui->lineEdit_page->text().toInt() *
                                           ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt()
                                           * ui->comboBox_list->currentText().toInt() -
                                           ui->comboBox_list->currentText().toInt()) + " , " +
                         QString::number(ui->comboBox_list->currentText().toInt()));
    }

    queryModel->setQuery(queryString, dbCalls);

    queryModel->setHeaderData(0, Qt::Horizontal, tr("Имя"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Кому"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("Откуда"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(4);
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Заметка"));

    ui->tableView_4->setModel(queryModel);

    ui->tableView_4->setColumnHidden(2, true);
    ui->tableView_4->setColumnHidden(5, true);
    ui->tableView_4->setColumnHidden(6, true);

    for (int row_index = 0; row_index < ui->tableView_4->model()->rowCount(); ++row_index)
    {
        uniqueid = queryModel->data(queryModel->index(row_index, 5)).toString();
        extfield = queryModel->data(queryModel->index(row_index, 0)).toString();
        dst = queryModel->data(queryModel->index(row_index, 1)).toString();
        src = queryModel->data(queryModel->index(row_index, 2)).toString();

        if (extfield.isEmpty())
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 0), loadName());

        QSqlQuery query(db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid ="+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toInt() != 0)
        {
            ui->tableView_4->setIndexWidget(queryModel->index(row_index, 4), loadNote());

            ui->tableView_4->resizeRowToContents(row_index);
        }
        else
            ui->tableView_4->setRowHeight(row_index, 34);
    }

    ui->tableView_4->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView_4->resizeColumnsToContents();

    if (ui->tableView_4->model()->columnCount() != 0)
        ui->tableView_4->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    if (!selectionPlaced.isEmpty())
        for (int i = 0; i < selectionPlaced.length(); ++i)
        {
            QModelIndex index = selectionPlaced.at(i);

            ui->tableView_4->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }

    disableButtons();
}

void CallHistoryDialog::daysChanged()
{
    clearSelections();

    days = ui->comboBox_2->currentText();
    go = "default";

    updateCount();
}

void CallHistoryDialog::tabSelected()
{
    clearSelections();

    go = "default";
    page = "1";

    updateCount();
}

void CallHistoryDialog::updateCount()
{
    QSqlQuery query(dbCalls);

    if (ui->tabWidget->currentIndex() == 0)
    {
        query.prepare("SELECT COUNT(*) FROM cdr "
                      "WHERE (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' "
                      "OR disposition = 'ANSWERED') AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL "
                      "'"+ days +"' DAY) AND (dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+""
                      "[)]$' OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                      "'^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' OR dst = '"+my_group+"' "
                    "OR src = '"+my_number+"')");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadAllCalls();
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE (disposition = 'NO ANSWER'"
                      " OR disposition = 'BUSY' OR disposition = 'CANCEL') AND "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                      "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' "
                      "OR dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$' "
                      "OR dst = '"+my_group+"')");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadMissedCalls();
    }
    else if (ui->tabWidget->currentIndex() == 2)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE disposition = 'ANSWERED' "
                      "AND datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND "
                      "(dst = '"+my_number+"' OR dst REGEXP '^[0-9]+[(]"+my_number+"[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[)]$' OR "
                      "dst REGEXP '^"+my_number+"[(][a-z]+ [0-9]+[(]"+my_number+"[)][)]$')");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadReceivedCalls();
    }
    else if (ui->tabWidget->currentIndex() == 3)
    {
        query.prepare("SELECT COUNT(*) FROM cdr WHERE "
                      "datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ days +"' DAY) AND src = '"+my_number+"'");
        query.exec();
        query.first();

        count = query.value(0).toInt();

        loadPlacedCalls();
    }
}

bool CallHistoryDialog::isInternalPhone(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("^[0-9]{4}$"));
    QRegularExpressionValidator validator2(QRegularExpression("^[2][0-9]{2}$"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void CallHistoryDialog::getData(const QModelIndex &index)
{
    ui->callButton->setDisabled(false);

    number = queryModel->data(queryModel->index(index.row(), 1)).toString();

    if (number == my_number)
    {
        number = queryModel->data(queryModel->index(index.row(), 2)).toString();
        number.remove(QRegularExpression("[(][a-z]+ [0-9]+[)]"));
    }

    if (!isInternalPhone(&number))
    {
        QSqlQuery query(db);

        ui->addContactButton->setDisabled(true);
        ui->addOrgContactButton->setDisabled(true);
        ui->addPhoneNumberButton->setDisabled(true);

        query.prepare("SELECT EXISTS(SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + number + "')");
        query.exec();
        query.next();

        if (query.value(0) == 0)
        {
            ui->addContactButton->setDisabled(false);
            ui->addOrgContactButton->setDisabled(false);
            ui->addPhoneNumberButton->setDisabled(false);
        }
        else
        {
            query.prepare("SELECT entry_type FROM entry_phone WHERE entry_phone = " + number);
            query.exec();
            query.next();

            if (query.value(0).toString() == "person")
                ui->addContactButton->setDisabled(false);
            else
                ui->addOrgContactButton->setDisabled(false);
        }
    }
    else
    {
        ui->addContactButton->setDisabled(true);
        ui->addOrgContactButton->setDisabled(true);
        ui->addPhoneNumberButton->setDisabled(true);
    }

    if (ui->tabWidget->currentIndex() == 0)
        recordpath = queryModel->data(queryModel->index(index.row(), 8)).toString();
    else if (ui->tabWidget->currentIndex() == 1)
        recordpath = "";
    else if (ui->tabWidget->currentIndex() == 2 || ui->tabWidget->currentIndex() == 3)
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

void CallHistoryDialog::onCallClicked()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Выберите одну запись!"), QMessageBox::Ok);

        return;
    }

    QString from = my_number;
    QString to = number;
    QString protocol = global::getSettingsValue(from, "extensions").toString();

    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

void CallHistoryDialog::receiveData(bool updating)
{
    if (updating)
    {
        clearSelections();

        disableButtons();
    }
}

void CallHistoryDialog::onAddContact()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    if (checkNumber(number))
    {
        addContactDialog = new AddContactDialog;
        addContactDialog->setValues(number);
        connect(addContactDialog, &AddContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        addContactDialog->show();
        addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        editContact(number);
}

void CallHistoryDialog::onAddOrgContact()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    if (checkNumber(number))
    {
        addOrgContactDialog = new AddOrgContactDialog;
        addOrgContactDialog->setValues(number);
        connect(addOrgContactDialog, &AddOrgContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        addOrgContactDialog->show();
        addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        editOrgContact(number);
}

bool CallHistoryDialog::checkNumber(QString number)
{
    QSqlQuery query(db);

    query.prepare("SELECT EXISTS(SELECT fone FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.next();

    if (query.value(0) != 0)
        return false;
    else
        return true;
}

void CallHistoryDialog::editContact(QString number)
{
    QSqlQuery query(db);

    QString contactId = getUpdateId(number);

    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();

    if (query.value(0).toString() == "person")
    {
        editContactDialog = new EditContactDialog;
        editContactDialog->setValues(contactId);
        editContactDialog->hideBackButton();
        connect(editContactDialog, &EditContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        editContactDialog->show();
        editContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        QMessageBox::critical(this, tr("Ошибка"), tr("Данный контакт принадлежит организации!"), QMessageBox::Ok);
}

void CallHistoryDialog::editOrgContact(QString number)
{
    QSqlQuery query(db);

    QString contactId = getUpdateId(number);

    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();

    if (query.value(0).toString() == "org")
    {
        editOrgContactDialog = new EditOrgContactDialog;
        editOrgContactDialog->setValues(contactId);
        editOrgContactDialog->hideBackButton();
        connect(editOrgContactDialog, &EditOrgContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        editOrgContactDialog->show();
        editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        QMessageBox::critical(this, tr("Ошибка"), tr("Данный контакт принадлежит физ. лицу!"), QMessageBox::Ok);
}

void CallHistoryDialog::onAddPhoneNumberToContact()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Выберите одну запись!"), QMessageBox::Ok);
        return;
    }

    QSqlQuery query(db);

    query.prepare("SELECT EXISTS(SELECT fone FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.next();

    if (query.value(0) == 0)
    {
        addPhoneNumberToContactDialog = new AddPhoneNumberToContactDialog;
        addPhoneNumberToContactDialog->setPhoneNumber(number);
        connect(addPhoneNumberToContactDialog, &AddPhoneNumberToContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        addPhoneNumberToContactDialog->show();
        addPhoneNumberToContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        QMessageBox::critical(this, tr("Ошибка"), tr("Данный номер уже принадлежит контакту!"), QMessageBox::Ok);
}

void CallHistoryDialog::onPlayAudio()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
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

void CallHistoryDialog::onPlayAudioPhone()
{
    if ((ui->tabWidget->currentIndex() == 0 && ui->tableView->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 1 && ui->tableView_2->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 2 && ui->tableView_3->selectionModel()->selectedRows().count() != 1) || (ui->tabWidget->currentIndex() == 3 && ui->tableView_4->selectionModel()->selectedRows().count() != 1))
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

void CallHistoryDialog::addNote(const QModelIndex &index)
{
    if (ui->tabWidget->currentIndex() == 0)
    {
        uniqueid = queryModel->data(queryModel->index(index.row(), 7)).toString();
        phone =  queryModel->data(queryModel->index(index.row(), 1)).toString();
        if(phone == my_number)
            phone =  queryModel->data(queryModel->index(index.row(), 2)).toString();
    }
    else
    {
        uniqueid = queryModel->data(queryModel->index(index.row(), 5)).toString();
        phone = queryModel->data(queryModel->index(index.row(), 1)).toString();
    }

    QString state = "byId";
    notesDialog = new NotesDialog;
    notesDialog->receiveData(uniqueid, phone, state);
    connect(notesDialog, &NotesDialog::sendData, this, &CallHistoryDialog::receiveDataFromNotes);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void CallHistoryDialog::receiveDataFromNotes()
{
    clearSelections();

    go = "default";

    updateCount();
}

QString CallHistoryDialog::getUpdateId(QString number)
{
    QSqlQuery query(db);

    query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '"+number+"')");
    query.exec();
    query.first();

    QString contactId = query.value(0).toString();

    return contactId;
}

QWidget* CallHistoryDialog::loadName()
{
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QWidget* nameWgt = new QWidget;
    QLabel* nameLabel = new QLabel(nameWgt);

    if (src == my_number)
        nameLabel->setText(dst);
    else
        nameLabel->setText(src);

    nameLayout->addWidget(nameLabel);

    nameLayout->setContentsMargins(3, 0, 0, 0);

    nameWgt->setLayout(nameLayout);

    if (ui->tabWidget->currentIndex() == 0)
    {
        layoutsAllName.append(nameLayout);
        widgetsAllName.append(nameWgt);
        labelsAllName.append(nameLabel);
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        layoutsMissedName.append(nameLayout);
        widgetsMissedName.append(nameWgt);
        labelsMissedName.append(nameLabel);
    }
    else if (ui->tabWidget->currentIndex() == 2)
    {
        layoutsReceivedName.append(nameLayout);
        widgetsReceivedName.append(nameWgt);
        labelsReceivedName.append(nameLabel);
    }
    else if (ui->tabWidget->currentIndex() == 3)
    {
        layoutsPlacedName.append(nameLayout);
        widgetsPlacedName.append(nameWgt);
        labelsPlacedName.append(nameLabel);
    }

    return nameWgt;
}

void CallHistoryDialog::clearSelections()
{
    selectionAll.clear();
    selectionMissed.clear();
    selectionReceived.clear();
    selectionPlaced.clear();

    ui->tableView->clearSelection();
    ui->tableView_2->clearSelection();
    ui->tableView_3->clearSelection();
    ui->tableView_4->clearSelection();
}

void CallHistoryDialog::onUpdateClick()
{
    clearSelections();

    go = "default";

    updateCount();
}

void CallHistoryDialog::onUpdate()
{
    clearSelections();

    if (ui->tabWidget->currentIndex() == 0)
        loadAllCalls();
    else if (ui->tabWidget->currentIndex() == 1)
        loadMissedCalls();
    else if (ui->tabWidget->currentIndex() == 2)
        loadReceivedCalls();
    else if (ui->tabWidget->currentIndex() == 3)
        loadPlacedCalls();
}

QWidget* CallHistoryDialog::loadNote()
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

    if (ui->tabWidget->currentIndex() == 0)
    {
        widgetsAllNotes.append(wgt);
        layoutsAllNotes.append(layout);
        notesAll.append(noteLabel);
    }
    else if (ui->tabWidget->currentIndex() == 1)
    {
        widgetsMissedNotes.append(wgt);
        layoutsMissedNotes.append(layout);
        notesMissed.append(noteLabel);
    }
    else if (ui->tabWidget->currentIndex() == 2)
    {
        widgetsReceivedNotes.append(wgt);
        layoutsReceivedNotes.append(layout);
        notesReceived.append(noteLabel);
    }
    else if (ui->tabWidget->currentIndex() == 3)
    {
        widgetsPlacedNotes.append(wgt);
        layoutsPlacedNotes.append(layout);
        notesPlaced.append(noteLabel);
    }

    return wgt;
}

QWidget* CallHistoryDialog::loadStatus()
{
    QHBoxLayout* statusLayout = new QHBoxLayout;
    QWidget* statusWgt = new QWidget;
    QLabel* statusLabel = new QLabel(statusWgt);

    if (dialogStatus == "NO ANSWER")
        statusLabel->setText(tr("Пропущенный") + " ");
    else if (dialogStatus == "BUSY")
        statusLabel->setText(tr("Занято") + " ");
    else if (dialogStatus == "CANCEL")
        statusLabel->setText(tr("Отколено") + " ");
    else if (dialogStatus == "ANSWERED")
        statusLabel->setText(tr("Принятый") + " ");

    statusLayout->addWidget(statusLabel);

    statusLayout->setContentsMargins(3, 0, 0, 0);

    statusWgt->setLayout(statusLayout);

    layoutsStatus.append(statusLayout);
    widgetsStatus.append(statusWgt);
    labelsStatus.append(statusLabel);

    return statusWgt;
}

void CallHistoryDialog::deleteObjects()
{
    if (ui->tabWidget->currentIndex() == 0)
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
    else if (ui->tabWidget->currentIndex() == 1)
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
    else if (ui->tabWidget->currentIndex() == 2)
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
    else if (ui->tabWidget->currentIndex() == 3)
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

void CallHistoryDialog::on_previousButton_clicked()
{
    go = "previous";

    onUpdate();
}

void CallHistoryDialog::on_nextButton_clicked()
{
    go = "next";

    onUpdate();
}

void CallHistoryDialog::on_previousStartButton_clicked()
{
    go = "previousStart";

    onUpdate();
}

void CallHistoryDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";

    onUpdate();
}

void CallHistoryDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";

    onUpdate();
}

void CallHistoryDialog::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}

void CallHistoryDialog::setPage()
{
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
    ui->label_pages->setText(tr("из ") + pages);
}

void CallHistoryDialog::disableButtons()
{
    ui->callButton->setDisabled(true);
    ui->addContactButton->setDisabled(true);
    ui->addOrgContactButton->setDisabled(true);
    ui->addPhoneNumberButton->setDisabled(true);
    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
}

void CallHistoryDialog::setHeadersNonClickable()
{
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView_2->verticalHeader()->setSectionsClickable(false);
    ui->tableView_3->verticalHeader()->setSectionsClickable(false);
    ui->tableView_4->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_2->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_3->horizontalHeader()->setSectionsClickable(false);
    ui->tableView_4->horizontalHeader()->setSectionsClickable(false);
}
