/*
 * Класс служит для отображения внутренних контактов (контактов сотрудников).
 */

#include "InternalContactsDialog.h"
#include "ui_InternalContactsDialog.h"

InternalContactsDialog::InternalContactsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InternalContactsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->comboBox_list->setVisible(false);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &InternalContactsDialog::onSearch);
}

InternalContactsDialog::~InternalContactsDialog()
{
    deleteObjects();

    delete ui;
}

/**
 * Выполняет удаление объектов класса.
 */
void InternalContactsDialog::deleteObjects()
{
    for (int i = 0; i < layouts.length(); ++i)
        layouts[i]->deleteLater();

    for (int i = 0; i < widgets.length(); ++i)
        widgets[i]->deleteLater();

    for (int i = 0; i < buttons.length(); ++i)
        buttons[i]->deleteLater();

    layouts.clear();
    widgets.clear();
    buttons.clear();
}

/**
 * Выполняет загрузку списка контактов при первом открытии окна.
 */
void InternalContactsDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    if (extensions.isEmpty() && ui->lineEdit->text().isEmpty())
    {
        extensions = g_pAsteriskManager->extensionNumbers.values();

        for (int i = 0; i < extensions.count(); ++i)
            if (extensions[i] == my_number)
                extensions.removeAt(i);

        extensions_full = extensions;

        list = new QListWidget;

        list->addItems(extensions);

        go = "default";

        page = "1";

        loadContacts();
    }
}

/**
 * Выполняет обработку закрытия окна.
 */
void InternalContactsDialog::closeEvent(QCloseEvent*)
{
    ui->lineEdit->clear();

    extensions.clear();

    extensions = extensions_full;

    go = "default";

    page = "1";
}

/**
 * Выполняет вывод и обновление списка внутренних контактов с и без фильтра.
 */
void InternalContactsDialog::loadContacts()
{
    int count = extensions.count();

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        int remainder = count % ui->comboBox_list->currentText().toInt();

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

    if (ui->lineEdit_page->text() == "1")
    {
        if (count < ui->comboBox_list->currentText().toInt())
        {
            l_from = 0;
            l_to = count;
        }
        else
        {
            l_from = 0;
            l_to = (ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt());
        }
    }
    else
    {
        l_from = ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt();

        if (page == pages)
            l_to = l_from + (count - l_from);
        else
            l_to = l_from + ui->comboBox_list->currentText().toInt();
    }

    ui->listWidget->clear();

    for (int i = l_from; i < l_to; ++i)
        ui->listWidget->addItem(extensions[i]);

    if (!widgets.isEmpty())
        deleteObjects();

    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
       ui->listWidget->setItemWidget(ui->listWidget->item(i), addWgt(ui->listWidget->item(i)->text()));

       ui->listWidget->item(i)->setSizeHint(addWgt(ui->listWidget->item(i)->text())->sizeHint());
    }
}

/**
 * Выполняет операции для последующего поиска по списку.
 */
void InternalContactsDialog::onSearch()
{
    if (!itemsSearch.isEmpty())
        itemsSearch.clear();

    itemsSearch = list->findItems(ui->lineEdit->text(), Qt::MatchContains);

    extensions.clear();

    for (int i = 0; i < itemsSearch.length(); i ++)
        extensions.append(itemsSearch[i]->text());

    ui->listWidget->clear();

    go = "default";

    page = "1";

    loadContacts();
}

/**
 * Выполняет добавление виджета для отображения кнопок осуществления звонка и создания напоминания.
 */
QWidget* InternalContactsDialog::addWgt(QString name)
{
    QHBoxLayout* layout = new QHBoxLayout;
    QWidget* wgt = new QWidget;

    QPushButton* callButton = new QPushButton();
    callButton->setAutoDefault(false);
    callButton->setFocusPolicy(Qt::ClickFocus);
    callButton->setIcon(QIcon(":/images/makeCall.png"));
    callButton->setIconSize(QSize(25, 25));
    callButton->setFixedSize(30, 30);
    connect(callButton, &QAbstractButton::clicked, this, &InternalContactsDialog::onCall);
    callButton->setProperty("callButton", QVariant::fromValue(name));

    QPushButton* addReminderButton = new QPushButton();
    addReminderButton->setAutoDefault(false);
    addReminderButton->setFocusPolicy(Qt::ClickFocus);
    addReminderButton->setIcon(QIcon(":/images/reminders.png"));
    addReminderButton->setIconSize(QSize(25, 25));
    addReminderButton->setFixedSize(30, 30);
    connect(addReminderButton, &QAbstractButton::clicked, this, &InternalContactsDialog::onAddReminder);
    addReminderButton->setProperty("addReminderButton", QVariant::fromValue(name));

    layout->addWidget(callButton);
    layout->addWidget(addReminderButton);

    wgt->setLayout(layout);

    wgt->setContentsMargins(130, 2, 2, 2);
    layout->setContentsMargins(130, 2, 2, 2);

    layouts.append(layout);
    widgets.append(wgt);
    buttons.append(callButton);
    buttons.append(addReminderButton);

    return wgt;
}

/**
 * Выполняет операции для последующего совершения звонка.
 */
void InternalContactsDialog::onCall()
{
    QString from = global::getExtensionNumber("extensions");
    QString protocol = global::getSettingsValue(from, "extensions").toString();
    QString to = sender()->property("callButton").value<QString>().remove(QRegularExpression(" .+"));

    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void InternalContactsDialog::onAddReminder()
{
    addReminderDialog = new AddReminderDialog;
    addReminderDialog->receiveEmployee(sender()->property("addReminderButton").value<QStringList>());
    addReminderDialog->show();
    addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void InternalContactsDialog::on_previousButton_clicked()
{
    go = "previous";

    loadContacts();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void InternalContactsDialog::on_nextButton_clicked()
{
    go = "next";

    loadContacts();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void InternalContactsDialog::on_previousStartButton_clicked()
{
    go = "previousStart";

    loadContacts();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void InternalContactsDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";

    loadContacts();;
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void InternalContactsDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";

    loadContacts();
}
