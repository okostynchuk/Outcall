/*
 * Класс служит для просмотра и взаимодействия с историей звонков.
 */

#include "CallHistoryDialog.h"
#include "ui_CallHistoryDialog.h"

#include <QMessageBox>
#include <QDesktopWidget>

CallHistoryDialog::CallHistoryDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::CallHistoryDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    geometry = saveGeometry();

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    my_number = global::getExtensionNumber("extensions");
    my_group = global::getGroupExtensionNumber("group_extensions");

    setWindowTitle(tr("История звонков по номеру:") + " " + my_number);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(ui->playAudio,            &QAbstractButton::clicked, this, &CallHistoryDialog::onPlayAudio);
    connect(ui->callButton,           &QAbstractButton::clicked, this, &CallHistoryDialog::onCallClicked);
    connect(ui->updateButton,         &QAbstractButton::clicked, this, &CallHistoryDialog::onUpdate);
    connect(ui->playAudioPhone,       &QAbstractButton::clicked, this, &CallHistoryDialog::onPlayAudioPhone);
    connect(ui->addContactButton,     &QAbstractButton::clicked, this, &CallHistoryDialog::onAddContact);
    connect(ui->addOrgContactButton,  &QAbstractButton::clicked, this, &CallHistoryDialog::onAddOrgContact);
    connect(ui->addPhoneNumberButton, &QAbstractButton::clicked, this, &CallHistoryDialog::onAddPhoneNumberToContact);

    connect(ui->tabWidget,     &QTabWidget::currentChanged,    this, &CallHistoryDialog::tabSelected);
    connect(ui->comboBox_days, &QComboBox::currentTextChanged, this, &CallHistoryDialog::onUpdate);

    connect(ui->tableView,  &QAbstractItemView::clicked,       this, &CallHistoryDialog::getData);
    connect(ui->tableView,  &QAbstractItemView::doubleClicked, this, &CallHistoryDialog::addNote);

    ui->comboBox_list->setVisible(false);

    go = "default";

    page = "1";
}

CallHistoryDialog::~CallHistoryDialog()
{
    delete ui;
}

/**
 * Выполняет обработку появления окна.
 */
void CallHistoryDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    go = "default";

    updateCount();
}

/**
 * Выполняет обработку закрытия окна.
 */
void CallHistoryDialog::closeEvent(QCloseEvent*)
{
    hide();

    ui->comboBox_days->setCurrentIndex(0);

    ui->tabWidget->setCurrentIndex(0);

    clearSelections();

    go = "default";

    page = "1";

    ui->tableView->scrollToTop();

    restoreGeometry(geometry);

    QDesktopWidget desktop;
    QRect scr = desktop.screenGeometry(this);
    move(scr.center() - rect().center());
}

/**
 * Выполняет вывод и обновление истории звонков.
 */
void CallHistoryDialog::loadCalls()
{
    deleteObjects();

    setPage();

    queryModel = new QSqlQueryModel(this);

    QString queryString;

    if (ui->tabWidget->currentIndex() == 0)
        queryString = "SELECT IF(src = '" + my_number + "', extfield2, extfield1), ";
    else if (ui->tabWidget->currentIndex() == 3)
        queryString = "SELECT extfield2, ";
    else
        queryString = "SELECT extfield1, ";

    queryString.append("src, dst, disposition, datetime, uniqueid, recordpath FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ ui->comboBox_days->currentText() +"' DAY) ");

    if (ui->tabWidget->currentIndex() == 3)
            queryString.append("AND src = '" + my_number + "' ");
    else
    {
        if (ui->tabWidget->currentIndex() == 0)
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') ");
        else if (ui->tabWidget->currentIndex() == 1)
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') ");
        else if (ui->tabWidget->currentIndex() == 2)
            queryString.append("AND disposition = 'ANSWERED' ");

        queryString.append("AND ( ");

        if (ui->tabWidget->currentIndex() == 0)
            queryString.append("dst = '" + my_group + "' OR src = '" + my_number + "' OR ");
        if (ui->tabWidget->currentIndex() == 1)
            queryString.append("dst = '" + my_group + "' OR ");

        queryString.append("dst = '" + my_number + "' OR dst REGEXP '^[0-9]+[(]" + my_number + "[)]$' "
                                                    "OR dst REGEXP '^" + my_number + "[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                                                                                 "'^" + my_number + "[(][a-z]+ [0-9]+[(]" + my_number + "[)][)]$') ");
    }

    queryString.append("ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0, "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append(QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()
                                           - ui->comboBox_list->currentText().toInt()) + " , " +
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

    if (ui->tabWidget->currentIndex() == 3)
        ui->tableView->setColumnHidden(1, true);

    ui->tableView->setColumnHidden(3, true);

    if (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2)
        ui->tableView->setColumnHidden(4, true);

    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);

    for (qint32 row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
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

    if (!selections.isEmpty())
        for (qint32 i = 0; i < selections.length(); ++i)
        {
            QModelIndex index = selections.at(i);

            ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    else
        disableButtons();
}

/**
 * Выполняет установку страницы для перехода.
 */
void CallHistoryDialog::setPage()
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
 * Выполняет обновление количества записей в истории звонков.
 */
void CallHistoryDialog::updateCount()
{
    QSqlQuery query(dbCalls);

    QString queryString = "SELECT COUNT(*) FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + ui->comboBox_days->currentText() + "' DAY) ";

    if (ui->tabWidget->currentIndex() == 3)
            queryString.append("AND src = '" + my_number + "' ");
    else
    {
        if (ui->tabWidget->currentIndex() == 0)
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') ");
        else if (ui->tabWidget->currentIndex() == 1)
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') ");
        else if (ui->tabWidget->currentIndex() == 2)
            queryString.append("AND disposition = 'ANSWERED' ");

        queryString.append("AND ( ");

        if (ui->tabWidget->currentIndex() == 0)
            queryString.append("dst = '" + my_group + "' OR src = '" + my_number + "' OR ");
        if (ui->tabWidget->currentIndex() == 1)
            queryString.append("dst = '" + my_group + "' OR ");

        queryString.append("dst = '" + my_number + "' OR dst REGEXP '^[0-9]+[(]" + my_number + "[)]$' "
                                                    "OR dst REGEXP '^" + my_number + "[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                                                                                 "'^" + my_number + "[(][a-z]+ [0-9]+[(]" + my_number + "[)][)]$') ");
    }

    query.prepare(queryString);
    query.exec();
    query.first();

    countRecords = query.value(0).toInt();

    loadCalls();
}

/**
 * Выполняет проверку номера на соотвествие шаблону внутреннего номера.
 */
bool CallHistoryDialog::isInternalPhone(QString* str)
{
    qint32 pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("^[0-9]{4}$"));
    QRegularExpressionValidator validator2(QRegularExpression("^[2][0-9]{2}$"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Выполняет вытягивание значений полей из записи.
 */
void CallHistoryDialog::getData(const QModelIndex& index)
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

        if (!query.value(0).toBool())
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

    ui->playAudio->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);

    recordpath = queryModel->data(queryModel->index(index.row(), 8)).toString();

    if (!recordpath.isEmpty())
    {
        ui->playAudio->setDisabled(false);
        ui->playAudioPhone->setDisabled(false);
    }
}

/**
 * Выполняет установку виджета для поля "Имя".
 */
QWidget* CallHistoryDialog::loadName(const QString& src, const QString& dst)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* nameLabel = new QLabel(widget);

    if (src == my_number)
        nameLabel->setText(dst);
    else
        nameLabel->setText(src);

    layout->addWidget(nameLabel);
    layout->setContentsMargins(3, 0, 0, 0);

    widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Заметка".
 */
QWidget* CallHistoryDialog::loadNote(const QString& uniqueid)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* noteLabel = new QLabel(widget);

    layout->addWidget(noteLabel);

    QSqlQuery query(db);

    query.prepare("SELECT note FROM calls WHERE uniqueid = '" + uniqueid + "' ORDER BY datetime DESC");
    query.exec();
    query.first();

    QString note = query.value(0).toString();

    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
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

    for (int i = 0; i < hrefs.length(); ++i)
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

    widget->setMinimumHeight(33);

    widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Статус".
 */
QWidget* CallHistoryDialog::loadStatus(const QString& dialogStatus)
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

    widgets.append(widget);

    return widget;
}

/**
 * Выполняет удаление объектов класса.
 */
void CallHistoryDialog::deleteObjects()
{
    if (!queryModel.isNull())
    {
        selections = ui->tableView->selectionModel()->selectedRows();

        for (qint32 i = 0; i < widgets.size(); ++i)
            widgets[i]->deleteLater();

        widgets.clear();

        queryModel->deleteLater();
    }
}

/**
 * Выполняет операции для последующего совершения звонка.
 */
void CallHistoryDialog::onCallClicked()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

        return;
    }

    QString from = my_number;
    QString to = number;
    QString protocol = global::getSettingsValue(from, "extensions").toString();

    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

/**
 * Выполняет открытие окна добавления физ. лица
 * или же осуществляет переход в функцию его редактирования.
 */
void CallHistoryDialog::onAddContact()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

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

/**
 * Выполняет открытие окна добавления организации
 * или же осуществляет переход в функцию её редактирования.
 */
void CallHistoryDialog::onAddOrgContact()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

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

/**
 * Выполняет открытие окна редактирования физ. лица.
 */
void CallHistoryDialog::editContact(const QString& number)
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
        MsgBoxError(tr("Данный контакт принадлежит организации!"));
}

/**
 * Выполняет открытие окна редактирования организации.
 */
void CallHistoryDialog::editOrgContact(const QString& number)
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
        MsgBoxError(tr("Данный контакт принадлежит физ. лицу!"));
}

/**
 * Выполняет открытие окна привязки номера к существующему контакту.
 */
void CallHistoryDialog::onAddPhoneNumberToContact()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

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
        MsgBoxError(tr("Данный номер уже принадлежит контакту!"));
}

/**
 * Выполняет открытие окна с медиапроигрывателем для прослушивания записи звонка.
 */
void CallHistoryDialog::onPlayAudio()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

        return;
    }

    if (!recordpath.isEmpty())
    {
        if (!player.isNull())
            player->close();

        player = new Player;
        player->openMedia(recordpath);
        player->show();
        player->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет операции для последующего прослушивания записи звонка через телефон.
 */
void CallHistoryDialog::onPlayAudioPhone()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

        return;
    }

    if (!recordpath.isEmpty())
    {
        QString protocol = global::getSettingsValue(my_number, "extensions").toString();

        g_pAsteriskManager->originateAudio(my_number, protocol, recordpath);
    }
}

/**
 * Выполняет открытие окна с заметками для их просмотра и добавления.
 */
void CallHistoryDialog::addNote(const QModelIndex& index)
{
    QString uniqueid = queryModel->data(queryModel->index(index.row(), 7)).toString();

    notesDialog = new NotesDialog;
    notesDialog->setValues(uniqueid, "");
    connect(notesDialog, &NotesDialog::sendData, this, &CallHistoryDialog::onUpdate);
    notesDialog->show();
    notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет проверку существования номера в БД.
 */
bool CallHistoryDialog::checkNumber(const QString& number)
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

/**
 * Получает запрос на обновление состояния окна.
 */
void CallHistoryDialog::receiveData(bool update)
{
    if (update)
    {
        clearSelections();

        disableButtons();
    }
}

/**
 * Возвращает id контакта по его номеру.
 */
QString CallHistoryDialog::getUpdateId(const QString& number)
{
    QSqlQuery query(db);

    query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();

    QString contactId = query.value(0).toString();

    return contactId;
}

/**
 * Выполняет обработку смены вкладки.
 */
void CallHistoryDialog::tabSelected()
{
    page = "1";

    ui->tableView->setModel(NULL);

    onUpdate();
}

/**
 * Выполняет операции для последующего обновления истории звонков
 * (с количеством записей).
 */
void CallHistoryDialog::onUpdate()
{
    clearSelections();

    go = "default";

    updateCount();
}

/**
 * Выполняет операции для последующего обновления истории звонков
 * (без количества записей).
 */
void CallHistoryDialog::updateDefault()
{
    clearSelections();

    loadCalls();
}

/**
 * Выполняет снятие выделения с записей.
 */
void CallHistoryDialog::clearSelections()
{
    selections.clear();

    ui->tableView->clearSelection();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void CallHistoryDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previous";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void CallHistoryDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "next";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void CallHistoryDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previousStart";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void CallHistoryDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "nextEnd";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void CallHistoryDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    go = "enter";

    updateDefault();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void CallHistoryDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}

/**
 * Выполняет отключение кнопок окна.
 */
void CallHistoryDialog::disableButtons()
{
    ui->playAudio->setDisabled(true);
    ui->callButton->setDisabled(true);
    ui->playAudioPhone->setDisabled(true);
    ui->addContactButton->setDisabled(true);
    ui->addOrgContactButton->setDisabled(true);
    ui->addPhoneNumberButton->setDisabled(true);
}
