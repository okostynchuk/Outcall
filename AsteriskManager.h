#ifndef ASTERISKMANAGER_H
#define ASTERISKMANAGER_H

#include <QMap>
#include <QTcpSocket>
#include <QObject>
#include <QTimer>

class AsteriskManager : public QObject
{
    Q_OBJECT

public:
    explicit AsteriskManager(const QString username, const QString secret, QObject *parent = 0);
    ~AsteriskManager();

    enum CallState
    {
        MISSED = 0,
        RECIEVED = 1,
        PLACED = 2
    };

    enum AsteriskState
    {
        CONNECTED,
        CONNECTING,
        DISCONNECTED,
        AUTHENTICATION_FAILED,
        ERROR_ON_CONNECTING
    };

    enum AsteriskVersion
    {
        VERSION_11,
        VERSION_13
    };

    struct Call
    {
        QString chExten;
        QString chType;
        QString exten;
        QString state;
        QString callerName;
        QString destUniqueid;
    };

    void originateCall(QString from, QString to,  QString protocol, QString callerId);
    void originateAudio(QString number, QString protocol, QString recordpath);

    bool isSignedIn() const;
    void signIn(const QString &serverName, const quint16 &port);
    void signOut();
    void reconnect();
    void setAutoSignIn(bool ok);
    void onSettingsChange();

    void setState(AsteriskState state);
    void getExtensionNumbers();

    QMap<QString, QString> extensionNumbers;
    AsteriskState m_currentState;

signals:
    void callStart(QString);
    void messageReceived(const QString &message);
    void authenticationState(bool state);
    void callDeteceted(const QMap<QString, QVariant> &call, CallState state);
    void callReceived(const QMap<QString, QVariant>&);
    void error(QAbstractSocket::SocketError socketError, const QString &msg);
    void stateChanged(AsteriskState state);

protected slots:
    void onError(QAbstractSocket::SocketError socketError);
    void read();
    void login();

protected:
    void getEventValues(QString eventData, QMap<QString, QString> &map);
    void parseEvent(const QString &event);
    void asterisk_11_eventHandler(const QString &eventData);
    void setAsteriskVersion(const QString &msg);

private:
    QTcpSocket* m_tcpSocket;

    AsteriskVersion m_currentVersion;

    QMap<QString, Call*> m_calls;
    QMap<QString, int> m_dialedNum;
    QList<QString> endpoints;

    QString m_eventData;
    bool m_isSignedIn;
    bool m_autoConnectingOnError;
    QString m_username;
    QString m_secret;
    QString m_server;
    quint16 m_port;
    QTimer m_timer;
    bool m_autoSignIn;
};

extern AsteriskManager* g_pAsteriskManager;

#endif // ASTERISKMANAGER_H
