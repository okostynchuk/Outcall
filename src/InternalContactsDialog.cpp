/*
 * Класс служит для отображения внутренних контактов (контактов сотрудников).
 */

#include "InternalContactsDialog.h"
#include "ui_InternalContactsDialog.h"

#include <QDesktopWidget>
#include <QMap>

InternalContactsDialog::InternalContactsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::InternalContactsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    m_geometry = saveGeometry();
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

    m_states = g_asteriskManager->m_states;

    if (m_extensions.isEmpty())
    {
        m_extensions = g_asteriskManager->m_extensionNumbers.values();

        for (qint32 i = 0; i < m_extensions.count(); ++i)
            if (m_extensions[i] == "" || m_extensions[i] == g_personalNumberName)
                m_extensions.removeAt(i);

        loadContacts();

        connect(g_asteriskManager, &AsteriskManager::extenStatusChanged, this, &InternalContactsDialog::onExtenStatusChanged);
    }

    ui->callButton->setDisabled(true);
    ui->addReminderButton->setDisabled(true);

    if (ui->listWidget->currentItem())
        if (ui->listWidget->currentItem()->isSelected())
        {
            ui->callButton->setDisabled(false);
            ui->addReminderButton->setDisabled(false);
        }
}

/**
 * Выполняет обработку закрытия окна.
 */
void InternalContactsDialog::closeEvent(QCloseEvent*)
{
    hide();

    restoreGeometry(m_geometry);

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);
    move(screen.center() - rect().center());

    ui->listWidget->clearSelection();
    ui->listWidget->scrollToTop();
}

/**
 * Выполняет вывод и обновление списка внутренних контактов с и без фильтра.
 */
void InternalContactsDialog::loadContacts()
{
    ui->listWidget->clear();

    ui->listWidget->addItems(m_extensions);

    for (qint32 i = 0; i < ui->listWidget->count(); ++i)
    {
        QString state = m_states.value(ui->listWidget->item(i)->text().remove(3, ui->listWidget->item(i)->text().length()));

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

    if (m_indexes.isEmpty())
        for (qint32 i = 0; i < ui->listWidget->count(); ++i)
            m_indexes.insert(ui->listWidget->item(i)->text().remove(QRegularExpression(" .+")), i);
}

void InternalContactsDialog::onExtenStatusChanged(const QString& exten, const QString& state)
{
    if (exten != g_personalNumber)
    {
        if (state == "0")
            ui->listWidget->item(m_indexes.value(exten))->setIcon(QIcon(":/images/presence-idle.png"));
        else if (state == "1" || state == "2" || state == "8" ||state == "9")
            ui->listWidget->item(m_indexes.value(exten))->setIcon(QIcon(":/images/presence-busy.png"));
        else if (state == "4")
            ui->listWidget->item(m_indexes.value(exten))->setIcon(QIcon(":/images/presence-unavial.png"));
        else if (state == "-2" || state == "-1")
            ui->listWidget->item(m_indexes.value(exten))->setIcon(QIcon(":/images/presence-dnd.png"));
        else if (state == "16" || state == "17")
            ui->listWidget->item(m_indexes.value(exten))->setIcon(QIcon(":/images/presence-hold.png"));
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
    QString from = g_personalNumber;
    QString to = ui->listWidget->currentItem()->text().remove(QRegExp(" .+"));
    QString protocol = global::getSettingsValue(from, "extensions").toString();

    g_asteriskManager->originateCall(from, to, protocol, from);
}

/**
 * Выполняет открытие окна добавления напоминания.
 */
void InternalContactsDialog::on_addReminderButton_clicked()
{
    QStringList employee;
    employee << ui->listWidget->currentItem()->text();

    m_addReminderDialog = new AddReminderDialog;
    m_addReminderDialog->receiveEmployee(employee);
    m_addReminderDialog->show();
    m_addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void InternalContactsDialog::on_listWidget_clicked()
{
    ui->callButton->setDisabled(false);
    ui->addReminderButton->setDisabled(false);
}
