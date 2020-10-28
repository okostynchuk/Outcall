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

#define TIME_TO_UPDATE 5000 // msec

RemindersDialog::RemindersDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RemindersDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    connect(&timer, &QTimer::timeout, this, &RemindersDialog::onTimer);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &RemindersDialog::onTabChanged);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &RemindersDialog::onAddReminder);
    connect(ui->tableView, &QAbstractItemView::doubleClicked, this, &RemindersDialog::onEditReminder);

    ui->comboBox_list->setVisible(false);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    ui->tabWidget->setCurrentIndex(0);

    resizeCells = true;

    go = "default";

    page = "1";

    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_to = ? AND active = true");
    query.addBindValue(my_number);
    query.exec();

    oldActiveReminders = 0;

    if (query.next())
        oldActiveReminders = query.value(0).toInt();

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false ORDER BY id DESC");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    oldReceivedReminders = 0;

    if (query.next())
        oldReceivedReminders = query.value(0).toInt();

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

    remindersThread = new QThread;
    remindersThreadManager = new RemindersThreadManager(ids, dateTimes, notes);

    remindersThreadManager->moveToThread(remindersThread);

    connect(remindersThread, &QThread::started, remindersThreadManager, &RemindersThreadManager::process);
    connect(remindersThreadManager, &RemindersThreadManager::notify, this, &RemindersDialog::onNotify);
    connect(remindersThreadManager, &RemindersThreadManager::finished, remindersThread, &QThread::quit);
    connect(remindersThreadManager, &RemindersThreadManager::finished, remindersThreadManager, &QObject::deleteLater);
    connect(remindersThread, &QThread::finished, remindersThread, &QObject::deleteLater);

    remindersThread->start();

    timer.setInterval(TIME_TO_UPDATE);

    loadReminders();

    ui->tableView->scrollToTop();
}

RemindersDialog::~RemindersDialog()
{
    remindersThread->requestInterruption();

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
        showReminder = true;

        loadReminders();

        timer.start();
    }
    else
    {
        showReminder = false;

        timer.stop();
    }
}

/**
 * Выполняет обработку появления окна.
 */
void RemindersDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET viewed = true WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    emit reminders(false);

    resizeCells = false;

    go = "default";

    loadReminders();
}

/**
 * Выполняет обработку закрытия окна.
 */
void RemindersDialog::closeEvent(QCloseEvent*)
{
    QDialog::clearFocus();

    clearSelections();

    verticalScrollBar = 0;
    horizontalScrollBar = 0;

    ui->tabWidget->setCurrentIndex(0);

    go = "default";

    page = "1";

    ui->tableView->scrollToTop();
}

/**
 * Выполняет снятие выделения с записей.
 */
void RemindersDialog::clearSelections()
{
    selections.clear();

    ui->tableView->clearSelection();
}

/**
 * Выполняет по таймеру обновление списка напоминаний.
 */
void RemindersDialog::onTimer()
{
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false ORDER BY id DESC");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    qint32 newReceivedReminders = 0;

    if (query.next())
        newReceivedReminders = query.value(0).toInt();

    if (newReceivedReminders > oldReceivedReminders)
    {
        query.prepare("SELECT id, phone_from, content FROM reminders WHERE phone_from <> ? AND phone_to = ? AND active = true AND viewed = false ORDER BY id DESC LIMIT 0,?");
        query.addBindValue(my_number);
        query.addBindValue(my_number);
        query.addBindValue(newReceivedReminders - oldReceivedReminders);
        query.exec();

        if (showReminder)
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

        if (newActiveReminders > oldActiveReminders)
            resizeCells = true;
        else
            resizeCells = false;

        oldActiveReminders = newActiveReminders;
    }

    oldReceivedReminders = newReceivedReminders;

    go = "default";

    loadReminders();

    sendValues();
}

/**
 * Выполняет удаление объектов класса.
 */
void RemindersDialog::deleteObjects()
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
 * Выполняет отправку данных актуальных напоминаний в класс RemindersThreadManager.
 */
void RemindersDialog::sendValues()
{
    QList<QString> ids;
    QList<QDateTime> dateTimes;
    QList<QString> notes;

    QSqlQuery query(db);

    query.prepare("SELECT id, datetime, content FROM reminders WHERE phone_to = '" + my_number + "' AND datetime > '" + QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss") + "' AND active IS TRUE");
    query.exec();

    while (query.next())
    {
        ids.append(query.value(0).value<QString>());
        dateTimes.append(query.value(1).value<QDateTime>());
        notes.append(query.value(2).value<QString>());
    }

    remindersThreadManager->setValues(ids, dateTimes, notes);
}

/**
 * Получает запрос на обновление состояния окна.
 */
void RemindersDialog::receiveData(bool update)
{
    if (update)
    {
        emit reminders(false);

        selections.clear();

        QSqlQuery query(db);

        query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_to = ? AND active = true");
        query.addBindValue(my_number);
        query.exec();

        qint32 newActiveReminders = 0;

        if (query.next())
            newActiveReminders = query.value(0).toInt();

        oldActiveReminders = newActiveReminders;

        go = "default";

        onUpdate();
    }
}

/**
 * Выполняет обновление количества напоминаний.
 */
void RemindersDialog::updateCount()
{
    QSqlQuery query(db);

    QString queryString;

    if (ui->tabWidget->currentIndex() == 0)
        queryString = "SELECT COUNT(*) FROM reminders WHERE phone_to = '" + my_number + "' AND active = true";
    if (ui->tabWidget->currentIndex() == 1)
         queryString = "SELECT COUNT(*) FROM reminders WHERE phone_to = '" + my_number + "' AND active = false";
    if (ui->tabWidget->currentIndex() == 2)
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
 * Выполняет вывод и обновление списка напоминаний.
 */
void RemindersDialog::loadReminders()
{
    deleteObjects();

    updateCount();

    queryModel = new QSqlQueryModelReminders(this);

    QString queryString = "SELECT id, phone_from, phone_to, datetime, content, active, viewed, completed, group_id FROM reminders WHERE ";

    if (ui->tabWidget->currentIndex() == 0)
         queryString.append("phone_to = '" + my_number + "' AND active = true ");
    else if (ui->tabWidget->currentIndex() == 1)
         queryString.append("phone_to = '" + my_number + "' AND active = false ");
    else if (ui->tabWidget->currentIndex() == 2)
        queryString = "SELECT id, phone_from, IF(group_id IS NULL, phone_to, NULL), datetime, content, active, viewed, completed, group_id FROM reminders WHERE "
                                           "phone_from = '" + my_number + "' AND phone_to <> '" + my_number + "' GROUP BY CASE WHEN group_id IS NOT NULL THEN group_id ELSE id END";

    queryString.append(" ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0, " + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()
                                                 - ui->comboBox_list->currentText().toInt()) + " , "
                            + QString::number(ui->comboBox_list->currentText().toInt()));

    queryModel->setQuery(queryString);

    queryModel->insertColumn(1);
    queryModel->setHeaderData(1, Qt::Horizontal, tr("Активно"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("От"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("Кому"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("Дата и время"));
    queryModel->insertColumn(9);
    queryModel->setHeaderData(9, Qt::Horizontal, tr("Содержание"));
    queryModel->insertColumn(10);
    queryModel->setHeaderData(10, Qt::Horizontal, tr("Просмотрено"));
    queryModel->insertColumn(11);
    queryModel->setHeaderData(11, Qt::Horizontal, tr("Выполнено"));

    ui->tableView->setModel(queryModel);

    queryModel->setParentTable(ui->tableView);

    ui->tableView->setColumnHidden(0, true);

    if (ui->tabWidget->currentIndex() == 2)
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
        if (ui->tabWidget->currentIndex() == 2)
        {
            if (ui->tableView->model()->index(row_index, 3).data(Qt::EditRole).toString().isEmpty())
                ui->tableView->setIndexWidget(queryModel->index(row_index, 3), addPushButtonGroup(row_index));

            ui->tableView->setIndexWidget(queryModel->index(row_index, 1),  addCheckBoxActive(row_index));
            ui->tableView->setIndexWidget(queryModel->index(row_index, 10), addCheckBoxViewed(row_index));
            ui->tableView->setIndexWidget(queryModel->index(row_index, 11), addCheckBoxCompleted(row_index));
        }
        else
        {
            if (queryModel->data(queryModel->index(row_index, 2), Qt::EditRole).toString() != queryModel->data(queryModel->index(row_index, 3), Qt::EditRole).toString())
            {
                ui->tableView->setIndexWidget(queryModel->index(row_index, 1),  addWidgetActive());
                ui->tableView->setIndexWidget(queryModel->index(row_index, 11), addCheckBoxCompleted(row_index));
            }
            else
            {
                ui->tableView->setIndexWidget(queryModel->index(row_index, 1),  addCheckBoxActive(row_index));
                ui->tableView->setIndexWidget(queryModel->index(row_index, 11), addWidgetCompleted());
            }
        }

        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(queryModel->data(queryModel->index(row_index, 5), Qt::EditRole).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(queryModel->index(row_index, 9), addWidgetContent(row_index, true));
        else
            ui->tableView->setIndexWidget(queryModel->index(row_index, 9), addWidgetContent(row_index, false));
    }

    if (resizeCells)
    {
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    }

    if (ui->tableView->model()->columnCount() != 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Stretch);

    if (!selections.isEmpty())
        for (qint32 i = 0; i < selections.length(); ++i)
        {
            QModelIndex index = selections.at(i);

            ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }

    if (verticalScrollBar != 0)
    {
        ui->tableView->verticalScrollBar()->setValue(verticalScrollBar);
        ui->tableView->horizontalScrollBar()->setValue(horizontalScrollBar);

        verticalScrollBar = 0;
        horizontalScrollBar = 0;
    }

    resizeCells = true;

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

    QString note = queryModel->data(queryModel->index(row_index, 5), Qt::EditRole).toString();

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
                index = note.indexOf(hrefs.at(i), index + hrefs.at(i - 1).size());

            if (index > 0)
                strList << note.at(index - 1);
            else
                strList << "";
        }

        for (qint32 i = 0; i < hrefs.length(); ++i)
            note.replace(note.indexOf(QRegularExpression("(^| |\\.|\\,|\\(|\\)|\\[|\\]|\\{|\\})" + QRegularExpression::escape(hrefs.at(i)) + "(|$)")),
                         hrefs.at(i).size()+1, QString(strList.at(i) + "<a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a>"));
    }

    contentLabel->setText(note);
    contentLabel->setOpenExternalLinks(true);
    contentLabel->setWordWrap(true);

    widgets.append(widget);

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

        QString group_id = queryModel->data(queryModel->index(row_index, 12), Qt::EditRole).toString();

        QSqlQuery query(db);

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

        verticalScrollBar = ui->tableView->verticalScrollBar()->value();
        horizontalScrollBar = ui->tableView->horizontalScrollBar()->value();

        go = "default";

        onUpdate();
    });

    widgets.append(widget);

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

    widgets.append(widget);

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

    QString group_id = queryModel->data(queryModel->index(row_index, 12), Qt::EditRole).toString();

    if (ui->tabWidget->currentIndex() == 2)
    {
        if (group_id == "0")
        {
            if (queryModel->data(queryModel->index(row_index, 6), Qt::EditRole) == true)
                checkBox->setChecked(true);
            else
                checkBox->setChecked(false);
        }
        else
        {
            QSqlQuery query(db);

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
        if (queryModel->data(queryModel->index(row_index, 6), Qt::EditRole) == true)
            checkBox->setChecked(true);
        else
            checkBox->setChecked(false);
    }

    QString column = "active";
    QString id = queryModel->data(queryModel->index(row_index, 0), Qt::EditRole).toString();

    QDateTime dateTime = queryModel->data(queryModel->index(row_index, 4), Qt::EditRole).toDateTime();

    connect(checkBox, &QAbstractButton::pressed, this, &RemindersDialog::checkBoxStateChanged);

    checkBox->setProperty("id",       QVariant::fromValue(id));
    checkBox->setProperty("column",   QVariant::fromValue(column));
    checkBox->setProperty("checkBox", QVariant::fromValue(checkBox));
    checkBox->setProperty("group_id", QVariant::fromValue(group_id));
    checkBox->setProperty("dateTime", QVariant::fromValue(dateTime));

    widgets.append(widget);

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

    QString group_id = queryModel->data(queryModel->index(row_index, 12), Qt::EditRole).toString();

    if (group_id == "0")
    {
        if (queryModel->data(queryModel->index(row_index, 7), Qt::EditRole) == true)
            checkBox->setChecked(true);
        else
            checkBox->setChecked(false);
    }
    else
    {
        QSqlQuery query(db);

        query.prepare("SELECT viewed, phone_to FROM reminders WHERE group_id = ?");
        query.addBindValue(group_id);
        query.exec();

        checkBox->setChecked(true);

        while (query.next())
            if (query.value(0).toBool() == false && query.value(1).toString() != my_number)
                checkBox->setChecked(false);
    }

    QString column = "viewed";
    QString id = queryModel->data(queryModel->index(row_index, 0), Qt::EditRole).toString();

    QDateTime dateTime = queryModel->data(queryModel->index(row_index, 4), Qt::EditRole).toDateTime();

    connect(checkBox, &QAbstractButton::pressed, this, &RemindersDialog::checkBoxStateChanged);

    checkBox->setProperty("id",       QVariant::fromValue(id));
    checkBox->setProperty("column",   QVariant::fromValue(column));
    checkBox->setProperty("checkBox", QVariant::fromValue(checkBox));
    checkBox->setProperty("dateTime", QVariant::fromValue(dateTime));

    widgets.append(widget);

    return widget;
}

/**
 * Выполняет установку виджета для поля "Выполнено" в личных напоминаниях.
 */
QWidget* RemindersDialog::addWidgetCompleted()
{
    QWidget* widget = new QWidget(this);

    widgets.append(widget);

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

    QString group_id = queryModel->data(queryModel->index(row_index, 12), Qt::EditRole).toString();

    if (ui->tabWidget->currentIndex() == 2)
    {


        if (group_id == "0")
        {
            if (queryModel->data(queryModel->index(row_index, 8), Qt::EditRole) == true)
                checkBox->setChecked(true);
            else
                checkBox->setChecked(false);
        }
        else
        {
            QSqlQuery query(db);

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
        if (queryModel->data(queryModel->index(row_index, 8), Qt::EditRole) == true)
            checkBox->setChecked(true);
        else
            checkBox->setChecked(false);
    }

    QString column = "completed";
    QString id = queryModel->data(queryModel->index(row_index, 0), Qt::EditRole).toString();

    QDateTime dateTime = queryModel->data(queryModel->index(row_index, 4), Qt::EditRole).toDateTime();

    connect(checkBox, &QAbstractButton::pressed, this, &RemindersDialog::checkBoxStateChanged);

    checkBox->setProperty("id",       QVariant::fromValue(id));
    checkBox->setProperty("column",   QVariant::fromValue(column));
    checkBox->setProperty("checkBox", QVariant::fromValue(checkBox));
    checkBox->setProperty("dateTime", QVariant::fromValue(dateTime));

    widgets.append(widget);

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

    if (!checkBox->isChecked() && dateTime < QDateTime::currentDateTime() && (ui->tabWidget->currentIndex() == 1 || ui->tabWidget->currentIndex() == 2) && column == "active")
    {
        checkBox->setChecked(false);

        resizeCells = false;

        QMessageBox::critical(this, tr("Ошибка"), tr("Указано прошедшее время!"), QMessageBox::Ok);
    }
    else
    {
        QSqlQuery query(db);

        if (ui->tabWidget->currentIndex() == 0)
        {
            if (checkBox->isChecked() && column == "active")
            {
                checkBox->setChecked(false);

                query.prepare("UPDATE reminders SET active = false WHERE id = ?");
                query.addBindValue(id);
                query.exec();

                resizeCells = true;

                emit reminders(false);
            }
            else if (!checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(true);

                query.prepare("UPDATE reminders SET active = false, viewed = true, completed = true WHERE id = ?");
                query.addBindValue(id);
                query.exec();

                resizeCells = true;

                emit reminders(false);
            }
        }
        else if (ui->tabWidget->currentIndex() == 1)
        {
            if (!checkBox->isChecked() && column == "active")
            {
                checkBox->setChecked(true);

                query.prepare("UPDATE reminders SET active = true WHERE id = ?");
                query.addBindValue(id);
                query.exec();

                resizeCells = true;

                emit reminders(false);
            }
            else if (checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(true);

                resizeCells = false;
            }
        }
        else if (ui->tabWidget->currentIndex() == 2)
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

                resizeCells = false;

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

                resizeCells = false;

                emit reminders(false);
            }
            else if (checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(true);

                resizeCells = false;
            }
            else if (!checkBox->isChecked() && column == "completed")
            {
                checkBox->setChecked(false);

                resizeCells = false;
            }
            else if (checkBox->isChecked() && column == "viewed")
            {
                checkBox->setChecked(true);

                resizeCells = false;
            }
            else if (!checkBox->isChecked() && column == "viewed")
            {
                checkBox->setChecked(false);

                resizeCells = false;
            }
        }
    }

    verticalScrollBar = ui->tableView->verticalScrollBar()->value();
    horizontalScrollBar = ui->tableView->horizontalScrollBar()->value();

    go = "default";

    onUpdate();
}

/**
 * Выполняет обработку смены вкладки.
 */
void RemindersDialog::onTabChanged()
{
    ui->tableView->setModel(NULL);

    go = "default";

    page = "1";

    onUpdate();
}

/**
 * Выполняет операции для последующего обновления списка напоминаний.
 */
void RemindersDialog::onUpdate()
{
    clearSelections();

    loadReminders();

    if (ui->tabWidget->currentIndex() == 0)
    {
        QSqlQuery query(db);

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
    addReminderDialog = new AddReminderDialog;
    connect(addReminderDialog, &AddReminderDialog::sendData, this, &RemindersDialog::receiveData);
    addReminderDialog->show();
    addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет открытие окна редактирования напоминания.
 */
void RemindersDialog::onEditReminder(const QModelIndex& index)
{
    if (ui->tabWidget->currentIndex() == 1 && queryModel->data(queryModel->index(index.row(), 2), Qt::EditRole).toString() != queryModel->data(queryModel->index(index.row(), 3), Qt::EditRole).toString())
        return;

    QString id = queryModel->data(queryModel->index(index.row(), 0), Qt::EditRole).toString();
    QDateTime dateTime = queryModel->data(queryModel->index(index.row(), 4), Qt::EditRole).toDateTime();
    QString note = queryModel->data(queryModel->index(index.row(), 5), Qt::EditRole).toString();
    QString group_id = queryModel->data(queryModel->index(index.row(), 12), Qt::EditRole).toString();

    if (ui->tabWidget->currentIndex() == 0 && queryModel->data(queryModel->index(index.row(), 2), Qt::EditRole).toString() != queryModel->data(queryModel->index(index.row(), 3), Qt::EditRole).toString())
    {
        QSqlQuery query(db);

        query.prepare("UPDATE reminders SET viewed = true WHERE id = ? AND active = true");
        query.addBindValue(id);
        query.exec();

        emit reminders(false);
    }

    editReminderDialog = new EditReminderDialog;
    editReminderDialog->setValues(id, group_id, dateTime, note);
    connect(editReminderDialog, &EditReminderDialog::sendData, this, &RemindersDialog::receiveData);
    editReminderDialog->show();
    editReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Получает данные напоминания для их последующей передачи классу PopupReminder.
 */
void RemindersDialog::onNotify(const QString& id, const QDateTime& dateTime, const QString& note)
{
    if (showReminder)
        PopupReminder::showReminder(this, my_number, id, dateTime, note);
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void RemindersDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previous";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void RemindersDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "next";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void RemindersDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "previousStart";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void RemindersDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    go = "nextEnd";

    onUpdate();;
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void RemindersDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    go = "enter";

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
