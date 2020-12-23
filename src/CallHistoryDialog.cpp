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

    m_geometry = saveGeometry();

    QRegularExpression regExp("^[0-9]*$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    setWindowTitle(tr("История звонков по номеру:") + " " + g_personalNumberName);

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

    m_go = "default";

    m_page = "1";
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

    m_go = "default";

    updateCount();
}

/**
 * Выполняет обработку закрытия окна.
 */
void CallHistoryDialog::closeEvent(QCloseEvent*)
{
    hide();

    ui->comboBox_days->setCurrentIndex(0);

    ui->tabWidget->setCurrentWidget(ui->tabWidget->findChild<QWidget*>(QString("allCalls")));

    clearSelections();

    m_go = "default";

    m_page = "1";

    ui->tableView->scrollToTop();

    restoreGeometry(m_geometry);

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);
    move(screen.center() - rect().center());
}

/**
 * Выполняет вывод и обновление истории звонков.
 */
void CallHistoryDialog::loadCalls()
{
    deleteObjects();

    setPage();

    m_queryModel = new QSqlQueryModel(this);

    QString queryString;

    if (ui->tabWidget->currentWidget()->objectName() == "allCalls")
        queryString = "SELECT IF(src = '" + g_personalNumberName + "', extfield2, extfield1), ";
    else if (ui->tabWidget->currentWidget()->objectName() == "placedCalls")
        queryString = "SELECT extfield2, ";
    else
        queryString = "SELECT extfield1, ";

    queryString.append("src, dst, disposition, datetime, uniqueid, recordpath FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '"+ ui->comboBox_days->currentText() +"' DAY) ");

    if (ui->tabWidget->currentWidget()->objectName() == "placedCalls")
            queryString.append("AND src = '" + g_personalNumberName + "' ");
    else
    {
        if (ui->tabWidget->currentWidget()->objectName() == "allCalls")
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') ");
        else if (ui->tabWidget->currentWidget()->objectName() == "missedCalls")
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') ");
        else if (ui->tabWidget->currentWidget()->objectName() == "answeredCalls")
            queryString.append("AND disposition = 'ANSWERED' ");

        queryString.append("AND ( ");

        if (ui->tabWidget->currentWidget()->objectName() == "allCalls")
            queryString.append("dst = '" + g_groupNumber + "' OR src = '" + g_personalNumberName + "' OR ");
        if (ui->tabWidget->currentWidget()->objectName() == "missedCalls")
            queryString.append("dst = '" + g_groupNumber + "' OR ");

        queryString.append("dst = '" + g_personalNumberName + "' OR dst REGEXP '^[0-9]+[(]" + g_personalNumberName + "[)]$' "
                                                    "OR dst REGEXP '^" + g_personalNumberName + "[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                                                                                 "'^" + g_personalNumberName + "[(][a-z]+ [0-9]+[(]" + g_personalNumberName + "[)][)]$') ");
    }

    queryString.append("ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0, "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append(QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()
                                           - ui->comboBox_list->currentText().toInt()) + " , " +
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

    ui->tableView->setModel(m_queryModel);

    if (ui->tabWidget->currentWidget()->objectName() == "placedCalls")
        ui->tableView->setColumnHidden(1, true);

    ui->tableView->setColumnHidden(3, true);

    if (ui->tabWidget->currentWidget()->objectName() == "missedCalls" || ui->tabWidget->currentWidget()->objectName() == "answeredCalls")
        ui->tableView->setColumnHidden(4, true);

    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);

    for (qint32 row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        QString extfield = m_queryModel->data(m_queryModel->index(row_index, 0)).toString();
        QString src = m_queryModel->data(m_queryModel->index(row_index, 1)).toString();
        QString dst = m_queryModel->data(m_queryModel->index(row_index, 2)).toString();
        QString dialogStatus = m_queryModel->data(m_queryModel->index(row_index, 3)).toString();
        QString uniqueid = m_queryModel->data(m_queryModel->index(row_index, 7)).toString();

        if (extfield.isEmpty())
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 0), loadName(src, dst));

        if (ui->tabWidget->currentWidget()->objectName() == "allCalls" || ui->tabWidget->currentWidget()->objectName() == "placedCalls")
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 4), loadStatus(dialogStatus));

        QSqlQuery query(m_db);

        query.prepare("SELECT EXISTS(SELECT note FROM calls WHERE uniqueid = "+uniqueid+")");
        query.exec();
        query.first();

        if (query.value(0).toBool())
        {
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 6), loadNote(uniqueid));

            ui->tableView->resizeRowToContents(row_index);
        }
        else
            ui->tableView->setRowHeight(row_index, 34);
    }

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    if (!m_selections.isEmpty())
        for (qint32 i = 0; i < m_selections.length(); ++i)
        {
            QModelIndex index = m_selections.at(i);

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
 * Выполняет обновление количества записей в истории звонков.
 */
void CallHistoryDialog::updateCount()
{
    QSqlQuery query(m_dbCalls);

    QString queryString = "SELECT COUNT(*) FROM cdr WHERE datetime >= DATE_SUB(CURRENT_DATE, INTERVAL '" + ui->comboBox_days->currentText() + "' DAY) ";

    if (ui->tabWidget->currentWidget()->objectName() == "placedCalls")
            queryString.append("AND src = '" + g_personalNumberName + "' ");
    else
    {
        if (ui->tabWidget->currentWidget()->objectName() == "allCalls")
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL' OR disposition = 'ANSWERED') ");
        else if (ui->tabWidget->currentWidget()->objectName() == "missedCalls")
            queryString.append("AND (disposition = 'NO ANSWER' OR disposition = 'BUSY' OR disposition = 'CANCEL') ");
        else if (ui->tabWidget->currentWidget()->objectName() == "answeredCalls")
            queryString.append("AND disposition = 'ANSWERED' ");

        queryString.append("AND ( ");

        if (ui->tabWidget->currentWidget()->objectName() == "allCalls")
            queryString.append("dst = '" + g_groupNumber + "' OR src = '" + g_personalNumberName + "' OR ");
        if (ui->tabWidget->currentWidget()->objectName() == "missedCalls")
            queryString.append("dst = '" + g_groupNumber + "' OR ");

        queryString.append("dst = '" + g_personalNumberName + "' OR dst REGEXP '^[0-9]+[(]" + g_personalNumberName + "[)]$' "
                                                    "OR dst REGEXP '^" + g_personalNumberName + "[(][a-z]+ [0-9]+[)]$' OR dst REGEXP "
                                                                                 "'^" + g_personalNumberName + "[(][a-z]+ [0-9]+[(]" + g_personalNumberName + "[)][)]$') ");
    }

    query.prepare(queryString);
    query.exec();
    query.first();

    m_countRecords = query.value(0).toInt();

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

    m_number = m_queryModel->data(m_queryModel->index(index.row(), 1)).toString();

    if (m_number == g_personalNumberName)
    {
        m_number = m_queryModel->data(m_queryModel->index(index.row(), 2)).toString();
        m_number.remove(QRegularExpression("[(][a-z]+ [0-9]+[)]"));
    }

    if (!isInternalPhone(&m_number))
    {
        QSqlQuery query(m_db);

        ui->addContactButton->setDisabled(true);
        ui->addOrgContactButton->setDisabled(true);
        ui->addPhoneNumberButton->setDisabled(true);

        query.prepare("SELECT EXISTS(SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + m_number + "')");
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
            query.prepare("SELECT entry_type FROM entry_phone WHERE entry_phone = " + m_number);
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

    m_recordpath = m_queryModel->data(m_queryModel->index(index.row(), 8)).toString();

    if (!m_recordpath.isEmpty())
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

    if (src == g_personalNumberName)
        nameLabel->setText(dst);
    else
        nameLabel->setText(src);

    layout->addWidget(nameLabel);
    layout->setContentsMargins(3, 0, 0, 0);

    m_widgets.append(widget);

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

    widget->setMinimumHeight(33);

    m_widgets.append(widget);

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

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет удаление объектов класса.
 */
void CallHistoryDialog::deleteObjects()
{
    if (!m_queryModel.isNull())
    {
        m_selections = ui->tableView->selectionModel()->selectedRows();

        for (qint32 i = 0; i < m_widgets.size(); ++i)
            m_widgets[i]->deleteLater();

        m_widgets.clear();

        m_queryModel->deleteLater();
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

    QString from = g_personalNumberName;
    QString to = m_number;
    QString protocol = global::getSettingsValue(from, "extensions").toString();

    g_asteriskManager->originateCall(from, to, protocol, from);
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

    if (checkNumber(m_number))
    {
        m_addContactDialog = new AddContactDialog;
        m_addContactDialog->setValues(m_number);
        connect(m_addContactDialog, &AddContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        m_addContactDialog->show();
        m_addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        editContact(m_number);
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

    if (checkNumber(m_number))
    {
        m_addOrgContactDialog = new AddOrgContactDialog;
        m_addOrgContactDialog->setValues(m_number);
        connect(m_addOrgContactDialog, &AddOrgContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        m_addOrgContactDialog->show();
        m_addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        editOrgContact(m_number);
}

/**
 * Выполняет открытие окна редактирования физ. лица.
 */
void CallHistoryDialog::editContact(const QString& number)
{
    QSqlQuery query(m_db);

    QString contactId = getUpdateId(number);

    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();

    if (query.value(0).toString() == "person")
    {
        m_editContactDialog = new EditContactDialog;
        m_editContactDialog->setValues(contactId);
        m_editContactDialog->hideBackButton();
        connect(m_editContactDialog, &EditContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        m_editContactDialog->show();
        m_editContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
        MsgBoxError(tr("Данный контакт принадлежит организации!"));
}

/**
 * Выполняет открытие окна редактирования организации.
 */
void CallHistoryDialog::editOrgContact(const QString& number)
{
    QSqlQuery query(m_db);

    QString contactId = getUpdateId(number);

    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();

    if (query.value(0).toString() == "org")
    {
        m_editOrgContactDialog = new EditOrgContactDialog;
        m_editOrgContactDialog->setValues(contactId);
        m_editOrgContactDialog->hideBackButton();
        connect(m_editOrgContactDialog, &EditOrgContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        m_editOrgContactDialog->show();
        m_editOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
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

    QSqlQuery query(m_db);

    query.prepare("SELECT EXISTS(SELECT fone FROM fones WHERE fone = '" + m_number + "')");
    query.exec();
    query.next();

    if (query.value(0) == 0)
    {
        m_addPhoneNumberToContactDialog = new AddPhoneNumberToContactDialog;
        m_addPhoneNumberToContactDialog->setPhoneNumber(m_number);
        connect(m_addPhoneNumberToContactDialog, &AddPhoneNumberToContactDialog::sendData, this, &CallHistoryDialog::receiveData);
        m_addPhoneNumberToContactDialog->show();
        m_addPhoneNumberToContactDialog->setAttribute(Qt::WA_DeleteOnClose);
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
void CallHistoryDialog::onPlayAudioPhone()
{
    if (ui->tableView->selectionModel()->selectedRows().count() != 1)
    {
        MsgBoxError(tr("Выберите одну запись!"));

        return;
    }

    if (!m_recordpath.isEmpty())
    {
        QString protocol = global::getSettingsValue(g_personalNumberName, "extensions").toString();

        g_asteriskManager->originateAudio(g_personalNumberName, protocol, m_recordpath);
    }
}

/**
 * Выполняет открытие окна с заметками для их просмотра и добавления.
 */
void CallHistoryDialog::addNote(const QModelIndex& index)
{
    QString uniqueid = m_queryModel->data(m_queryModel->index(index.row(), 7)).toString();

    m_notesDialog = new NotesDialog;
    m_notesDialog->setValues(uniqueid, "");
    connect(m_notesDialog, &NotesDialog::sendData, this, &CallHistoryDialog::onUpdate);
    m_notesDialog->show();
    m_notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет проверку существования номера в БД.
 */
bool CallHistoryDialog::checkNumber(const QString& number)
{
    QSqlQuery query(m_db);

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
    QSqlQuery query(m_db);

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
    m_page = "1";

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

    m_go = "default";

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
    m_selections.clear();

    ui->tableView->clearSelection();
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void CallHistoryDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previous";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void CallHistoryDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "next";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void CallHistoryDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previousStart";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void CallHistoryDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "nextEnd";

    updateDefault();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void CallHistoryDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    m_go = "enter";

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
