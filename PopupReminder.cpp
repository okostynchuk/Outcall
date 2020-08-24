#include "PopupReminder.h"
#include "ui_PopupReminder.h"

#include <QDesktopWidget>
#include <QKeyEvent>
#include <QListView>
#include <QSqlQuery>
#include <QDebug>

QList<PopupReminder*> PopupReminder::m_PopupReminders;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800 // msec

PopupReminder::PopupReminder(PopupReminderInfo& pri, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopupReminder)
{
    m_pri = pri;

    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT call_id, phone_from, group_id FROM reminders WHERE id = ?");
    query.addBindValue(m_pri.id);
    query.exec();
    query.next();

    m_pri.group_id = query.value(2).toString();

    if (query.value(0).toString() != NULL)
    {
        m_pri.call_id = query.value(0).toString();

        QSqlDatabase db = QSqlDatabase::database("Calls");
        QSqlQuery query(db);

        query.prepare("SELECT src, extfield1 FROM cdr WHERE uniqueid = ?");
        query.addBindValue(m_pri.call_id);
        query.exec();

        if (query.next())
        {
            m_pri.number = query.value(0).toString();
            m_pri.name = query.value(1).toString();

            if (isInnerPhone(&m_pri.number))
            {
                ui->callButton->setText(m_pri.name);
                ui->openAccessButton->hide();
            }
            else
            {
                QSqlDatabase db;
                QSqlQuery query(db);

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
            QSqlDatabase db;
            QSqlQuery query(db);

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
    else if (query.value(0).toString() == NULL && query.value(1).toString() == m_pri.my_number)
    {
        m_pri.number = m_pri.my_number;

        ui->callButton->hide();
        ui->openAccessButton->hide();
    }
    else
    {
        m_pri.number = query.value(1).toString();

        ui->callButton->hide();
        ui->openAccessButton->hide();
    }

    if (!MSSQLopened)
        ui->openAccessButton->hide();

    QString note = m_pri.text;

    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
    QStringList hrefs;

    while (hrefIterator.hasNext())
    {
        QRegularExpressionMatch match = hrefIterator.next();
        QString href = match.captured(1);

        hrefs << href;
    }

    for (int i = 0; i < hrefs.length(); ++i)
        note.replace(QRegularExpression("(^|\\s)" + QRegularExpression::escape(hrefs.at(i)) + "(\\s|$)"), QString(" <a href='" + hrefs.at(i) + "' style='color: #ffb64f'>" + hrefs.at(i) + "</a> "));

    ui->lblText->setText(note);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    ui->comboBox->addItem(tr("Напомнить позже"));
    ui->comboBox->addItem(tr("Задать время"));
    ui->comboBox->addItem(tr("Через 10 минут"));
    ui->comboBox->addItem(tr("Через 30 минут"));
    ui->comboBox->addItem(tr("Через 1 час"));
    ui->comboBox->addItem(tr("Через 24 часа"));

    QString languages = global::getSettingsValue("language", "settings").toString();
    if (languages == "Русский (по умолчанию)")
        ui->comboBox->setStyleSheet("*{background-color: #ffb64f; border: 1.5px solid #a53501; color: black; padding-left: 16px;} ::drop-down{border: 0px;}");
    else if (languages == "Українська")
        ui->comboBox->setStyleSheet("*{background-color: #ffb64f; border: 1.5px solid #a53501; color: black; padding-left: 21px;} ::drop-down{border: 0px;}");
    else if (languages == "English")
        ui->comboBox->setStyleSheet("*{background-color: #ffb64f; border: 1.5px solid #a53501; color: black; padding-left: 40px;} ::drop-down{border: 0px;}");

    qobject_cast<QListView *>(ui->comboBox->view())->setRowHidden(0, true);

    this->installEventFilter(this);

    if (!ui->callButton->isHidden())
        connect(ui->callButton, &QPushButton::clicked, this, &PopupReminder::onCall);

    connect(&m_timer, &QTimer::timeout, this, &PopupReminder::onTimer);
    connect(ui->okButton, &QPushButton::clicked, this, &PopupReminder::onClosePopup);
    connect(ui->openAccessButton, SIGNAL(clicked(bool)), this, SLOT(onOpenAccess()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectTime()));

    ui->okButton->installEventFilter(this);
    ui->comboBox->installEventFilter(this);

    unsigned int nDesktopHeight;
    unsigned int nDesktopWidth;
    unsigned int nScreenWidth;
    unsigned int nScreenHeight;

    QDesktopWidget desktop;
    QRect rcScreen = desktop.screenGeometry(this);
    QRect rcDesktop = desktop.availableGeometry(this);

    nDesktopWidth = rcDesktop.width();
    nDesktopHeight = rcDesktop.height();
    nScreenWidth = rcScreen.width();
    nScreenHeight = rcScreen.height();

    bool bTaskbarOnRight = nDesktopWidth <= nScreenWidth && rcDesktop.left() == 0;
    bool bTaskbarOnLeft = nDesktopWidth <= nScreenWidth && rcDesktop.left() != 0;
    bool bTaskBarOnTop = nDesktopHeight <= nScreenHeight && rcDesktop.top() != 0;

    int nTimeToShow = TIME_TO_SHOW;
    int nTimerDelay;

    m_nIncrement = 2;

    if (bTaskbarOnRight)
    {
        m_nStartPosX = (rcDesktop.right());
        m_nStartPosY = rcDesktop.bottom() - height();
        m_nTaskbarPlacement = TASKBAR_ON_RIGHT;
        nTimerDelay = nTimeToShow / (width() / m_nIncrement);
    }
    else if (bTaskbarOnLeft)
    {
        m_nStartPosX = (rcDesktop.left() - width());
        m_nStartPosY = rcDesktop.bottom() - height();
        m_nTaskbarPlacement = TASKBAR_ON_LEFT;
        nTimerDelay = nTimeToShow / (width() / m_nIncrement);
    }
    else if (bTaskBarOnTop)
    {
        m_nStartPosX = rcDesktop.right() - width();
        m_nStartPosY = (rcDesktop.top() - height());
        m_nTaskbarPlacement = TASKBAR_ON_TOP;
        nTimerDelay = nTimeToShow / (height() / m_nIncrement);
    }
    else
    {
        m_nStartPosX = rcDesktop.right() - width();
        m_nStartPosY = rcDesktop.bottom();
        m_nTaskbarPlacement = TASKBAR_ON_BOTTOM;
        nTimerDelay = nTimeToShow / (height() / m_nIncrement);
    }

    m_nCurrentPosX = m_nStartPosX;
    m_nCurrentPosY = m_nStartPosY;

    position = QPoint();

    move(m_nCurrentPosX, m_nCurrentPosY);

    m_bAppearing = true;

    m_timer.setInterval(nTimerDelay);
    m_timer.start();
}

PopupReminder::~PopupReminder()
{
    delete ui;
}

void PopupReminder::closeAndDestroy()
{
    hide();

    m_timer.stop();

    m_PopupReminders.removeOne(this);

    delete this;
}

void PopupReminder::mousePressEvent(QMouseEvent* event)
{
    position = event->globalPos();
}

void PopupReminder::mouseReleaseEvent(QMouseEvent *event)
{
    (void) event;
    position = QPoint();
}

void PopupReminder::mouseMoveEvent(QMouseEvent* event)
{
    if (!position.isNull())
    {
        QPoint delta = event->globalPos() - position;

        if (position.x() > this->x() + this->width() - 10
                || position.y() > this->y() + this->height() - 10)
        {}
        else
        {
            move(this->x() + delta.x(), this->y() + delta.y());
            position = event->globalPos();
        }
    }
}

bool PopupReminder::isInnerPhone(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[2][0-9]{2}"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void PopupReminder::onTimer()
{
    if (m_bAppearing) // APPEARING
    {
        switch (m_nTaskbarPlacement)
        {
            case TASKBAR_ON_BOTTOM:
                if (m_nCurrentPosY>(m_nStartPosY-height()))
                    m_nCurrentPosY-=m_nIncrement;
                else
                {
                    m_bAppearing = false;

                    m_timer.stop();
                }
                break;
            case TASKBAR_ON_TOP:
                if ((m_nCurrentPosY-m_nStartPosY)<height())
                    m_nCurrentPosY+=m_nIncrement;
                else
                {
                    m_bAppearing = false;

                    m_timer.stop();
                }
                break;
            case TASKBAR_ON_LEFT:
                if ((m_nCurrentPosX-m_nStartPosX)<width())
                    m_nCurrentPosX+=m_nIncrement;
                else
                {
                    m_bAppearing = false;

                    m_timer.stop();
                }
                break;
            case TASKBAR_ON_RIGHT:
                if (m_nCurrentPosX>(m_nStartPosX-width()))
                    m_nCurrentPosX-=m_nIncrement;
                else
                {
                    m_bAppearing = false;

                    m_timer.stop();
                }
                break;
        }
    }
    else // DISSAPPEARING
    {
        switch (m_nTaskbarPlacement)
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

    move(m_nCurrentPosX, m_nCurrentPosY);
}

void PopupReminder::receiveData(bool updating)
{
    if (updating)
    {
        m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->resizeCells = false;
        m_pri.remindersDialog->onUpdate();

        editReminderDialog->close();

        closeAndDestroy();
    }
    else
        editReminderDialog = nullptr;
}

void PopupReminder::receiveNumber(QString &number)
{
    QString my_number = m_pri.my_number.remove(QRegularExpression(" .+"));
    QString protocol = global::getSettingsValue(my_number, "extensions").toString();

    g_pAsteriskManager->originateCall(my_number, number, protocol, my_number);
}

void PopupReminder::onCall()
{
    QString my_number = m_pri.my_number.remove(QRegularExpression(" .+"));

    if (!m_pri.numbers.isEmpty())
    {
        if (m_pri.numbers.length() > 1)
        {
            chooseNumber = new ChooseNumber;
            chooseNumber->setValuesNumber(m_pri.call_id);
            connect(chooseNumber, SIGNAL(sendNumber(QString &)), this, SLOT(receiveNumber(QString &)));
            chooseNumber->show();
            chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
        }
        else
        {
            const QString protocol = global::getSettingsValue(my_number, "extensions").toString();

            g_pAsteriskManager->originateCall(my_number, m_pri.numbers.at(0), protocol, my_number);
        }
    }
    else
    {
        const QString protocol = global::getSettingsValue(my_number, "extensions").toString();

        g_pAsteriskManager->originateCall(my_number, m_pri.number, protocol, my_number);
    }
}

void PopupReminder::onSelectTime()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    switch (ui->comboBox->currentIndex())
    {
    case 1:
        if (editReminderDialog != nullptr)
            editReminderDialog->close();

        editReminderDialog = new EditReminderDialog;
        editReminderDialog->setValuesReminders(m_pri.id, m_pri.group_id, m_pri.dateTime, m_pri.note);
        connect(editReminderDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        editReminderDialog->show();
        editReminderDialog->setAttribute(Qt::WA_DeleteOnClose);

        ui->comboBox->setCurrentIndex(0);
        break;
    case 2:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(600)));
        query.addBindValue(m_pri.id);
        query.exec();

        m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->resizeCells = false;
        m_pri.remindersDialog->onUpdate();

        if (editReminderDialog != nullptr)
            editReminderDialog->close();

        closeAndDestroy();
        break;
    case 3:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(1800)));
        query.addBindValue(m_pri.id);
        query.exec();

        m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->resizeCells = false;
        m_pri.remindersDialog->onUpdate();

        if (editReminderDialog != nullptr)
            editReminderDialog->close();

        closeAndDestroy();
        break;
    case 4:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(3600)));
        query.addBindValue(m_pri.id);
        query.exec();

        m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->resizeCells = false;
        m_pri.remindersDialog->onUpdate();

        if (editReminderDialog != nullptr)
            editReminderDialog->close();

        closeAndDestroy();
        break;
    case 5:
        query.prepare("UPDATE reminders SET datetime = ?, active = true, viewed = true, completed = false WHERE id = ?");
        query.addBindValue(QDateTime(QDate::currentDate().addDays(1), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0)));
        query.addBindValue(m_pri.id);
        query.exec();

        m_pri.remindersDialog->reminders(false);
        m_pri.remindersDialog->resizeCells = false;
        m_pri.remindersDialog->onUpdate();

        if (editReminderDialog != nullptr)
            editReminderDialog->close();

        closeAndDestroy();
        break;
    default:
        break;
    }
}

void PopupReminder::onOpenAccess()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pri.number + "')");
    query.exec();
    query.first();

    QString vyborID = query.value(0).toString();
    QString userID = global::getSettingsValue("user_login", "settings").toString();

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
        QSqlQuery query1(dbOrders);

        query1.prepare("INSERT INTO CallTable (UserID, ClientID)"
                   "VALUES (user_id(?), ?)");
        query1.addBindValue(userID);
        query1.addBindValue(vyborID);
        query1.exec();

        ui->openAccessButton->setDisabled(true);

        dbOrders.close();
    }
    else
    {
        setStyleSheet("QMessageBox{ color: #000000; }");

        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе заказов!"), QMessageBox::Ok);
    }
}

void PopupReminder::onClosePopup()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    if (m_pri.my_number == m_pri.number)
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

    m_pri.remindersDialog->reminders(false);
    m_pri.remindersDialog->resizeCells = false;
    m_pri.remindersDialog->onUpdate();

    if (editReminderDialog != nullptr)
        editReminderDialog->close();

    if (isVisible())
        m_timer.start();
}

void PopupReminder::closeAll()
{
    for (int i = 0; i < m_PopupReminders.size(); ++i)
        m_PopupReminders[i]->deleteLater();

    m_PopupReminders.clear();
}

void PopupReminder::showReminder(RemindersDialog* receivedRemindersDialog, QString receivedNumber, QString receivedId, QDateTime receivedDateTime, QString receivedNote)
{
    PopupReminderInfo pri;

    pri.remindersDialog = receivedRemindersDialog;
    pri.my_number = receivedNumber;
    pri.id = receivedId;
    pri.dateTime = receivedDateTime;
    pri.note = receivedNote;
    pri.active = true;

    pri.text = "<b> " + pri.note + " </b>";

    PopupReminder *reminder = new PopupReminder(pri);

    reminder->show();

    reminder->ui->labelTime->setText(tr("<font size = 1>%1</font>").arg(pri.dateTime.toString("dd.MM.yy hh:mm")));
    reminder->ui->labelTime->setStyleSheet("*{color: white; font-weight:bold}");

    m_PopupReminders.append(reminder);
}

void PopupReminder::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        onClosePopup();
    else
        QWidget::keyPressEvent(event);
}
