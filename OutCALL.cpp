#include "OutCALL.h"

#include "DebugInfoDialog.h"
#include "SettingsDialog.h"
#include "ContactsDialog.h"
#include "CallHistoryDialog.h"
#include "Global.h"
#include "PlaceCallDialog.h"
#include "AsteriskManager.h"
#include "PopupWindow.h"
#include "PopupHelloWindow.h"
#include "RemindersDialog.h"
#include "PopupReminder.h"
#include "PopupNotification.h"

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
#include <QPainter>
#include <QPixmap>

OutCall::OutCall() :
    QWidget()
{
    m_systemTrayIcon      = new QSystemTrayIcon(this);
    m_menu                = new QMenu(this);
    m_settingsDialog      = new SettingsDialog;
    m_contactsDialog      = new ContactsDialog;
    m_debugInfoDialog     = new DebugInfoDialog;
    m_callHistoryDialog   = new CallHistoryDialog;
    m_placeCallDialog     = new PlaceCallDialog;
    m_remindersDialog     = new RemindersDialog;

    connect(m_systemTrayIcon,   &QSystemTrayIcon::activated,            this, &OutCall::onActivated);

    connect(g_pAsteriskManager, &AsteriskManager::messageReceived,      this, &OutCall::onMessageReceived);
    connect(g_pAsteriskManager, &AsteriskManager::callReceived,         this, &OutCall::onCallReceived);
    connect(g_pAsteriskManager, &AsteriskManager::error,                this, &OutCall::displayError);
    connect(g_pAsteriskManager, &AsteriskManager::stateChanged,         this, &OutCall::onStateChanged);
    connect(&m_timer,           &QTimer::timeout,                       this, &OutCall::changeIcon);

    connect(m_remindersDialog, SIGNAL(reminders(bool)), this, SLOT(changeIconReminders(bool)));
    connect(m_settingsDialog, SIGNAL(restart(bool)), this, SLOT(hideTrayIcon(bool)));

    connect(this, SIGNAL(showReminders(bool)), m_remindersDialog, SLOT(showReminders(bool)));

    global::setSettingsValue("InstallDir", g_AppDirPath.replace("/", "\\"));

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    createContextMenu();

    m_systemTrayIcon->setContextMenu(m_menu);

    QString path(":/images/disconnected.png");
    m_systemTrayIcon->setIcon(QIcon(path));

    g_pAsteriskManager->setAutoSignIn(global::getSettingsValue("auto_sign_in", "general", true).toBool());

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
    QAction* exitAction = new QAction(QObject::tr("Выход"), m_menu);
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
    //m_menu->addAction(debugInfoAction);
    m_menu->addSeparator();

    m_menu->addAction(m_signIn);
    m_menu->addSeparator();

    m_menu->addAction(exitAction);
}

void OutCall::automaticlySignIn()
{
    bool autoSingIn = global::getSettingsValue("auto_sign_in", "general", true).toBool();

    if (autoSingIn)
        signInOut();
}

void OutCall::signInOut()
{
    if (m_signIn->text() == tr("Выйти из аккаунта") || m_signIn->text() == tr("Отменить вход"))
    {
        g_pAsteriskManager->signOut();

        return;
    }

    if (g_pAsteriskManager->isSignedIn() == false)
    {
        QString server = global::getSettingsValue("servername", "settings").toString();
        QString port = global::getSettingsValue("port", "settings").toString();

        g_pAsteriskManager->signIn(server, port.toUInt());
    }
    else
        g_pAsteriskManager->signOut();
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
    QString my_number       = call.value("to").toString();

    if (from == callerIDName)
        callerIDName = QObject::tr("Неизвестный");

    PopupWindow::showCallNotification(dateTime, uniqueid, from, QString("<b style='color:white'>%1</b><br><b>%2</b>").arg(from).arg(callerIDName), my_number);
}

void OutCall::onStateChanged(AsteriskManager::AsteriskState state)
{
    if (state == AsteriskManager::CONNECTED)
    {
        QSqlDatabase::database().open();
        QSqlDatabase::database("Calls").open();

        if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database("Calls").isOpen())
            opened = false;

        m_signIn->setText(tr("Выйти из аккаунта"));

        PopupHelloWindow::showInformationMessage(tr(APP_NAME), tr("Вы успешно вошли"));
        m_systemTrayIcon->setToolTip(tr("") + tr("") + tr("Вы успешно вошли"));

        m_timer.stop();

        if (!opened)
        {
            m_systemTrayIcon->hide();

            qApp->closeAllWindows();
            qApp->quit();

            QProcess::startDetached(qApp->arguments()[0], QStringList() << "restart");
        }
        else
        {
            changeIconReminders(false);

            enableActions();

            emit showReminders(true);
        }
    }
    else if (state == AsteriskManager::CONNECTING)
    {
        m_signIn->setText(tr("Отменить вход"));
        m_systemTrayIcon->setToolTip(tr("") + tr("") + tr("Вход в аккаунт"));

        disableActions();

        m_timer.start(500);
    }
    else if (state == AsteriskManager::DISCONNECTED)
    {
        QString path(":/images/disconnected.png");
        m_systemTrayIcon->setIcon(QIcon(path));

        m_signIn->setText(tr("&Войти в аккаунт"));
        m_systemTrayIcon->setToolTip(tr("") + tr("") + tr("Вы не вошли"));

        emit showReminders(false);

        PopupHelloWindow::closeAll();
        PopupWindow::closeAll();
        PopupNotification::closeAll();
        PopupReminder::closeAll();

        QApplication::closeAllWindows();

        disableActions();

        m_timer.stop();
    }
    else if (state == AsteriskManager::AUTHENTICATION_FAILED)
    {
        QString path(":/images/started.png");
        m_systemTrayIcon->setIcon(QIcon(path));

        PopupHelloWindow::showInformationMessage(tr(""), tr("Ошибка аутентификации"));

        m_systemTrayIcon->setToolTip(tr("") + tr("") + tr("Не настроен"));
        m_signIn->setText(tr("&Войти в аккаунт"));

        emit showReminders(false);

        PopupHelloWindow::closeAll();
        PopupWindow::closeAll();
        PopupNotification::closeAll();
        PopupReminder::closeAll();

        QApplication::closeAllWindows();

        disableActions();

        m_timer.stop();
    }
}

void OutCall::disableActions()
{
    m_placeCall->setEnabled(false);
    callHistoryAction->setEnabled(false);
    contactsAction->setEnabled(false);
    remindersAction->setEnabled(false);
}

void OutCall::enableActions()
{
    m_placeCall->setEnabled(true);
    callHistoryAction->setEnabled(true);
    contactsAction->setEnabled(true);
    remindersAction->setEnabled(true);
}

void OutCall::hideTrayIcon(bool hide)
{
    if (hide)
        m_systemTrayIcon->hide();
}

void OutCall::changeIconReminders(bool changing)
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_from <> ? AND phone_to = ? AND viewed = false");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    int receivedReminders = 0;

    if (query.next())
        receivedReminders = query.value(0).toInt();

    query.prepare("SELECT COUNT(*) FROM reminders WHERE phone_to = ? AND active = true");
    query.addBindValue(my_number);
    query.exec();

    int activeReminders = 0;

    if (query.next())
        activeReminders = query.value(0).toInt();

    if (changing)
    {
        QPixmap pixmap(22, 22);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0, 0, QPixmap(":/images/connectedTray.png"));
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap));
        painter.setBrush(QBrush(Qt::red));
        painter.drawEllipse(4, 4, 18, 18);
        QFont font("MS Shell Dlg 2", 8);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRect(4, 7, 18, 13), Qt::AlignCenter, QString::number(activeReminders));
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.end();

        m_systemTrayIcon->setIcon(QIcon(pixmap));
    }
    else if (!changing && activeReminders > 0 && receivedReminders > 0)
    {
        QPixmap pixmap(22, 22);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0, 0, QPixmap(":/images/connectedTray.png"));
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap));
        painter.setBrush(QBrush(Qt::red));
        painter.drawEllipse(4, 4, 18, 18);
        QFont font("MS Shell Dlg 2", 8);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRect(4, 7, 18, 13), Qt::AlignCenter, QString::number(activeReminders));
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.end();

        m_systemTrayIcon->setIcon(QIcon(pixmap));
    }
    else if (!changing && activeReminders > 0 && receivedReminders == 0)
    {
        QPixmap pixmap(22, 22);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.drawPixmap(0, 0, QPixmap(":/images/connectedTray.png"));
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::SquareCap));
        painter.setBrush(QBrush(Qt::lightGray));
        painter.drawEllipse(4, 4, 18, 18);
        QFont font("MS Shell Dlg 2", 8);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(QRect(4, 7, 18, 13), Qt::AlignCenter, QString::number(activeReminders));
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.end();

        m_systemTrayIcon->setIcon(QIcon(pixmap));
    }
    else if (!changing && activeReminders == 0)
    {
        QString path(":/images/connected.png");
        m_systemTrayIcon->setIcon(QIcon(path));
    }
}

void OutCall::changeIcon()
{
    if (m_switch)
    {
        QString path(":/images/connected.png");
        m_systemTrayIcon->setIcon(QIcon(path));

        m_switch = false;
    }
    else
    {
        QString path(":/images/disconnected.png");
        m_systemTrayIcon->setIcon(QIcon(path));

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
//    SettingsDialog dialog;
//    dialog.exec();
    m_settingsDialog->showNormal();
    m_settingsDialog->raise();
}

void OutCall::onDebugInfo()
{
    m_debugInfoDialog->showNormal();
    m_debugInfoDialog->raise();
}

void OutCall::onPlaceCall()
{
    m_placeCallDialog->showNormal();
    m_placeCallDialog->raise();
}

void OutCall::onContactsDialog()
{
    m_contactsDialog->showMaximized();
    m_contactsDialog->raise();
}

void OutCall::onRemindersDialog()
{
    m_remindersDialog->showNormal();
    m_remindersDialog->raise();

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET viewed = true WHERE phone_from <> ? AND phone_to = ?");
    query.addBindValue(my_number);
    query.addBindValue(my_number);
    query.exec();

    m_remindersDialog->reminders(false);
}

void OutCall::close()
{
    g_pAsteriskManager->signOut();

    m_systemTrayIcon->hide();

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
        if (opened && g_pAsteriskManager->m_currentState == AsteriskManager::CONNECTED)
        {
            m_placeCallDialog->showNormal();
            m_placeCallDialog->activateWindow();
        }
    }
}

void OutCall::show()
{
    m_systemTrayIcon->show();
}
