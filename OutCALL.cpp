#include "OutCALL.h"
#include "DebugInfoDialog.h"
#include "SettingsDialog.h"
#include "ContactsDialog.h"
#include "CallHistoryDialog.h"
#include "Global.h"
#include "PlaceCallDialog.h"
#include "AsteriskManager.h"
#include "Notifier.h"
#include "PopupWindow.h"
#include "PopupHelloWindow.h"
#include "RemindersDialog.h"

#include <QMenu>
#include <QTcpSocket>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QProcess>

OutCall::OutCall() :
    QWidget()
{
    m_systemTryIcon       = new QSystemTrayIcon(this);
    m_menu                = new QMenu(this);
    m_settingsDialog      = new SettingsDialog;
    m_contactsDialog      = new ContactsDialog;
    m_debugInfoDialog     = new DebugInfoDialog;
    m_callHistoryDialog   = new CallHistoryDialog;
    m_placeCallDialog     = new PlaceCallDialog;
    m_remindersDialog     = new RemindersDialog;

    connect(m_systemTryIcon,    &QSystemTrayIcon::activated,            this, &OutCall::onActivated);

    connect(g_pAsteriskManager, &AsteriskManager::messageReceived,      this, &OutCall::onMessageReceived);
    connect(g_pAsteriskManager, &AsteriskManager::callReceived,         this, &OutCall::onCallReceived);
    connect(g_pAsteriskManager, &AsteriskManager::error,                this, &OutCall::displayError);
    connect(g_pAsteriskManager, &AsteriskManager::stateChanged,         this, &OutCall::onStateChanged);
    connect(&m_timer,           &QTimer::timeout,                       this, &OutCall::changeIcon);

    global::setSettingsValue("InstallDir", g_AppDirPath.replace("/", "\\"));

    createContextMenu();

    m_systemTryIcon->setContextMenu(m_menu);

    QString path(":/images/disconnected.png");
    m_systemTryIcon->setIcon(QIcon(path));

    automaticlySignIn();
}

OutCall::~OutCall()
{
    delete m_settingsDialog;
    delete m_debugInfoDialog;
    delete m_contactsDialog;
    delete m_callHistoryDialog;
    delete m_placeCallDialog;
    delete m_remindersDialog;
}

void OutCall::createContextMenu()
{
    // Exit action
    QAction* exitAction = new QAction(trUtf8("Выход"), m_menu);
    connect(exitAction, &QAction::triggered, this, &OutCall::close);

    // Sign In
    m_signIn  = new QAction(tr("Войти в аккаунт"), m_menu);
    connect(m_signIn, &QAction::triggered, this, &OutCall::signInOut);

    // Settings
    QAction* settingsAction = new QAction(tr("Настройки"), m_menu);
    connect(settingsAction, &QAction::triggered, this, &OutCall::onSettingsDialog);

    QAction* debugInfoAction = new QAction(tr("Отладка"), m_menu);
    connect(debugInfoAction, &QAction::triggered, this, &OutCall::onDebugInfo);

    // Contacts
    contactsAction = new QAction(tr("Контакты"), m_menu);
    connect(contactsAction, &QAction::triggered, this, &OutCall::onContactsDialog);

    // Call History
    callHistoryAction = new QAction(tr("История звонков"), m_menu);
    connect(callHistoryAction, &QAction::triggered, this, &OutCall::onCallHistory);

    // Reminders
    remindersAction = new QAction(tr("Напоминания"), m_menu);
    connect(remindersAction, &QAction::triggered, this, &OutCall::onRemindersDialog);

    // Place a Call
    m_placeCall = new QAction(tr("Позвонить"), 0);
    QFont font = m_placeCall->font();
    font.setBold(true);
    m_placeCall->setFont(font);
    connect(m_placeCall, &QAction::triggered, this, &OutCall::onPlaceCall);

    // Add actions
    m_menu->addSeparator();

    m_menu->addAction(m_placeCall);
    m_menu->addAction(callHistoryAction);
    m_menu->addAction(contactsAction);
    m_menu->addSeparator();

    m_menu->addAction(remindersAction);
    m_menu->addSeparator();

    m_menu->addAction(settingsAction);
    m_menu->addAction(debugInfoAction);
    m_menu->addSeparator();

    m_menu->addAction(m_signIn);
    m_menu->addSeparator();

    m_menu->addAction(exitAction);
}

void OutCall::automaticlySignIn()
{
    bool autoSingIn = global::getSettingsValue("auto_sign_in", "general").toBool();
    if(autoSingIn)
        signInOut();
}

void OutCall::signInOut()
{
    if (m_signIn->text() == "Выйти из аккаунта")
    {
        g_pAsteriskManager->signOut();
        return;
    }

    if (g_pAsteriskManager->isSignedIn() == false)
    {
        QString server = global::getSettingsValue("servername", "settings").toString();
        QString port = global::getSettingsValue("port", "settings", "5038").toString();
        g_pAsteriskManager->signIn(server, port.toUInt());
    }
    else
    {
        g_pAsteriskManager->signOut();
    }
}

void OutCall::displayError(QAbstractSocket::SocketError socketError, const QString &msg)
{
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        MsgBoxInformation(tr("Удаленный хост закрыл соединение"));
        break;
    case QAbstractSocket::HostNotFoundError:
        MsgBoxInformation(tr("Хост не был найден. Пожалуйста, проверьте имя хоста "
                             "и настройки порта"));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        MsgBoxInformation(tr("Соединение было отклонено узлом. "
                             "Убедитесь, что сервер работает, "
                             "и проверьте правильность имени хоста "
                             "и настроек порта"));
        break;
    default:
        MsgBoxInformation(msg);
    }
}

void OutCall::onMessageReceived(const QString &message)
{
    if (m_debugInfoDialog)
        m_debugInfoDialog->updateDebug(message);
}

void OutCall::onCallReceived(const QMap<QString, QVariant> &call)
{
    QString dateTime        = call.value("dateTime").toString();
    QString from            = call.value("from").toString();
    QString callerIDName    = call.value("callerIDName").toString();
    QString uniqueid        = call.value("uniqueid").toString();
    QString my_number        = call.value("to").toString();
    //QString context         = call.value("context").toString();

    if (from == callerIDName)
        callerIDName = "Неизвестный";

    PopupWindow::showCallNotification(dateTime, uniqueid, from, QString("%1 (%2)").arg(callerIDName).arg(from), my_number);
}

void OutCall::onStateChanged(AsteriskManager::AsteriskState state)
{
    if (state == AsteriskManager::CONNECTED)
    {
        QString path(":/images/connected.png");
        m_systemTryIcon->setIcon(QIcon(path));

        m_signIn->setText(tr("Выйти из аккаунта"));

        PopupHelloWindow::showInformationMessage(tr(APP_NAME), tr("Вы успешно вошли"));
        m_systemTryIcon->setToolTip(tr("") + tr("") + tr("Вы успешно вошли"));

        m_timer.stop();

        if(!opened)
        {
            QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Соединение восстановлено. Приложение будет перезапущено!"), QMessageBox::Ok);
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
    else if (state == AsteriskManager::CONNECTING)
    {
        m_signIn->setText(tr("Отменить вход"));
        m_systemTryIcon->setToolTip(tr("") + tr("") + tr("Вход в аккаунт"));

        m_timer.start(500);
    }
    else if (state == AsteriskManager::DISCONNECTED)
    {
        QString path(":/images/disconnected.png");
        m_systemTryIcon->setIcon(QIcon(path));

        m_signIn->setText(tr("&Войти в аккаунт"));
        m_systemTryIcon->setToolTip(tr("") + tr("") + tr("Вы не вошли"));

        disableActions();
        opened = false;

        m_timer.stop();
    }
    else if (state == AsteriskManager::AUTHENTICATION_FAILED)
    {
        QString path(":/images/started.png");
        m_systemTryIcon->setIcon(QIcon(path));

        PopupHelloWindow::showInformationMessage(tr(""), tr("Ошибка аутентификации"));
        m_systemTryIcon->setToolTip(tr("") + tr("") + tr("Не настроен"));
        m_signIn->setText(tr("&Войти в аккаунт"));

        disableActions();
        opened = false;

        m_timer.stop();
    }
}

void OutCall::disableActions()
{
    QApplication::closeAllWindows();
    if (m_contactsDialog->isVisible())
        m_contactsDialog->close();
    if (m_callHistoryDialog->isVisible())
        m_callHistoryDialog->close();
    m_placeCall->setEnabled(false);
    callHistoryAction->setEnabled(false);
    contactsAction->setEnabled(false);
}

void OutCall::changeIcon()
{
    if (m_switch)
    {
        QString path(":/images/connected.png");
        m_systemTryIcon->setIcon(QIcon(path));
        m_switch = false;
    }
    else
    {
        QString path(":/images/disconnected.png");
        m_systemTryIcon->setIcon(QIcon(path));
        m_switch = true;
    }
}

void OutCall::onCallHistory()
{
    m_callHistoryDialog->showNormal();
    m_callHistoryDialog->raise();
}

void OutCall::onSettingsDialog()
{
    SettingsDialog dialog;
    dialog.exec();
}

void OutCall::onDebugInfo()
{
    m_debugInfoDialog->show();
    m_debugInfoDialog->raise();
}

void OutCall::onPlaceCall()
{
    m_placeCallDialog->show();
    m_placeCallDialog->raise();
}

void OutCall::onContactsDialog()
{
    m_contactsDialog->showMaximized();
    m_contactsDialog->raise();
}

void OutCall::onRemindersDialog()
{
    m_remindersDialog->show();
    m_remindersDialog->raise();
}

void OutCall::close()
{
    g_pAsteriskManager->signOut();
    QApplication::quit();
}

void OutCall::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        m_debugInfoDialog->activateWindow();
        m_settingsDialog->activateWindow();
        m_callHistoryDialog->activateWindow();
        m_contactsDialog->activateWindow();
        m_placeCallDialog->activateWindow();
        m_remindersDialog->activateWindow();
    }
    else if (reason == QSystemTrayIcon::DoubleClick)
    {
        m_placeCallDialog->show();
        m_placeCallDialog->activateWindow();
    }
}

void OutCall::show()
{
    m_systemTryIcon->show();
}
