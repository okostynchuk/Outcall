/*
 * Класс служит для отображения внутренних контактов (контактов сотрудников).
 */

#include "InternalContactsDialog.h"
#include "ui_InternalContactsDialog.h"

#include <QMap>

InternalContactsDialog::InternalContactsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::InternalContactsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    my_exten = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();
    my_number = global::getExtensionNumber("extensions");
}

InternalContactsDialog::~InternalContactsDialog()
{
    delete ui;
}

/**
 * Выполняет загрузку списка контактов при первом открытии окна.
 */
void InternalContactsDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    stateList = g_pAsteriskManager->stateList;

    if (extensions.isEmpty())
    {
        extensions = g_pAsteriskManager->extensionNumbers.values();

        for (qint32 i = 0; i < extensions.count(); ++i)
            if (extensions[i] == "" || extensions[i] == my_exten)
                extensions.removeAt(i);

        loadContacts();

        connect(g_pAsteriskManager, AsteriskManager::extenStatusChanged, this, &InternalContactsDialog::onExtenStatusChanged);
    }

    if (!ui->listWidget->isItemSelected(ui->listWidget->currentItem()))
    {
        ui->callButton->setDisabled(true);
        ui->addReminderButton->setDisabled(true);
    }
}

/**
 * Выполняет обработку закрытия окна.
 */
void InternalContactsDialog::closeEvent(QCloseEvent*)
{
    ui->listWidget->clearSelection();
    ui->listWidget->scrollToTop();
}

/**
 * Выполняет вывод и обновление списка внутренних контактов с и без фильтра.
 */
void InternalContactsDialog::loadContacts()
{
    ui->listWidget->clear();

    ui->listWidget->addItems(extensions);

    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        QString state = stateList.value(ui->listWidget->item(i)->text().remove(3, ui->listWidget->item(i)->text().length()));

        if (state == "0")
            ui->listWidget->item(i)->setIcon(QIcon(":/images/presence-idle.png"));
        else if (state == "1" || state == "2" || state == "8" ||state == "9")
            ui->listWidget->item(i)->setIcon(QIcon(":/images/presence-busy.png"));
        else if (state == "4")
            ui->listWidget->item(i)->setIcon(QIcon(":/images/presence-unavial.png"));
        else if (state == "-2" || state == "-1")
            ui->listWidget->item(i)->setIcon(QIcon(":/images/presence-dnd.png"));
        else if (state == "16" || state == "17")
            ui->listWidget->item(i)->setIcon(QIcon(":/images/presence-hold.png"));
    }

    if (indexes.isEmpty())
        for (qint32 i = 0; i < ui->listWidget->count(); ++i)
            indexes.insert(ui->listWidget->item(i)->text().remove(QRegularExpression(" .+")), i);
}

void InternalContactsDialog::onExtenStatusChanged(QString exten, QString state)
{
    if (exten != my_number)
    {
        if (state == "0")
            ui->listWidget->item(indexes.value(exten))->setIcon(QIcon(":/images/presence-idle.png"));
        else if (state == "1" || state == "2" || state == "8" ||state == "9")
            ui->listWidget->item(indexes.value(exten))->setIcon(QIcon(":/images/presence-busy.png"));
        else if (state == "4")
            ui->listWidget->item(indexes.value(exten))->setIcon(QIcon(":/images/presence-unavial.png"));
        else if (state == "-2" || state == "-1")
            ui->listWidget->item(indexes.value(exten))->setIcon(QIcon(":/images/presence-dnd.png"));
        else if (state == "16" || state == "17")
            ui->listWidget->item(indexes.value(exten))->setIcon(QIcon(":/images/presence-hold.png"));
    }
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void InternalContactsDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QDialog::keyPressEvent(event);
}

/**
 * Выполняет операции для последующего совершения звонка.
 */
void InternalContactsDialog::on_callButton_clicked()
{
    QString from = my_number;
    QString to = ui->listWidget->currentItem()->text().remove(QRegExp(" .+"));
    QString protocol = global::getSettingsValue(from, "extensions").toString();

    g_pAsteriskManager->originateCall(from, to, protocol, from);
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void InternalContactsDialog::on_addReminderButton_clicked()
{
    QStringList employee;
    employee << ui->listWidget->currentItem()->text();

    addReminderDialog = new AddReminderDialog;
    addReminderDialog->receiveEmployee(employee);
    addReminderDialog->show();
    addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void InternalContactsDialog::on_listWidget_clicked()
{
    ui->callButton->setDisabled(false);
    ui->addReminderButton->setDisabled(false);
}
