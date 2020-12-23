/*
 * Класс служит для просмотра и взаимодейстия с напоминаниями.
 */

#include "RemindersDialog.h"
#include "ui_RemindersDialog.h"

#include "PopupReminder.h"
#include "PopupNotification.h"
#include "Global.h"
#include "QSqlQueryModelReminders.h"
#include "QCustomWidget.h"

#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QSqlQuery>
#include <QLabel>
#include <QRegularExpression>
#include <QDesktopWidget>

#define TIME_TO_UPDATE 5000 // msec

RemindersDialog::RemindersDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RemindersDialog)
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

    connect(&m_timer, &QTimer::timeout, this, &RemindersDialog::onTimer);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &RemindersDialog::onTabChanged);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &RemindersDialog::onAddReminder);
    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &RemindersDialog::onEditReminder);

    ui->comboBox_list->setVisible(false);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    m_resizeCells = true;

    m_go = "default";

    m_page = "1";

    QSqlQuery query(m_db);

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_to = ? AND active = true");
    query.addBindValue(my_number);
    query.exec();

    m_oldActiveReminders = 0;

    if (query.next())
        m_oldActiveReminders = query.value(0).toInt();

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false ORDER BY id DESC");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    m_oldReceivedReminders = 0;

    if (query.next())
        m_oldReceivedReminders = query.value(0).toInt();

    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    query.prepare("SELECT id, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND datetime > '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' AND active IS TRUE");
    query.exec();

    while (query.next())
    {
        ids.append(query.value(0).value<QString>());
        dateTimes.append(query.value(1).value<QDateTime>());
        notes.append(query.value(2).value<QString>());
    }

    m_remindersThread = new QThread;
    m_remindersThreadManager = new RemindersThreadManager(ids, dateTimes, notes);

    m_remindersThreadManager->moveToThread(m_remindersThread);

    connect(m_remindersThread, &QThread::started, m_remindersThreadManager, &RemindersThreadManager::process);
    connect(m_remindersThreadManager, &RemindersThreadManager::notify, this, &RemindersDialog::onNotify);
    connect(m_remindersThreadManager, &RemindersThreadManager::finished, m_remindersThread, &QThread::quit);
    connect(m_remindersThreadManager, &RemindersThreadManager::finished, m_remindersThreadManager, &QObject::deleteLater);
    connect(m_remindersThread, &QThread::finished, m_remindersThread, &QObject::deleteLater);

    m_remindersThread->start();

    m_timer.setInterval(TIME_TO_UPDATE);

    loadReminders();

    ui->tableView->scrollToTop();
}

RemindersDialog::~RemindersDialog()
{
    m_remindersThread->requestInterruption();

    delete ui;
}

/**
 * Получает запрос на показ / скрытие
 * всплывающих окон напоминаний.
 */
void RemindersDialog::showReminders(bool show)
{
    if (show)
    {
        m_showReminder = true;

        loadReminders();

        m_timer.start();
    }
    else
    {
        m_showReminder = false;

        m_timer.stop();
    }
}

/**
 * Выполняет обработку появления окна.
 */
void RemindersDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QSqlQuery query(m_db);

    query.prepare("UPDATE reminders SET viewed = true WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    emit reminders(false);

    m_resizeCells = false;

    m_go = "default";

    loadReminders();
}

/**
 * Выполняет обработку закрытия окна.
 */
void RemindersDialog::closeEvent(QCloseEvent*)
{
    hide();

    QDialog::clearFocus();

    clearSelections();

    m_verticalScrollBar = 0;
    m_horizontalScrollBar = 0;

    ui->tabWidget->setCurrentWidget(ui->tabWidget->findChild<QWidget*>(QString("relevant")));

    m_go = "default";

    m_page = "1";

    ui->tableView->scrollToTop();

    restoreGeometry(m_geometry);

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);

    move(screen.center() - rect().center());
}

/**
 * Выполняет снятие выделения с записей.
 */
void RemindersDialog::clearSelections()
{
    m_selections.clear();

    ui->tableView->clearSelection();
}

/**
 * Выполняет по таймеру обновление списка напоминаний.
 */
void RemindersDialog::onTimer()
{
    QSqlQuery query(m_db);

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false ORDER BY id DESC");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    qint32 newReceivedReminders = 0;

    if (query.next())
        newReceivedReminders = query.value(0).toInt();

    if (newReceivedReminders > m_oldReceivedReminders)
    {
        query.prepare("SELECT id, phone_from, content FROM reminders WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false ORDER BY id DESC LIMIT 0,?");
        query.addBindValue(my_number);
        query.addBindValue(my_number);
        query.addBindValue(newReceivedReminders - m_oldReceivedReminders);
        query.exec();

        if (m_showReminder)
        {
            emit reminders(true);

            while (query.next())
                PopupNotification::showReminderNotification(this, query.value(0).toString(), query.value(1).toString(), query.value(2).toString());
        }
    }
    else
    {
        query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_to = ? AND active = true");
        query.addBindValue(my_number);
        query.exec();

        qint32 newActiveReminders = 0;

        if (query.next())
            newActiveReminders = query.value(0).toInt();

        if (newActiveReminders > m_oldActiveReminders)
            m_resizeCells = true;
        else
            m_resizeCells = false;

        m_oldActiveReminders = newActiveReminders;
    }

    m_oldReceivedReminders = newReceivedReminders;

    m_go = "default";

    loadReminders();

    sendValues();
}

/**
 * Выполняет удаление объектов класса.
 */
void RemindersDialog::deleteObjects()
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
 * Выполняет отправку данных актуальных напоминаний в класс RemindersThreadManager.
 */
void RemindersDialog::sendValues()
{
    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QSqlQuery query(m_db);

    query.prepare("SELECT id, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND datetime > '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' AND active IS TRUE");
    query.exec();

    while (query.next())
    {
        ids.append(query.value(0).value<QString>());
        dateTimes.append(query.value(1).value<QDateTime>());
        notes.append(query.value(2).value<QString>());
    }

    m_remindersThreadManager->setValues(ids, dateTimes, notes);
}

/**
 * Получает запрос на обновление состояния окна.
 */
void RemindersDialog::receiveData(bool update)
{
    if (update)
    {
        emit reminders(false);

        m_selections.clear();

        QSqlQuery query(m_db);

        query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_to = ? AND active = true");
        query.addBindValue(my_number);
        query.exec();

        qint32 newActiveReminders = 0;

        if (query.next())
            newActiveReminders = query.value(0).toInt();

        m_oldActiveReminders = newActiveReminders;

        m_go = "default";

        onUpdate();
    }
}

/**
 * Выполняет обновление количества напоминаний.
 */
void RemindersDialog::updateCount()
{
    QSqlQuery query(m_db);

    QString queryString;

    if (ui->tabWidget->currentWidget()->objectName() == "relevant")
        queryString = "SELECT COUNT(*) FROM reminders WHERE phone_to = '" + my_number + "' AND active = true";
    if (ui->tabWidget->currentWidget()->objectName() == "irrelevant")
         queryString = "SELECT COUNT(*) FROM reminders WHERE phone_to = '" + my_number + "' AND active = false";
    if (ui->tabWidget->currentWidget()->objectName() == "delegated")
         queryString = "SELECT COUNT(*) FROM (SELECT COUNT(*) FROM reminders WHERE phone_from = '" + my_number + "' AND phone_to <> '" + my_number + "' GROUP BY CASE WHEN group_id IS NOT NULL THEN group_id ELSE id END) reminders";

    query.exec(queryString);
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
    else if (m_go == "default" && m_page == "1")
        m_page = "1";

    ui->lineEdit_page->setText(m_page);
    ui->label_pages->setText(tr("из ") + pages);
}

/**
 * Выполняет вывод и обновление списка напоминаний.
 */
void RemindersDialog::loadReminders()
{
    deleteObjects();

    updateCount();

    m_queryModel = new QSqlQueryModelReminders(this);

    QString queryString = "SELECT id, phone_from, phone_to, datetime, content, active, viewed, completed, group_id FROM reminders WHERE ";

    if (ui->tabWidget->currentWidget()->objectName() == "relevant")
         queryString.append("phone_to = '" + my_number + "' AND active = true ");
    else if (ui->tabWidget->currentWidget()->objectName() == "irrelevant")
         queryString.append("phone_to = '" + my_number + "' AND active = false ");
    else if (ui->tabWidget->currentWidget()->objectName() == "delegated")
        queryString = "SELECT id, phone_from, IF(group_id IS NULL, phone_to, NULL), datetime, content, active, viewed, completed, group_id FROM reminders WHERE "
                                           "phone_from = '" + my_number + "' AND phone_to <> '" + my_number + "' GROUP BY CASE WHEN group_id IS NOT NULL THEN group_id ELSE id END";

    queryString.append(" ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0, " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()
                                                 - ui->comboBox_list->currentText().toInt()) + " , "
                            + QString::number(ui->comboBox_list->currentText().toInt()));

    m_queryModel->setQuery(queryString);

    m_queryModel->insertColumn(1);
    m_queryModel->setHeaderData(1, Qt::Horizontal, tr("Активно"));
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("От"));
    m_queryModel->setHeaderData(3, Qt::Horizontal, tr("Кому"));
    m_queryModel->setHeaderData(4, Qt::Horizontal, tr("Дата и время"));
    m_queryModel->insertColumn(9);
    m_queryModel->setHeaderData(9, Qt::Horizontal, tr("Содержание"));
    m_queryModel->insertColumn(10);
    m_queryModel->setHeaderData(10, Qt::Horizontal, tr("Просмотрено"));
    m_queryModel->insertColumn(11);
    m_queryModel->setHeaderData(11, Qt::Horizontal, tr("Выполнено"));

    ui->tableView->setModel(m_queryModel);

    m_queryModel->setParentTable(ui->tableView);

    ui->tableView->setColumnHidden(0, true);

    if (ui->tabWidget->currentWidget()->objectName() == "delegated")
        ui->tableView->setColumnHidden(2, true);
    else
    {
        ui->tableView->setColumnHidden(3, true);
        ui->tableView->setColumnHidden(10, true);
    }

    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnHidden(6, true);
    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(8, true);
    ui->tableView->setColumnHidden(12, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    for (qint32 row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        if (ui->tabWidget->currentWidget()->objectName() == "delegated")
        {
            if (ui->tableView->model()->index(row_index, 3).data(Qt::EditRole).toString().isEmpty())
                ui->tableView->setIndexWidget(m_queryModel->index(row_index, 3), addPushButtonGroup(row_index));

            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 1),  addCheckBoxActive(row_index));
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 10), addCheckBoxViewed(row_index));
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 11), addCheckBoxCompleted(row_index));
        }
        else
        {
            if (m_queryModel->data(m_queryModel->index(row_index, 2), Qt::EditRole).toString() != m_queryModel->data(m_queryModel->index(row_index, 3), Qt::EditRole).toString())
            {
                ui->tableView->setIndexWidget(m_queryModel->index(row_index, 1),  addWidgetActive());
                ui->tableView->setIndexWidget(m_queryModel->index(row_index, 11), addCheckBoxCompleted(row_index));
            }
            else
            {
                ui->tableView->setIndexWidget(m_queryModel->index(row_index, 1),  addCheckBoxActive(row_index));
                ui->tableView->setIndexWidget(m_queryModel->index(row_index, 11), addWidgetCompleted());
            }
        }

        QRegularExpressionMatchIterator hrefIterator = m_hrefRegExp.globalMatch(m_queryModel->data(m_queryModel->index(row_index, 5), Qt::EditRole).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 9), addWidgetContent(row_index, true));
        else
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 9), addWidgetContent(row_index, false));
    }

    if (m_resizeCells)
    {
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    }

    if (ui->tableView->model()->columnCount() != 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Stretch);

    if (!m_selections.isEmpty())
        for (qint32 i = 0; i < m_selections.length(); ++i)
        {
            QModelIndex index = m_selections.at(i);

            ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }

    if (m_verticalScrollBar != 0)
    {
        ui->tableView->verticalScrollBar()->setValue(m_verticalScrollBar);
        ui->tableView->horizontalScrollBar()->setValue(m_horizontalScrollBar);

        m_verticalScrollBar = 0;
        m_horizontalScrollBar = 0;
    }

    m_resizeCells = true;

    emit reminders(false);
}

/**
 * Выполняет установку виджета для поля "Содержание".
 */
QWidget* RemindersDialog::addWidgetContent(qint32 row_index, bool url)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* contentLabel = new QLabel(widget);

    layout->addWidget(contentLabel);

    QString note = m_queryModel->data(m_queryModel->index(row_index, 5), Qt::EditRole).toString();

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

    contentLabel->setText(note);
    contentLabel->setOpenExternalLinks(true);
    contentLabel->setWordWrap(true);

    m_widgets.append(widget);

    return widget;
}

/**
 *
 */
QWidget* RemindersDialog::addPushButtonGroup(qint32 row_index)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QPushButton* pushButton = new QPushButton(tr("Группа"), widget);

    layout->addWidget(pushButton, 0, Qt::AlignCenter);

    connect(pushButton, &QPushButton::clicked, this, [=]()
    {
        QCustomWidget* customWidget = new QCustomWidget(this);
        QHBoxLayout* gridLayout = new QHBoxLayout(customWidget);
        QListWidget* listWidget = new QListWidget(customWidget);

        qint32 size = 60;

        customWidget->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        customWidget->setFixedSize(250, size);

        gridLayout->addWidget(listWidget, 0, Qt::AlignCenter);

        QString group_id = m_queryModel->data(m_queryModel->index(row_index, 12), Qt::EditRole).toString();

        QSqlQuery query(m_db);

        query.prepare("SELECT phone_to FROM reminders WHERE group_id = ?");
        query.addBindValue(group_id);
        query.exec();

        while (query.next())
        {
            listWidget->addItem(query.value(0).toString());

            if (listWidget->count() > 2 && customWidget->height() < 170)
                customWidget->setFixedHeight(size += 17);
        }

        for (qint32 i = 0; i < listWidget->count(); ++i)
           listWidget->item(i)->setFlags(listWidget->item(i)->flags() & ~Qt::ItemIsSelectable);

        customWidget->setWindowTitle(tr("Группа") + " (" + QString::number(listWidget->count()) + ")");
        customWidget->show();
        customWidget->setAttribute(Qt::WA_DeleteOnClose);

        m_verticalScrollBar = ui->tableView->verticalScrollBar()->value();
        m_horizontalScrollBar = ui->tableView->horizontalScrollBar()->value();

        m_go = "default";

        onUpdate();
    });

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Активно" в полученных напоминаниях.
 */
QWidget* RemindersDialog::addWidgetActive()
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* imageLabel = new QLabel(widget);

    layout->addWidget(imageLabel, 0, Qt::AlignCenter);

    imageLabel->setPixmap(QPixmap(":/images/incomingNotification.png").scaled(15, 15, Qt::IgnoreAspectRatio));

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Активно" в личных напоминаниях.
 */
QWidget* RemindersDialog::addCheckBoxActive(qint32 row_index)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QCheckBox* checkBox = new QCheckBox(widget);

    layout->addWidget(checkBox, 0, Qt::AlignCenter);

    QString group_id = m_queryModel->data(m_queryModel->index(row_index, 12), Qt::EditRole).toString();

    if (ui->tabWidget->currentWidget()->objectName() == "delegated")
    {
        if (group_id == "0")
        {
            if (m_queryModel->data(m_queryModel->index(row_index, 6), Qt::EditRole) == true)
                checkBox->setChecked(true);
            else
                checkBox->setChecked(false);
        }
        else
        {
            QSqlQuery query(m_db);

            query.prepare("SELECT active FROM reminders WHERE group_id = ?");
            query.addBindValue(group_id);
            query.exec();

            checkBox->setChecked(false);

            while (query.next())
                if (query.value(0).toBool() == true)
                    checkBox->setChecked(true);
        }
    }
    else
    {
        if (m_queryModel->data(m_queryModel->index(row_index, 6), Qt::EditRole) == true)
            checkBox->setChecked(true);
        else
            checkBox->setChecked(false);
    }

    QString column = "active";
    QString id = m_queryModel->data(m_queryModel->index(row_index, 0), Qt::EditRole).toString();

    QDateTime dateTime = m_queryModel->data(m_queryModel->index(row_index, 4), Qt::EditRole).toDateTime();

    connect(checkBox, &QAbstractButton::pressed, this, &RemindersDialog::checkBoxStateChanged);

    checkBox->setProperty("id",       QVariant::fromValue(id));
    checkBox->setProperty("column",   QVariant::fromValue(column));
    checkBox->setProperty("checkBox", QVariant::fromValue(checkBox));
    checkBox->setProperty("group_id", QVariant::fromValue(group_id));
    checkBox->setProperty("dateTime", QVariant::fromValue(dateTime));

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Просмотрено".
 */
QWidget* RemindersDialog::addCheckBoxViewed(qint32 row_index)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QCheckBox* checkBox = new QCheckBox(widget);

    layout->addWidget(checkBox, 0, Qt::AlignCenter);

    QString group_id = m_queryModel->data(m_queryModel->index(row_index, 12), Qt::EditRole).toString();

    if (group_id == "0")
    {
        if (m_queryModel->data(m_queryModel->index(row_index, 7), Qt::EditRole) == true)
            checkBox->setChecked(true);
        else
            checkBox->setChecked(false);
    }
    else
    {
        QSqlQuery query(m_db);

        query.prepare("SELECT viewed, phone_to FROM reminders WHERE group_id = ?");
        query.addBindValue(group_id);
        query.exec();

        checkBox->setChecked(true);

        while (query.next())
            if (query.value(0).toBool() == false && query.value(1).toString() != my_number)
                checkBox->setChecked(false);
    }

    QString column = "viewed";
    QString id = m_queryModel->data(m_queryModel->index(row_index, 0), Qt::EditRole).toString();

    QDateTime dateTime = m_queryModel->data(m_queryModel->index(row_index, 4), Qt::EditRole).toDateTime();

    connect(checkBox, &QAbstractButton::pressed, this, &RemindersDialog::checkBoxStateChanged);

    checkBox->setProperty("id",       QVariant::fromValue(id));
    checkBox->setProperty("column",   QVariant::fromValue(column));
    checkBox->setProperty("checkBox", QVariant::fromValue(checkBox));
    checkBox->setProperty("dateTime", QVariant::fromValue(dateTime));

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Выполнено" в личных напоминаниях.
 */
QWidget* RemindersDialog::addWidgetCompleted()
{
    QWidget* widget = new QWidget(this);

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Выполнено" в полученных напоминаниях.
 */
QWidget* RemindersDialog::addCheckBoxCompleted(qint32 row_index)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QCheckBox* checkBox = new QCheckBox(widget);

    layout->addWidget(checkBox, 0, Qt::AlignCenter);

    QString group_id = m_queryModel->data(m_queryModel->index(row_index, 12), Qt::EditRole).toString();

    if (ui->tabWidget->currentWidget()->objectName() == "delegated")
    {


        if (group_id == "0")
        {
            if (m_queryModel->data(m_queryModel->index(row_index, 8), Qt::EditRole) == true)
                checkBox->setChecked(true);
            else
                checkBox->setChecked(false);
        }
        else
        {
            QSqlQuery query(m_db);

            query.prepare("SELECT completed, phone_to FROM reminders WHERE group_id = ?");
            query.addBindValue(group_id);
            query.exec();

            checkBox->setChecked(true);

            while (query.next())
                if (query.value(0).toBool() == false && query.value(1).toString() != my_number)
                    checkBox->setChecked(false);
        }
    }
    else
    {
        if (m_queryModel->data(m_queryModel->index(row_index, 8), Qt::EditRole) == true)
            checkBox->setChecked(true);
        else
            checkBox->setChecked(false);
    }

    QString column = "completed";
    QString id = m_queryModel->data(m_queryModel->index(row_index, 0), Qt::EditRole).toString();

    QDateTime dateTime = m_queryModel->data(m_queryModel->index(row_index, 4), Qt::EditRole).toDateTime();

    connect(checkBox, &QAbstractButton::pressed, this, &RemindersDialog::checkBoxStateChanged);

    checkBox->setProperty("id",       QVariant::fromValue(id));
    checkBox->setProperty("column",   QVariant::fromValue(column));
    checkBox->setProperty("checkBox", QVariant::fromValue(checkBox));
    checkBox->setProperty("dateTime", QVariant::fromValue(dateTime));

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет обработку смены состояния чекбокса.
 */
void RemindersDialog::checkBoxStateChanged()
{
    QString id = sender()->property("id").value<QString>();
    QString column = sender()->property("column").value<QString>();
    QDateTime dateTime = sender()->property("dateTime").value<QDateTime>();
    QCheckBox* checkBox = sender()->property("checkBox").value<QCheckBox*>();

    QString group_id = "0";

    if (column == "active")
        group_id = sender()->property("group_id").value<QString>();

    if (!checkBox->isChecked() && dateTime < QDateTime::currentDateTime() && (ui->tabWidget->currentWidget()->objectName() == "irrelevant" || ui->tabWidget->currentWidget()->objectName() == "delegated") && column == "active")
    {
        checkBox->setChecked(false);

        m_resizeCells = false;

        MsgBoxError(tr("Указано прошедшее время!"));
    }
    else
    {
        QSqlQuery query(m_db);

        if (ui->tabWidget->currentWidget()->objectName() == "relevant")
        {
            if (checkBox->isChecked() && column == "active")
            {
                checkBox->setChecked(false);

                query.prepare("UPDATE reminders SET active = false WHERE id = ?");
                query.addBindValue(id);
                query.exec();

                m_resizeCells = true;

                emit reminders(false);
            }
            else if (!checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(true);

                query.prepare("UPDATE reminders SET active = false, viewed = true, completed = true WHERE id = ?");
                query.addBindValue(id);
                query.exec();

                m_resizeCells = true;

                emit reminders(false);
            }
        }
        else if (ui->tabWidget->currentWidget()->objectName() == "irrelevant")
        {
            if (!checkBox->isChecked() && column == "active")
            {
                checkBox->setChecked(true);

                query.prepare("UPDATE reminders SET active = true WHERE id = ?");
                query.addBindValue(id);
                query.exec();

                m_resizeCells = true;

                emit reminders(false);
            }
            else if (checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(true);

                m_resizeCells = false;
            }
        }
        else if (ui->tabWidget->currentWidget()->objectName() == "delegated")
        {
            if (checkBox->isChecked() && column == "active")
            {
                checkBox->setChecked(false);

                if (group_id == "0")
                {
                    query.prepare("UPDATE reminders SET active = false WHERE id = ?");
                    query.addBindValue(id);
                    query.exec();
                }
                else
                {
                    query.prepare("UPDATE reminders SET active = false WHERE group_id = ?");
                    query.addBindValue(group_id);
                    query.exec();
                }

                m_resizeCells = false;

                emit reminders(false);
            }
            else if (!checkBox->isChecked() && column == "active")
            {
                checkBox->setChecked(true);

                if (group_id == "0")
                {
                    query.prepare("UPDATE reminders SET active = true, viewed = false, completed = false WHERE id = ?");
                    query.addBindValue(id);
                    query.exec();
                }
                else
                {
                    query.prepare("UPDATE reminders SET active = true, viewed = false, completed = false WHERE group_id = ?");
                    query.addBindValue(group_id);
                    query.exec();
                }

                m_resizeCells = false;

                emit reminders(false);
            }
            else if (checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(true);

                m_resizeCells = false;
            }
            else if (!checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(false);

                m_resizeCells = false;
            }
            else if (checkBox->isChecked() && column == "viewed")
            {
                checkBox->setChecked(true);

                m_resizeCells = false;
            }
            else if (!checkBox->isChecked() && column == "viewed")
            {
                checkBox->setChecked(false);

                m_resizeCells = false;
            }
        }
    }

    m_verticalScrollBar = ui->tableView->verticalScrollBar()->value();
    m_horizontalScrollBar = ui->tableView->horizontalScrollBar()->value();

    m_go = "default";

    onUpdate();
}

/**
 * Выполняет обработку смены вкладки.
 */
void RemindersDialog::onTabChanged()
{
    ui->tableView->setModel(NULL);

    m_go = "default";

    m_page = "1";

    onUpdate();
}

/**
 * Выполняет операции для последующего обновления списка напоминаний.
 */
void RemindersDialog::onUpdate()
{
    clearSelections();

    loadReminders();

    if (ui->tabWidget->currentWidget()->objectName() == "relevant")
    {
        QSqlQuery query(m_db);

        query.prepare("UPDATE reminders SET viewed = true WHERE phone_from <> ? AND phone_to = ? AND active = true");
        query.addBindValue(my_number);
        query.addBindValue(my_number);
        query.exec();

        emit reminders(false);
    }
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void RemindersDialog::onAddReminder()
{
    m_addReminderDialog = new AddReminderDialog;
    connect(m_addReminderDialog, &AddReminderDialog::sendData, this, &RemindersDialog::receiveData);
    m_addReminderDialog->show();
    m_addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна редактирования напоминания.
 */
void RemindersDialog::onEditReminder(const QModelIndex& index)
{
    if (ui->tabWidget->currentWidget()->objectName() == "irrelevant" && m_queryModel->data(m_queryModel->index(index.row(), 2), Qt::EditRole).toString() != m_queryModel->data(m_queryModel->index(index.row(), 3), Qt::EditRole).toString())
        return;

    QString id = m_queryModel->data(m_queryModel->index(index.row(), 0), Qt::EditRole).toString();
    QDateTime dateTime = m_queryModel->data(m_queryModel->index(index.row(), 4), Qt::EditRole).toDateTime();
    QString note = m_queryModel->data(m_queryModel->index(index.row(), 5), Qt::EditRole).toString();
    QString group_id = m_queryModel->data(m_queryModel->index(index.row(), 12), Qt::EditRole).toString();

    if (ui->tabWidget->currentWidget()->objectName() == "relevant" && m_queryModel->data(m_queryModel->index(index.row(), 2), Qt::EditRole).toString() != m_queryModel->data(m_queryModel->index(index.row(), 3), Qt::EditRole).toString())
    {
        QSqlQuery query(m_db);

        query.prepare("UPDATE reminders SET viewed = true WHERE id = ? AND active = true");
        query.addBindValue(id);
        query.exec();

        emit reminders(false);
    }

    m_editReminderDialog = new EditReminderDialog;
    m_editReminderDialog->setValues(id, group_id, dateTime, note);
    connect(m_editReminderDialog, &EditReminderDialog::sendData, this, &RemindersDialog::receiveData);
    m_editReminderDialog->show();
    m_editReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Получает данные напоминания для их последующей передачи классу PopupReminder.
 */
void RemindersDialog::onNotify(const QString& id, const QDateTime& dateTime, const QString& note)
{
    if (m_showReminder)
        PopupReminder::showReminder(this, my_number, id, dateTime, note);
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void RemindersDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previous";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void RemindersDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "next";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void RemindersDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previousStart";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void RemindersDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "nextEnd";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void RemindersDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    m_go = "enter";

    onUpdate();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void RemindersDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QDialog::keyPressEvent(event);
}
