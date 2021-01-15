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
    explicit AsteriskManager(const QString& username, const QString& secret, QObject* parent = 0);
    ~AsteriskManager();

    enum AsteriskVersion
    {
        VERSION_11,
        VERSION_13
    };

    enum AsteriskState
    {
        CONNECTED,
        CONNECTING,
        DISCONNECTED,
        AUTHENTICATION_FAILED,
        ERROR_ON_CONNECTING
    };

    void originateCall(const QString& from, const QString& to, const QString& protocol, const QString& callerId);
    void originateAudio(const QString& number, const QString& protocol, QString recordpath);

    bool isSignedIn() const;
    void signIn(const QString& serverName, const quint16& port);
    void signOut();
    void reconnect();
    void setAutoSignIn(bool ok);
    void onSettingsChange();

    void setState(const AsteriskState& state);

    QMap<QString, QString> m_extensionNumbers;
    QMap<QString, QString> m_states;

    QStringList m_groupNumbers;

    AsteriskState m_currentState;

signals:
    void callStart(const QString& uniqueid);
    void messageReceived(const QString& message);
    void authenticationState(bool state);
    void callReceived(const QMap<QString, QVariant>& call);
    void error(const QAbstractSocket::SocketError& socketError, const QString& msg);
    void stateChanged(const AsteriskManager::AsteriskState& state);
    void extenStatusChanged(const QString& exten, qint32& state);

private slots:
    void login();
    void read();

    void getEventValues(const QString& eventData, QMap<QString, QString>& map);
    void parseEvent(const QString& eventData);
    void setAsteriskVersion(const QString& msg);
    void onError(const QAbstractSocket::SocketError& socketError);

    void getExtensionNumbers();
    void getGroups();
    void getExtensionsStateList();

    bool isGroup(QString* str);
    bool isInternalPhone(QString* str);

private:
    QTcpSocket* m_tcpSocket;

    AsteriskVersion m_currentVersion;

    QMap<QString, qint32> m_dialedNum;

    QList<QString> m_endpoints;

    bool m_isSignedIn;
    bool m_autoConnectingOnError;
    bool m_autoSignIn;

    QString m_eventData;
    QString m_username;
    QString m_secret;
    QString m_server;

    quint16 m_port;

    QTimer m_timer;
};

#endif // ASTERISKMANAGER_H
