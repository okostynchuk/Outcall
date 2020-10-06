/*
 * Класс служит для взаимодействия с сервером Asterisk.
 */

#include "AsteriskManager.h"

#include "Global.h"

#include <QTcpSocket>
#include <QTime>
#include <QVariantList>
#include <QVariantMap>
#include <QTimer>
#include <QtGlobal>
#include <QDebug>

AsteriskManager* g_pAsteriskManager = nullptr;

AsteriskManager::AsteriskManager(const QString& username, const QString& secret, QObject* parent)
    : QObject(parent),
      m_isSignedIn(false),
      m_autoConnectingOnError(false),
      m_username(username),
      m_secret(secret)
{
    g_pAsteriskManager = this;

    m_tcpSocket = new QTcpSocket(this);

    void (QAbstractSocket:: *sig)(QAbstractSocket::SocketError) = &QAbstractSocket::error;

    connect(m_tcpSocket, &QIODevice::readyRead,       this, &AsteriskManager::read);
    connect(m_tcpSocket, &QAbstractSocket::connected, this, &AsteriskManager::login);
    connect(m_tcpSocket, sig,                         this, &AsteriskManager::onError);
    connect(&m_timer,    &QTimer::timeout,            this, &AsteriskManager::reconnect);

    m_currentState = DISCONNECTED;
}

AsteriskManager::~AsteriskManager()
{}

/**
 * Выполняет подключение к серверу Asterisk.
 */
void AsteriskManager::signIn(const QString& serverName, const quint16& port)
{
    if (!m_isSignedIn && (m_currentState == DISCONNECTED || m_currentState == ERROR_ON_CONNECTING))
    {
        m_server = serverName;
        m_port   = port;

        setState(CONNECTING);

        m_tcpSocket->connectToHost(serverName, port);
    }
}

/**
 * Выполняет отключение от сервера Asterisk.
 */
void AsteriskManager::signOut()
{
    m_tcpSocket->abort();

    m_isSignedIn = false;
    m_autoConnectingOnError = false;

    m_timer.stop();

    setState(DISCONNECTED);
}

/**
 * Выполняет обработку события неконтролируемого
 * отключения от сервера Asterisk.
 */
void AsteriskManager::onError(const QAbstractSocket::SocketError& socketError)
{
    if (m_currentState == CONNECTING && !m_autoConnectingOnError)
    {
        QString msg = m_tcpSocket->errorString();

        emit error(socketError, msg);

        setState(DISCONNECTED);
    }
    else if (m_currentState == CONNECTED)
    {
        setState(DISCONNECTED);

        if (!m_timer.isActive() && m_autoSignIn)
            m_timer.start(10000); // 10 sec to reconnect
    }

    setState(ERROR_ON_CONNECTING);
}

/**
 * Выполняет операции для последующего переподключения к серверу Asterisk.
 */
void AsteriskManager::reconnect()
{
    if (!m_autoSignIn)
    {
        m_timer.stop();

        m_autoConnectingOnError = false;

        return;
    }

    m_autoConnectingOnError = true;

    signIn(m_server, m_port);
}

/**
 * Получает и присваивает значение для включения / отключения
 * автоматического подключения к серверу Asterisk.
 */
void AsteriskManager::setAutoSignIn(bool ok)
{
    m_autoSignIn = ok;
}

/**
 * Получает и присваивает значение состояния подключения к серверу Asterisk.
 */
void AsteriskManager::setState(const AsteriskState& state)
{
    if (state == m_currentState)
        return;

    if (state == CONNECTED)
    {
        m_currentState = CONNECTED;

        emit stateChanged(CONNECTED);
    }
    else if (state == CONNECTING)
    {
        m_currentState = CONNECTING;

        emit stateChanged(CONNECTING);
    }
    else if (state == DISCONNECTED)
    {
        m_currentState = DISCONNECTED;

        m_isSignedIn = false;

        emit stateChanged(DISCONNECTED);
    }
    else if (state == AUTHENTICATION_FAILED)
    {
        m_currentState = DISCONNECTED;

        m_isSignedIn = false;

        emit stateChanged(AUTHENTICATION_FAILED);
    }
    else if (state == ERROR_ON_CONNECTING)
        m_currentState = ERROR_ON_CONNECTING;
}

/**
 * Получает и присваивает значение используемой версии Asterisk.
 */
void AsteriskManager::setAsteriskVersion(const QString& msg)
{
    qint32 index = msg.indexOf("/") + 1;

    QString ami = msg.mid(index);

    if (ami.at(0) == '1')
        m_currentVersion = VERSION_11;
    else if (ami.at(0) == '2')
        m_currentVersion = VERSION_13;
}

/**
 * Выполняет считывание сообщения, полученного через TCP сокет от сервера Asterisk.
 */
void AsteriskManager::read()
{
    while (m_tcpSocket->canReadLine())
    {
        QString message = QString::fromUtf8(m_tcpSocket->readLine());

        if (m_isSignedIn == false)
        {
            QString msg = message.trimmed();

            if (msg.contains("Asterisk Call Manager"))
                setAsteriskVersion(msg);

            if (msg == "Message: Authentication accepted")
            {
                m_isSignedIn = true;
                m_autoConnectingOnError = false;

                m_timer.stop();

                setState(CONNECTED);
            }
            else if (msg == "Message: Authentication failed")
            {
                m_timer.stop();

                m_tcpSocket->abort();

                m_isSignedIn = false;
                m_autoConnectingOnError = false;

                setState(AUTHENTICATION_FAILED);
            }
        }
        else
        {
            if (message != "\r\n")
                m_eventData.append(message);
            else if (!m_eventData.isEmpty())
            {
                if (m_currentVersion == VERSION_13)
                    parseEvent(m_eventData);
                else if (m_currentVersion == VERSION_11)
                    asterisk_11_eventHandler(m_eventData);

                m_eventData.clear();
            }
        }

        //emit messageReceived(message.trimmed());
    }
}

/**
 * Выполняет обработку событий, полученных в сообщении (Asterisk 13).
 */
void AsteriskManager::parseEvent(const QString& eventData)
{
    if (eventData.contains("Event: Newchannel"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);
        QString uniqueid     = eventValues.value("Uniqueid");
        QString callerIdName = eventValues.value("CallerIDName");
        QString ch           = eventValues.value("Channel");

        QRegExp reg("([^/]*)(/)(\\d+)");
        reg.indexIn(ch);
        QString chType = reg.cap(1);
        QString chExten = reg.cap(3);

        if (!global::containsSettingsKey(chExten, "extensions"))
            return;

        QString type = global::getSettingsValue(chExten, "extensions").toString();
        if (type == chType)
        {
            Call* call       = new Call;
            call->callerName = callerIdName;
            call->chType     = chType;
            call->chExten    = chExten;

            m_calls.insert(uniqueid, call);
        }
    }
    else if (eventData.contains("Event: EndpointList"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        endpoints.append(eventValues.value("Aor"));

        if (eventValues.value("Event") == "EndpointListComplete")
        {
            for (qint32 i = 0; i < endpoints.length(); ++i)
            {
                QString command;
                command   = "Action: PJSIPShowEndpoint\r\n";
                command  += "Endpoint: " + endpoints.at(i) + "\r\n";

                m_tcpSocket->write(command.toLatin1().data());
                m_tcpSocket->write("\r\n");
                m_tcpSocket->flush();
            }
        }
    }
    else if (eventData.contains("Event: EndpointDetail"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        if (eventValues.value("Event") == "EndpointDetailComplete")
            return;

        const QString callerId = eventValues.value("Callerid");
        QString name;
        QString number;

        QRegExp reg("([\\][\"])(.+)([\\][\"] [<])([0-9]+)([>])");
        reg.indexIn(callerId);
        name = reg.cap(2);
        number = reg.cap(4);

        extensionNumbers.insert(number, name);
    }
    else if (eventData.contains("Event: BlindTransfer"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);
    }
    else if (eventData.contains("Event: AttendedTransfer"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        const QString number         = eventValues.value("TransferTargetCallerIDNum");
        const QString uniqueid       = eventValues.value("TransfereeUniqueid");
        const QString callerIdName   = eventValues.value("TransfereeCallerIDName");
        const QString callerIdNum    = eventValues.value("TransfereeCallerIDNum");

        QString dateTime = QTime::currentTime().toString();

        if (global::containsSettingsKey(number, "extensions"))
        {
            QMap<QString, QVariant> received;

            received.insert("dateTime", dateTime);
            received.insert("from", callerIdNum);
            received.insert("to", number);
            received.insert("callerIdName", callerIdName);
            received.insert("uniqueid", uniqueid);

            qint32 counter = m_dialedNum.value(uniqueid, 0);

            counter++;

            m_dialedNum.insert(uniqueid, counter);

            if (counter == 1)
            {
                emit callReceived(received);
                emit callStart(uniqueid);
            }
        }
    }
    else if (eventData.contains("Event: Newexten"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        QString exten       = eventValues.value("Exten");
        QString uniqueid    = eventValues.value("Uniqueid");
        QString state       = eventValues.value("ChannelStateDesc");
        QString appData     = eventValues.value("AppData");

        if (exten == "s" || exten == "h" || !m_calls.contains(uniqueid) || exten.isEmpty())
            return;

        if (appData == "(Outgoing Line)")
            return;

        Call* call = m_calls.value(uniqueid);
        call->state = state;

        if (call->exten.isEmpty())
        {
            QString dateTime = QDateTime::currentDateTime().toString();

            call->exten = exten;

            m_calls.insert(uniqueid, call);

            QMap<QString, QVariant> placed;

            placed.insert("from",       call->chExten);
            placed.insert("to",         call->exten);
            placed.insert("protocol",   call->chType);
            placed.insert("date_time",  dateTime);

            QList<QVariant> list = global::getSettingsValue("placed", "calls", QVariantList()).toList();

            if (list.size() >= 50)
                list.removeFirst();

            list.append(QVariant::fromValue(placed));

            global::setSettingsValue("placed", list, "calls");

            emit callDeteceted(placed, PLACED);
        }
    }
    else if (eventData.contains("Event: DialBegin"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        const QString channelStateDesc  = eventValues.value("ChannelStateDesc");
        const QString callerIdNum       = eventValues.value("CallerIDNum");
        QString destExten               = eventValues.value("DestExten");
        const QString destChannel       = eventValues.value("DestChannel");
        const QString callerIdName      = eventValues.value("CallerIDName");
        const QString uniqueid          = eventValues.value("Uniqueid");
        const QString context           = eventValues.value("Context");
        const QString linkedid          = eventValues.value("Linkedid");
        QString destProtocol;

        QString dateTime = QTime::currentTime().toString();

        QRegExp reg("([^/]*)(/)(\\d+)");
        reg.indexIn(destChannel);
        destProtocol = reg.cap(1);
        destExten = reg.cap(3);

        if (channelStateDesc == "Ring" || channelStateDesc == "Up")
        {
            if (global::containsSettingsKey(destExten, "extensions"))
            {
                QString protocol        = global::getSettingsValue(destExten, "extensions").toString();
                bool recievedProtocol   = false;

                if (protocol == destProtocol)
                    recievedProtocol = true;

                if (protocol == "SIP" && destProtocol  == "PJSIP")
                    recievedProtocol = true;

                if (recievedProtocol)
                {
                    QMap<QString, QVariant> received;

                    received.insert("dateTime", dateTime);
                    received.insert("from", callerIdNum);
                    received.insert("to", destExten);
                    received.insert("protocol", destProtocol);
                    received.insert("callerIdName", callerIdName);
                    received.insert("uniqueid", uniqueid);
                    received.insert("context", context);
                    received.insert("linkedid", linkedid);

                    qint32 counter = m_dialedNum.value(uniqueid, 0);

                    counter++;

                    m_dialedNum.insert(uniqueid, counter);

                    if (counter == 1)
                        emit callReceived(received);
                }
            }
        }
    }
    else if (eventData.contains("Event: DialEnd"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        QString channelStateDesc = eventValues.value("ChannelStateDesc");
        QString callerIdNum      = eventValues.value("CallerIDNum");
        QString exten            = eventValues.value("Exten");
        QString destChannel      = eventValues.value("DestChannel");
        QString dialStatus       = eventValues.value("DialStatus");
        QString callerIdName     = eventValues.value("CallerIDName");
        QString uniqueid         = eventValues.value("Uniqueid");

        qint32 index                = destChannel.indexOf("/");
        QString destProtocol     = destChannel.mid(0, index);

        QRegExp reg("([^/]*)(/)(\\d+)");
        reg.indexIn(destChannel);
        destProtocol = reg.cap(1);
        exten = reg.cap(3);

        if (channelStateDesc == "Ring" || channelStateDesc == "Up")
        {
            QString protocol = global::getSettingsValue(exten, "extensions").toString();

            bool isProtocolOk = false;

            if (protocol == destProtocol)
                isProtocolOk = true;
            else if (destProtocol == "PJSIP" && protocol == "SIP")
                isProtocolOk = true;

            if (global::containsSettingsKey(exten, "extensions") && isProtocolOk)
            {
                if (dialStatus == "ANSWER")
                {
                    QString dateTime = QDateTime::currentDateTime().toString();

                    QMap<QString, QVariant> received;

                    received.insert("from",         callerIdNum);
                    received.insert("to",           exten);
                    received.insert("protocol",     destProtocol);
                    received.insert("date_time",    dateTime);
                    received.insert("callerIdName", callerIdName);

                    QList<QVariant> list = global::getSettingsValue("received", "calls").toList();

                    if (list.size() >= 50)
                        list.removeFirst();

                    list.append(QVariant::fromValue(received));

                    global::setSettingsValue("received", list, "calls");

                    m_dialedNum.remove(uniqueid);

                    emit callStart(uniqueid);

                    emit callDeteceted(received, RECIEVED);
                }
                else if (dialStatus == "CANCEL" || dialStatus == "BUSY" || dialStatus == "NOANSWER")
                {
                    qint32 counter = 0;

                    if (m_dialedNum.contains(uniqueid))
                        counter = m_dialedNum.value(uniqueid, 0);
                    else
                        return;

                    if (counter > 1)
                    {
                        counter--;

                        m_dialedNum.insert(uniqueid, counter);

                        return;
                    }

                    m_dialedNum.remove(uniqueid);

                    QString date = QDateTime::currentDateTime().toString();

                    QMap<QString, QVariant> missed;

                    missed.insert("from",           callerIdNum);
                    missed.insert("to",             exten);
                    missed.insert("protocol",       destProtocol);
                    missed.insert("date_time",      date);
                    missed.insert("callerIdName",   callerIdName);

                    QList<QVariant> list = global::getSettingsValue("missed", "calls", QVariantList()).toList();

                    if (list.size() >= 50)
                        list.removeFirst();

                    list.append(QVariant::fromValue(missed));

                    global::setSettingsValue("missed", list, "calls");

                    emit callDeteceted(missed, MISSED);
                }
            }
        }
    }
    else if (eventData.contains("Event: Hangup"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        const QString uniqueid = eventValues.value("Uniqueid");

        if (m_calls.contains(uniqueid))
        {
            Call* call = m_calls.value(uniqueid);
            delete call;
            m_calls.remove(uniqueid);
        }
    }
}

/**
 * Выполняет преобразование полученного сообщения в список пар "ключ: значение".
 */
void AsteriskManager::getEventValues(const QString& eventData, QMap<QString, QString> &map)
{
    QStringList list = eventData.split("\r\n");

    list.removeLast();

    foreach (QString values, list)
    {
        QStringList c = values.split(": ");
        map.insert(c.at(0), c.at(1));
    }
}

/**
 * Выполняет отправку команды для получения списка внутренних номеров.
 */
void AsteriskManager::getExtensionNumbers()
{
    extensionNumbers.clear();

    QString command;
    command = "Action: PJSIPShowEndpoints\r\n";

    m_tcpSocket->write(command.toLatin1().data());
    m_tcpSocket->write("\r\n");
    m_tcpSocket->flush();
}

/**
 * Выполняет отправку команды для входа в аккаунт.
 */
void AsteriskManager::login()
{
    QString login;
    login =  "Action: Login\r\n";
    login += "Username: " + m_username + "\r\n";
    login += "Secret: " + m_secret + "\r\n";

    m_tcpSocket->write(login.toLatin1().data());
    m_tcpSocket->write("\r\n");
    m_tcpSocket->flush();

    getExtensionNumbers();
}

/**
 * Выполняет отправку команды для совершения звонка.
 */
void AsteriskManager::originateCall(const QString& from, const QString& to, const QString& protocol, const QString& callerId)
{
    const QString channel = protocol + "/" + from;

    QString result;
    result =  "Action: Originate\r\n";
    result += "Channel: " + channel + "\r\n";
    result += "Exten: " + to + "\r\n";
    result += "Context: DLPN_DialPlan" + from + "\r\n";
    result += "Priority: 1\r\n";
    result += "CallerID: " + callerId + "\r\n";

    m_tcpSocket->write(result.toLatin1().data());
    m_tcpSocket->write("\r\n");
    m_tcpSocket->flush();
}

/**
 * Выполняет отправку команды для аудио воспроизведения записи звонка.
 */
void AsteriskManager::originateAudio(const QString& number, const QString& protocol, QString recordpath)
{
    const QString channel = protocol + "/" + number;

    recordpath.remove(recordpath.length() - 4, recordpath.length());

    QString result;
    result =  "Action: Originate\r\n";
    result += "Channel: " + channel + "\r\n";
    result += "CallerID: " + number + "\r\n";
    result += "Application: Playback\r\n";
    result += "Data: " + recordpath + "\r\n";

    m_tcpSocket->write(result.toLatin1().data());
    m_tcpSocket->write("\r\n");
    m_tcpSocket->flush();
}

/**
 * Выполняет изменение настроек подключения к серверу Asterisk.
 */
void AsteriskManager::onSettingsChange()
{
    QString server            = global::getSettingsValue("servername", "settings").toString();
    quint16 port              = global::getSettingsValue("port",       "settings").toUInt();
    QString username          = global::getSettingsValue("username",   "settings").toString();
    QByteArray secretInByte   = global::getSettingsValue("password",   "settings").toByteArray();
    QString secret            = QString(QByteArray::fromBase64(secretInByte));

    if (server != m_server || port != m_port ||
            username != m_username || secret != m_secret)
    {
        m_server    = server;
        m_port      = port;
        m_username  = username;
        m_secret    = secret;

        signOut();
        signIn(m_server, m_port);
    }
}

/**
 * Возвращает состояние подключения к серверу Asterisk.
 */
bool AsteriskManager::isSignedIn() const
{
    return m_isSignedIn;
}

/**
 * Выполняет обработку событий, полученных в сообщении (Asterisk 11).
 */
void AsteriskManager::asterisk_11_eventHandler(const QString& eventData)
{
    if (eventData.contains("Event: Newchannel"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        QString uniqueid         = eventValues.value("Uniqueid");
        QString channel          = eventValues.value("Channel");
        QString calledNum        = eventValues.value("Exten");
        QString state            = eventValues.value("ChannelStateDesc");
        QString callerIdName     = eventValues.value("CallerIDName");

        QRegExp reg("([^/]*)(/)(\\d+)");
        reg.indexIn(channel);

        QString protocol    = reg.cap(1);
        QString exten       = reg.cap(3);

        if (global::containsSettingsKey(exten, "extensions"))
        {
            Call* call          = new Call;
            call->chExten       = exten;
            call->exten         = calledNum;
            call->state         = state;
            call->chType        = protocol;
            call->callerName    = callerIdName;

            m_calls.insert(uniqueid, call);
        }
    }
    else if (eventData.contains("Event: Newstate"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        QString channelStateDesc = eventValues.value("ChannelStateDesc");
        QString uniqueid         = eventValues.value("Uniqueid");
        QString channel          = eventValues.value("Channel");
        QString callerIdName     = eventValues.value("ConnectedLineName");
        QString connectedLineNum = eventValues.value("ConnectedLineNum");

        if (!m_calls.contains(uniqueid))
            return;

        QRegExp reg("([^/]*)(/)(\\d+)");
        reg.indexIn(channel);

        QString protocol    = reg.cap(1);
        QString exten       = reg.cap(3);

        bool processEvent = false;
        if (global::containsSettingsKey(exten, "extensions"))
        {
            QString userProtocol = global::getSettingsValue(exten, "extensions").toString();
            processEvent = userProtocol == protocol ? true : false;
        }

        if (exten == connectedLineNum)
            return;

        if (processEvent)
        {
            QString dateTime = QDateTime::currentDateTime().toString();

            if (channelStateDesc == "Ringing")
            {
                QMap<QString, QVariant> received;

                received.insert("from", connectedLineNum);
                received.insert("callerIdName", callerIdName);

                Call* call = m_calls.value(uniqueid);
                call->state      = channelStateDesc;
                call->exten         = connectedLineNum;

                m_calls.insert(uniqueid, call);

                emit callReceived(received);
            }
            if (channelStateDesc == "Up")
            {
                QMap<QString, QVariant> received;

                received.insert("from",         connectedLineNum);
                received.insert("to",           exten);
                received.insert("protocol",     protocol);
                received.insert("date_time",    dateTime);
                received.insert("callerIdName", callerIdName);

                QList<QVariant> list = global::getSettingsValue("received", "calls").toList();

                if (list.size() >= 50)
                    list.removeFirst();

                list.append(QVariant::fromValue(received));
                global::setSettingsValue("received", list, "calls");

                Call* call = m_calls.value(uniqueid);
                call->state      = channelStateDesc;

                m_calls.insert(uniqueid, call);

                emit callDeteceted(received, RECIEVED);
            }

            if (channelStateDesc == "Ring")
            {
                Call* call = m_calls.value(uniqueid);
                call->state      = channelStateDesc;
                QString to       = call->exten;

                m_calls.insert(uniqueid, call);

                QMap<QString, QVariant> placed;

                placed.insert("from", exten);
                placed.insert("to", to);
                placed.insert("protocol", protocol);
                placed.insert("date_time", dateTime);

                QList<QVariant> list = global::getSettingsValue("placed", "calls", QVariantList()).toList();

                if (list.size() >= 50)
                    list.removeFirst();

                list.append(QVariant::fromValue(placed));

                global::setSettingsValue("placed", list, "calls");

                emit callDeteceted(placed, PLACED);
            }
        }
    }
    else if (eventData.contains("Event: Newexten"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        QString uniqueid    = eventValues.value("Uniqueid");
        QString extension   = eventValues.value("Extension");

        if (!m_calls.contains(uniqueid))
            return;

        Call* call = m_calls.value(uniqueid);
        QString to = call->exten;
        if (to.isEmpty())
        {
            call->exten = extension;
            QString dateTime = QDateTime::currentDateTime().toString();

            QMap<QString, QVariant> placed;

            placed.insert("from", call->chExten);
            placed.insert("to", call->exten);
            placed.insert("protocol", call->chType);
            placed.insert("date_time", dateTime);

            QList<QVariant> list = global::getSettingsValue("placed", "calls", QVariantList()).toList();

            if (list.size() >= 50)
                list.removeFirst();

            list.append(QVariant::fromValue(placed));

            global::setSettingsValue("placed", list, "calls");

            emit callDeteceted(placed, PLACED);
        }
    }
    else if (eventData.contains("Event: Hangup"))
    {
        QMap<QString, QString> eventValues;
        getEventValues(eventData, eventValues);

        QString uniqueid    = eventValues.value("Uniqueid");
        QString connectedLineName = eventValues.value("ConnectedLineName");

        if (!m_calls.contains(uniqueid))
            return;

        Call* call = m_calls.value(uniqueid);
        QString state = call->state;

        if (state == "Ringing")
        {
            QString dateTime = QDateTime::currentDateTime().toString();

            QMap<QString, QVariant> missed;
            missed.insert("from",           call->exten);
            missed.insert("to",             call->chExten);
            missed.insert("protocol",       call->chType);
            missed.insert("date_time",      dateTime);
            missed.insert("callerIdName",   connectedLineName);

            QList<QVariant> list = global::getSettingsValue("missed", "calls", QVariantList()).toList();

            if (list.size() >= 50)
                list.removeFirst();

            list.append(QVariant::fromValue(missed));

            global::setSettingsValue("missed", list, "calls");

            emit callDeteceted(missed, MISSED);
        }

        delete call;

        m_calls.remove(uniqueid);
    }
}
