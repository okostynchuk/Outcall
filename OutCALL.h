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
class InternalContactsDialog;

class OutCall : public QWidget
{
    Q_OBJECT

signals:
    void showReminders(bool);

public:
    OutCall();
    ~OutCall();

    void show();

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
    void onInternalContactsDialog();
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void displayError(QAbstractSocket::SocketError socketError, const QString &msg);
    void onStateChanged(AsteriskManager::AsteriskState state);
    void onMessageReceived(const QString &message);
    void onCallReceived(const QMap<QString, QVariant> &call);
    void close();
    void changeIcon();
    void disableActions();
    void enableActions();

protected:
    void automaticlySignIn();
    void createContextMenu();

private:
    QMenu *m_menu;

    QAction *m_signIn;
    QAction *m_placeCall;
    QAction *contactsAction;
    QAction *callHistoryAction;
    QAction *remindersAction;
    QAction *internalContactsAction;

    QSystemTrayIcon *m_systemTrayIcon;
    DebugInfoDialog *m_debugInfoDialog;
    SettingsDialog *m_settingsDialog;
    ContactsDialog *m_contactsDialog;
    CallHistoryDialog *m_callHistoryDialog;
    PlaceCallDialog *m_placeCallDialog;
    RemindersDialog *m_remindersDialog;
    InternalContactsDialog *m_internalContactsDialog;

    QTimer m_timer;
    bool m_switch;
    QString my_number;
};

#endif // OUTCALL_H
