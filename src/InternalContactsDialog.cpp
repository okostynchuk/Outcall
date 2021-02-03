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
        qint32 state = m_states.value(ui->listWidget->item(i)->text().remove(3, ui->listWidget->item(i)->text().length())).toInt();

        setIcon(i, state);
    }

    if (m_indexes.isEmpty())
        for (qint32 i = 0; i < ui->listWidget->count(); ++i)
            m_indexes.insert(ui->listWidget->item(i)->text().remove(QRegularExpression(" .+")), i);
}

/**
 * Изменение отображаемого статуса абонента
 */
void InternalContactsDialog::onExtenStatusChanged(const QString& exten, qint32& state)
{
    if (exten != g_personalNumber)
        setIcon(qint32(m_indexes.value(exten)), state);
}

/**
 * Установка отображаемого статуса абонента
 */
void InternalContactsDialog::setIcon(qint32 index, qint32 state)
{
    QString path;

    switch(state)
    {
        case Idle:
            path = ":/images/presence-idle.png";
            break;
        case InUse:
        case Busy:
        case Ringing:
        case InUseAndRinging:
            path = ":/images/presence-busy.png";
            break;
        case Unavailable:
            path = ":/images/presence-unavial.png";
            break;
        case Hold:
        case InUseAndHold:
            path = ":/images/presence-hold.png";
            break;
        case Removed:
        case HintRemoved:
            path = ":/images/presence-dnd.png";
            break;
    }

    ui->listWidget->item(index)->setIcon(QIcon(path));
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

/**
 * Активация кнопок при клике на элемент списка
 */
void InternalContactsDialog::on_listWidget_clicked()
{
    ui->callButton->setDisabled(false);
    ui->addReminderButton->setDisabled(false);
}
