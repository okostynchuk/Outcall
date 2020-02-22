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
    void signInOut();
    void onSettingsDialog();
    void onContactsInfo();
    void onDebugInfo();
    //void onActiveCalls();
    void onPlaceCall();
    void onCallHistory();
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void displayError(QAbstractSocket::SocketError socketError, const QString &msg);
    void onStateChanged(AsteriskManager::AsteriskState state);
    void onMessageReceived(const QString &message);
    void onCallReceived(const QMap<QString, QVariant> &call);
    void close();
    void changeIcon();
    void connectToDatabases();
    void enableActions();
    void disableActions();

private:
    QMenu *m_menu;
    QAction *m_signIn;
    QAction *m_placeCall;
    QAction *contactsInfoAction;
    QAction *callHistoryAction;
    QSystemTrayIcon *m_systemTryIcon;
    DebugInfoDialog *m_debugInfoDialog;
    SettingsDialog *m_settingsDialog;
    ContactsDialog *m_contactsDialog;
    CallHistoryDialog *m_callHistoryDialog;
    PlaceCallDialog *m_placeCallDialog;
    QTimer m_timer;
    bool m_switch;
};

#endif // OUTCALL_H
