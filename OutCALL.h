#ifndef OUTCALL_H
#define OUTCALL_H

#include "AsteriskManager.h"
#include "PopupWindow.h"

#include <QWidget>
#include <QMap>
#include <QSystemTrayIcon>

class QSystemTrayIcon;
class QMenu;
class DebugInfoDialog;
class QTcpSocket;
class CallHistoryDialog;
class SettingsDialog;
class ContactsDialog;
class PlaceCallDialog;
class RemindersDialog;

class OutCall : public QWidget
{
    Q_OBJECT

public:
    OutCall();
    ~OutCall();

    void show();

protected:
    void automaticlySignIn();
    void createContextMenu();

protected slots:
    void hideTrayIcon(bool);
    void changeIconReminders(bool);
    void signInOut();
    void onSettingsDialog();
    void onContactsDialog();
    void onDebugInfo();
    //void onActiveCalls();
    void onPlaceCall();
    void onCallHistory();
    void onRemindersDialog();
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void displayError(QAbstractSocket::SocketError socketError, const QString &msg);
    void onStateChanged(AsteriskManager::AsteriskState state);
    void onMessageReceived(const QString &message);
    void onCallReceived(const QMap<QString, QVariant> &call);
    void close();
    void changeIcon();
    void disableActions();
    void enableActions();

private:
    QMenu *m_menu;
    QAction *m_signIn;
    QAction *m_placeCall;
    QAction *contactsAction;
    QAction *callHistoryAction;
    QAction *remindersAction;
    QSystemTrayIcon *m_systemTrayIcon;
    DebugInfoDialog *m_debugInfoDialog;
    SettingsDialog *m_settingsDialog;
    ContactsDialog *m_contactsDialog;
    CallHistoryDialog *m_callHistoryDialog;
    PlaceCallDialog *m_placeCallDialog;
    RemindersDialog *m_remindersDialog;
    QTimer m_timer;
    bool m_switch;
    QString my_number;
    int oldReceivedReminders;
};

#endif // OUTCALL_H
