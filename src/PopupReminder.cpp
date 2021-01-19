/*
 * Класс служит для отображения окна с напоминанием.
 */

#include "PopupReminder.h"
#include "ui_PopupReminder.h"

#include <QDesktopWidget>
#include <QKeyEvent>
#include <QListView>
#include <QSqlQuery>
#include <QDebug>
#include <QLineEdit>

QList<PopupReminder*> PopupReminder::s_popupReminders;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW    800 // msec

PopupReminder::PopupReminder(const PopupReminderInfo& pri, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PopupReminder)
{
    m_pri = pri;

    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    QSqlQuery query(m_db);

    query.prepare("SELECT call_id, phone_from, group_id FROM reminders WHERE id = ?");
    query.addBindValue(m_pri.id);
    query.exec();
    query.next();

    m_pri.group_id = query.value(2).toString();

    if (query.value(0).toString() != NULL)
    {
        m_pri.call_id = query.value(0).toString();

        QSqlQuery query(m_dbCalls);

        query.prepare("SELECT src, extfield1 FROM cdr WHERE uniqueid = ?");
        query.addBindValue(m_pri.call_id);
        query.exec();

        if (query.next())
        {
            m_pri.number = query.value(0).toString();
            m_pri.name = query.value(1).toString();

            if (isInternalPhone(&m_pri.number))
            {
                ui->callButton->setText(m_pri.name);
                ui->openAccessButton->hide();
            }
            else
            {
                QSqlQuery query(m_db);

                query.prepare("SELECT entry_name, entry_vybor_id FROM entry_phone WHERE entry_phone = ?");
                query.addBindValue(m_pri.number);
                query.exec();

                if (query.next())
                {
                    ui->callButton->setText(query.value(0).toString());

                    if (query.value(1) == 0)
                        ui->openAccessButton->hide();
                }
                else
                {
                    ui->callButton->setText(m_pri.number);
                    ui->openAccessButton->hide();
                }
            }
        }
        else
        {
            QSqlQuery query(m_db);

            query.prepare("SELECT entry_name, entry_phone, entry_vybor_id FROM entry_phone WHERE entry_id = ?");
            query.addBindValue(m_pri.call_id);
            query.exec();

            if (query.next())
            {
                m_pri.name = query.value(0).toString();
                m_pri.numbers.append(query.value(1).toString());

                if (query.value(2) == 0)
                    ui->openAccessButton->hide();
            }

            while (query.next())
                m_pri.numbers.append(query.value(1).toString());

            ui->callButton->setText(m_pri.name);
        }
    }
    else if (query.value(0).toString() == NULL && query.value(1).toString() == g_personalNumberName)
    {
        m_pri.number = g_personalNumberName;

        ui->callButton->hide();
        ui->openAccessButton->hide();
    }
    else
    {
        m_pri.number = query.value(1).toString();

        ui->callButton->hide();
        ui->openAccessButton->hide();
    }

    if (!g_ordersDbOpened)
        ui->openAccessButton->hide();

    QString note = m_pri.text;


    QRegularExpressionMatchIterator hrefIterator = m_hrefRegExp.globalMatch(note);
    QStringList hrefs, hrefsNoCharacters, hrefsReplaceCharacters;

    note.replace("<", "&lt;").replace(">", "&gt;");

    while (hrefIterator.hasNext())
    {
        QRegularExpressionMatch match = hrefIterator.next();
        QString href = match.captured(1);

        hrefs << href;
        href.remove(QRegularExpression("[\\,\\.\\;\\:\\'\\\"\\-\\!\\?\\^\\`\\~\\*\\№\\%\\&\\$\\#\\<\\>\\(\\)\\[\\]\\{\\}]+$"));
        hrefsNoCharacters << href;
    }

    QStringList firstCharList, lastCharList;

    for (qint32 i = 0; i < hrefs.length(); ++i)
    {
        QString hrefReplaceCharacters = QString(hrefs.at(i)).replace("<", "&lt;").replace(">", "&gt;");
        hrefsReplaceCharacters << hrefReplaceCharacters;
        hrefReplaceCharacters = hrefReplaceCharacters.remove(hrefsNoCharacters.at(i));

        if (hrefReplaceCharacters.isEmpty())
            lastCharList << " ";
        else
            lastCharList << hrefReplaceCharacters;
    }

    note.replace(QRegularExpression("\\n"), QString(" <br> "));

    qint32 index = 0;

    for (qint32 i = 0; i < hrefsReplaceCharacters.length(); ++i)
    {
        if (i == 0)
            index = note.indexOf(hrefsReplaceCharacters.at(i));
        else
            index = note.indexOf(hrefsReplaceCharacters.at(i), index + hrefsReplaceCharacters.at(i - 1).size());

        if (index > 0)
            firstCharList << note.at(index - 1);
        else
            firstCharList << "";
    }

    for (qint32 i = 0; i < hrefs.length(); ++i)
    {
        qint32 size;

        if (firstCharList.at(i) == "")
            size = hrefsReplaceCharacters.at(i).size();
        else
            size = hrefsReplaceCharacters.at(i).size() + 1;

        note.replace(note.indexOf(QRegularExpression("( |^|\\^|\\.|\\,|\\(|\\)|\\[|\\]|\\{|\\}|\\;|\\'|\\\"|[a-zA-Z0-9а-яА-Я]|\\`|\\~|\\%|\\$|\\#|\\№|\\@|\\&|\\/|\\\\|\\!|\\*)" + QRegularExpression::escape(hrefsReplaceCharacters.at(i)) + "( |$)")),
                     size, QString(firstCharList.at(i) + "<a href='" + hrefsNoCharacters.at(i) + "' style='color: #ffb64f'>" + hrefsNoCharacters.at(i) + "</a>" + lastCharList.at(i)));
    }

    ui->textBrowser->setText(note);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    ui->comboBox->addItem(tr("Напомнить позже"));
    ui->comboBox->addItem(tr("Задать время"));
    ui->comboBox->addItem(tr("Через 10 минут"));
    ui->comboBox->addItem(tr("Через 30 минут"));
    ui->comboBox->addItem(tr("Через 1 час"));
    ui->comboBox->addItem(tr("Через 24 часа"));

    if (g_language == "Русский")
        ui->comboBox->setStyleSheet("*{background-color: #ffb64f; border: 1.5px solid #a53501; color: black; padding-left: 16px;} ::drop-down{border: 0px;}");
    else if (g_language == "Українська")
        ui->comboBox->setStyleSheet("*{background-color: #ffb64f; border: 1.5px solid #a53501; color: black; padding-left: 21px;} ::drop-down{border: 0px;}");
    else if (g_language == "English")
        ui->comboBox->setStyleSheet("*{background-color: #ffb64f; border: 1.5px solid #a53501; color: black; padding-left: 40px;} ::drop-down{border: 0px;}");

    qobject_cast<QListView*>(ui->comboBox->view())->setRowHidden(0, true);

    if (!ui->callButton->isHidden())
        connect(ui->callButton, &QAbstractButton::clicked, this, &PopupReminder::onCall);

    connect(&m_timer, &QTimer::timeout, this, &PopupReminder::onTimer);
    connect(ui->okButton, &QAbstractButton::clicked, this, &PopupReminder::onClosePopup);
    connect(ui->openAccessButton, &QAbstractButton::clicked, this, &PopupReminder::onOpenAccess);
    connect(ui->comboBox, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, &PopupReminder::onSelectTime);

    quint32 desktopHeight;
    quint32 desktopWidth;
    quint32 screenWidth;
    quint32 screenHeight;

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);
    QRect desktop = desktopWidget.availableGeometry(this);

    desktopWidth = desktop.width();
    desktopHeight = desktop.height();
    screenWidth = screen.width();
    screenHeight = screen.height();

    bool isTaskbarOnRight = desktopWidth <= screenWidth && desktop.left() == 0;
    bool isTaskbarOnLeft = desktopWidth <= screenWidth && desktop.left() != 0;
    bool isTaskBarOnTop = desktopHeight <= screenHeight && desktop.top() != 0;

    qint32 timeToShow = TIME_TO_SHOW;
    qint32 timerDelay;

    m_increment = 2;

    if (isTaskbarOnRight)
    {
        m_startPosX = (desktop.right());
        m_startPosY = desktop.bottom() - height();
        m_taskbarPlacement = TASKBAR_ON_RIGHT;
        timerDelay = timeToShow / (width() / m_increment);
    }
    else if (isTaskbarOnLeft)
    {
        m_startPosX = (desktop.left() - width());
        m_startPosY = desktop.bottom() - height();
        m_taskbarPlacement = TASKBAR_ON_LEFT;
        timerDelay = timeToShow / (width() / m_increment);
    }
    else if (isTaskBarOnTop)
    {
        m_startPosX = desktop.right() - width();
        m_startPosY = (desktop.top() - height());
        m_taskbarPlacement = TASKBAR_ON_TOP;
        timerDelay = timeToShow / (height() / m_increment);
    }
    else
    {
        m_startPosX = desktop.right() - width();
        m_startPosY = desktop.bottom();
        m_taskbarPlacement = TASKBAR_ON_BOTTOM;
        timerDelay = timeToShow / (height() / m_increment);
    }

    m_currentPosX = m_startPosX;
    m_currentPosY = m_startPosY;

    m_position = QPoint();

    move(m_currentPosX, m_currentPosY);

    m_appearing = true;

    m_timer.setInterval(timerDelay);
    m_timer.start();
}

PopupReminder::~PopupReminder()
{
    delete ui;
}

/**
 * Выполняет закрытие и удаление объекта окна.
 */
void PopupReminder::closeAndDestroy()
{
    hide();

    m_timer.stop();

    s_popupReminders.removeOne(this);

    this->deleteLater();
}

/**
 * Выполняет сохранение позиции нажатия мышью по окну.
 */
void PopupReminder::mousePressEvent(QMouseEvent* event)
{
    m_position = event->globalPos();
}

/**
 * Выполняет установку нулевой позиции при отжатии кнопки мыши.
 */
void PopupReminder::mouseReleaseEvent(QMouseEvent* event)
{
    (void) event;
    m_position = QPoint();
}

/**
 * Выполняет изменение позиции окна на экране.
 */
void PopupReminder::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_position.isNull())
    {
        QPoint delta = event->globalPos() - m_position;

        if (m_position.x() > this->x() + this->width() - 10
                || m_position.y() > this->y() + this->height() - 10)
        {}
        else
        {
            move(this->x() + delta.x(), this->y() + delta.y());
            m_position = event->globalPos();
        }
    }
}

/**
 * Выполняет проверку номера на соотвествие шаблону внутреннего номера.
 */
bool PopupReminder::isInternalPhone(QString* str)
{
    qint32 pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("^[0-9]{4}$"));
    QRegularExpressionValidator validator2(QRegularExpression("^[2][0-9]{2}$"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Выполняет операции динамического появления и последующего закрытия окна.
 */
void PopupReminder::onTimer()
{
    if (m_appearing)
    {
        switch (m_taskbarPlacement)
        {
        case TASKBAR_ON_BOTTOM:
            if (m_currentPosY > (m_startPosY - height()))
                m_currentPosY -= m_increment;
            else
            {
                m_appearing = false;
                m_timer.stop();
            }
            break;
        case TASKBAR_ON_TOP:
            if ((m_currentPosY - m_startPosY) < height())
                m_currentPosY += m_increment;
            else
            {
                m_appearing = false;
                m_timer.stop();
            }
            break;
        case TASKBAR_ON_LEFT:
            if ((m_currentPosX - m_startPosX) < width())
                m_currentPosX += m_increment;
            else
            {
                m_appearing = false;
                m_timer.stop();
            }
            break;
        case TASKBAR_ON_RIGHT:
            if (m_currentPosX > (m_startPosX - width()))
                m_currentPosX -= m_increment;
            else
            {
                m_appearing = false;
                m_timer.stop();
            }
            break;
        }
    }
    else
    {
        switch (m_taskbarPlacement)
        {
        case TASKBAR_ON_BOTTOM:
            closeAndDestroy();
            return;
            break;
        case TASKBAR_ON_TOP:
            closeAndDestroy();
            return;
            break;
        case TASKBAR_ON_LEFT:
            closeAndDestroy();
            return;
            break;
        case TASKBAR_ON_RIGHT:
            closeAndDestroy();
            return;
            break;
        }
    }

    move(m_currentPosX, m_currentPosY);
}

/**
 * Получает запрос из класса EditReminderDialog на обновление списка напоминаний,
 * изменение иконки приложения, а также закрытие и удаление объекта окна.
 */
void PopupReminder::receiveData(bool update)
{
    if (update)
    {
        emit m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->m_resizeCells = false;
        m_pri.remindersDialog->loadReminders();

        closeAndDestroy();
    }
}

/**
 * Выполняет операции для последующего выбора номера контакта и совершения звонка.
 */
void PopupReminder::onCall()
{
    if (!m_pri.numbers.isEmpty())
    {
        if (m_pri.numbers.length() > 1)
        {
            if (!m_chooseNumber.isNull())
                m_chooseNumber->close();

            m_chooseNumber = new ChooseNumber;
            m_chooseNumber->setValues(m_pri.numbers);
            m_chooseNumber->show();
            m_chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
        }
        else
        {
            QString protocol = global::getSettingsValue(g_personalNumber, "extensions").toString();

            g_asteriskManager->originateCall(g_personalNumber, m_pri.numbers.at(0), protocol, g_personalNumber);
        }
    }
    else
    {
        QString protocol = global::getSettingsValue(g_personalNumber, "extensions").toString();

        g_asteriskManager->originateCall(g_personalNumber, m_pri.number, protocol, g_personalNumber);
    }
}

/**
 * Выполняет перенос напоминания на заданное время.
 */
void PopupReminder::onSelectTime(qint32 index)
{
    QSqlQuery query(m_db);

    switch (index)
    {
    case 1:
        if (!m_editReminderDialog.isNull())
            m_editReminderDialog->close();

        m_editReminderDialog = new EditReminderDialog;
        m_editReminderDialog->setValues(m_pri.id, m_pri.group_id, m_pri.dateTime, m_pri.note);
        connect(m_editReminderDialog, &EditReminderDialog::sendData, this, &PopupReminder::receiveData);
        m_editReminderDialog->show();
        m_editReminderDialog->setAttribute(Qt::WA_DeleteOnClose);

        ui->comboBox->setCurrentIndex(0);
        break;
    case 2:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(600)));
        query.addBindValue(m_pri.id);
        query.exec();

        emit m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->m_resizeCells = false;
        m_pri.remindersDialog->loadReminders();

        if (!m_editReminderDialog.isNull())
            m_editReminderDialog->close();

        closeAndDestroy();
        break;
    case 3:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(1800)));
        query.addBindValue(m_pri.id);
        query.exec();

        emit m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->m_resizeCells = false;
        m_pri.remindersDialog->loadReminders();

        if (!m_editReminderDialog.isNull())
            m_editReminderDialog->close();

        closeAndDestroy();
        break;
    case 4:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(3600)));
        query.addBindValue(m_pri.id);
        query.exec();

        emit m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->m_resizeCells = false;
        m_pri.remindersDialog->loadReminders();

        if (!m_editReminderDialog.isNull())
            m_editReminderDialog->close();

        closeAndDestroy();
        break;
    case 5:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate().addDays(1), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0)));
        query.addBindValue(m_pri.id);
        query.exec();

        emit m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->m_resizeCells = false;
        m_pri.remindersDialog->loadReminders();

        if (!m_editReminderDialog.isNull())
            m_editReminderDialog->close();

        closeAndDestroy();
        break;
    default:
        break;
    }
}

/**
 * Выполняет открытие базы заказов.
 */
void PopupReminder::onOpenAccess()
{
    QSqlQuery query(m_db);

    query.prepare("SELECT entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pri.number + "')");
    query.exec();
    query.first();

    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray password3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(password3));
    QString port_3 = global::getSettingsValue("port_3", "settings").toString();

    QSqlDatabase dbOrders = QSqlDatabase::addDatabase("QODBC", "Orders");

    dbOrders.setDatabaseName("DRIVER={SQL Server Native Client 10.0};"
                            "Server="+hostName_3+","+port_3+";"
                            "Database="+databaseName_3+";"
                            "Uid="+userName_3+";"
                            "Pwd="+password_3);
    dbOrders.open();

    if (dbOrders.isOpen())
    {
        QString userId = global::getSettingsValue("user_login", "settings").toString();
        QString vyborId = query.value(0).toString();

        QSqlQuery query1(dbOrders);

        query1.prepare("INSERT INTO CallTable (UserID, ClientID)"
                   "VALUES (user_id(?), ?)");
        query1.addBindValue(userId);
        query1.addBindValue(vyborId);
        query1.exec();

        ui->openAccessButton->setDisabled(true);

        dbOrders.close();
    }
    else
    {
        setStyleSheet("QMessageBox { color: #000000; }");

        MsgBoxError(tr("Отсутствует подключение к базе заказов!"));
    }
}

/**
 * Выполняет установку напоминания в неактивное состояние и
 * запускает таймер для последующего закрытия окна.
 */
void PopupReminder::onClosePopup()
{
    QSqlQuery query(m_db);

    if (g_personalNumberName == m_pri.number)
    {
        query.prepare("UPDATE reminders SET active = false WHERE id = ?");
        query.addBindValue(m_pri.id);
        query.exec();
    }
    else
    {
        query.prepare("UPDATE reminders SET active = false, viewed = true, completed = true WHERE id = ?");
        query.addBindValue(m_pri.id);
        query.exec();
    }

    emit m_pri.remindersDialog->reminders(false);

    m_pri.remindersDialog->m_resizeCells = false;
    m_pri.remindersDialog->loadReminders();

    if (!m_editReminderDialog.isNull())
        m_editReminderDialog->close();

    if (isVisible())
        m_timer.start();
}

/**
 * Выполняет закрытие и удаление всех объектов окон.
 */
void PopupReminder::closeAll()
{
    for (qint32 i = 0; i < s_popupReminders.size(); ++i)
        s_popupReminders[i]->deleteLater();

    s_popupReminders.clear();
}

/**
 * Выполняет создание окна и отображение в нём полученной информации из класса RemindersDialog.
 */
void PopupReminder::showReminder(RemindersDialog* remindersDialog, const QString& id, const QDateTime& dateTime, const QString& note)
{
    PopupReminderInfo pri;

    pri.remindersDialog = remindersDialog;
    pri.id = id;
    pri.dateTime = dateTime;
    pri.note = note;
    pri.active = true;

    pri.text = pri.note;

    PopupReminder* reminder = new PopupReminder(pri);

    reminder->show();

    reminder->ui->labelTime->setText(tr("<font size = 1>%1</font>").arg(pri.dateTime.toString("dd.MM.yy hh:mm")));
    reminder->ui->labelTime->setStyleSheet("*{color: white; font-weight:bold}");

    s_popupReminders.append(reminder);
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void PopupReminder::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        onClosePopup();
    else
        QWidget::keyPressEvent(event);
}
