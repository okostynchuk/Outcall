#ifndef OUTCALL_H
#define OUTCALL_H

#include "AsteriskManager.h"
#include "PopupWindow.h"

#include <QWidget>
#include <QMap>
#include <QSystemTrayIcon>
#include <QSqlDatabase>
#include <QColor>

class QSystemTrayIcon;
class QMenu;
class DebugInfoDialog;
class QTcpSocket;
class CallHistoryDialog;
class SettingsDialog;
class ContactsDialog;
class PlaceCallDialog;
class RemindersDialog;
class InternalContactsDialog;

class Outcall : public QWidget
{
    Q_OBJECT

signals:
    void showReminders(bool show);

public:
    Outcall();
    ~Outcall();

    void show();

protected slots:
    void hideTrayIcon(bool hide);
    void setIconReminders(QColor color, qint32 activeReminders);
    void changeIconReminders(bool change);
    void changeIcon();

    void signInOut();

    void onSettingsDialog();
    void onContactsDialog();
    void onDebugInfo();
    void onPlaceCall();
    void onCallHistory();
    void onRemindersDialog();
    void onInternalContactsDialog();

    void onActivated(const QSystemTrayIcon::ActivationReason& reason);

    void displayError(const QAbstractSocket::SocketError& socketError, const QString& msg);
    void onStateChanged(const AsteriskManager::AsteriskState& state);
    void onMessageReceived(const QString& message);

    void onCallReceived(const QMap<QString, QVariant>& call);

    void close();

    void disableActions();
    void enableActions();

protected:
    void automaticlySignIn();
    void createContextMenu();

private:
    QSqlDatabase db;

    QMenu* m_menu;

    QAction* m_signIn;
    QAction* m_placeCall;
    QAction* contactsAction;
    QAction* callHistoryAction;
    QAction* remindersAction;
    QAction* internalContactsAction;

    QSystemTrayIcon* m_systemTrayIcon;
    DebugInfoDialog* m_debugInfoDialog;
    SettingsDialog* m_settingsDialog;
    ContactsDialog* m_contactsDialog;
    CallHistoryDialog* m_callHistoryDialog;
    PlaceCallDialog* m_placeCallDialog;
    RemindersDialog* m_remindersDialog;
    InternalContactsDialog* m_internalContactsDialog;

    QTimer m_timer;

    bool m_switch;    
    bool show_call_popup;

    QString my_number;
};

#endif // OUTCALL_H
