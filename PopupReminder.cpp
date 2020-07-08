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

#define TIME_TO_SHOW	800 //msec

PopupReminder::PopupReminder(PopupReminderInfo& pri, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopupReminder)
{
    m_pri = pri;

    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    ui->lblText->setText(m_pri.text);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    ui->comboBox->addItem(tr("Напомнить позже"));
    ui->comboBox->addItem(tr("Задать время"));
    ui->comboBox->addItem(tr("Через 10 минут"));
    ui->comboBox->addItem(tr("Через 30 минут"));
    ui->comboBox->addItem(tr("Через 1 час"));
    ui->comboBox->addItem(tr("Через 24 часа"));

    qobject_cast<QListView *>(ui->comboBox->view())->setRowHidden(0, true);
    this->installEventFilter(this);

    connect(&m_timer, &QTimer::timeout, this, &PopupReminder::onTimer);
    connect(ui->okButton, &QAbstractButton::clicked, this, &PopupReminder::onClosePopup);
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

void PopupReminder::onTimer()
{
    if (m_bAppearing) // APPEARING
    {
        switch(m_nTaskbarPlacement)
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
        switch(m_nTaskbarPlacement)
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
        m_pri.remindersDialog->onUpdate();
        m_pri.remindersDialog->sendNewValues();

        onClosePopup();
    }
}

void PopupReminder::onSelectTime()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    switch (ui->comboBox->currentIndex())
    {
    case 1:
        editReminderDialog = new EditReminderDialog;
        editReminderDialog->setValuesReminders(m_pri.my_number, m_pri.selectedNumber, m_pri.id, m_pri.dateTime, m_pri.note);
        connect(editReminderDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        editReminderDialog->show();
        editReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
        hide();
        break;
    case 2:
        query.prepare("UPDATE reminders SET datetime = ?, active = true WHERE id = ? AND phone_to = ?");
        query.addBindValue(QDateTime::QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(600)));
        query.addBindValue(m_pri.id);
        query.addBindValue(m_pri.my_number);
        query.exec();

        m_pri.remindersDialog->onUpdate();
        m_pri.remindersDialog->sendNewValues();

        onClosePopup();
        break;
    case 3:
        query.prepare("UPDATE reminders SET datetime = ?, active = true WHERE id = ? AND phone_to = ?");
        query.addBindValue(QDateTime::QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(1800)));
        query.addBindValue(m_pri.id);
        query.addBindValue(m_pri.my_number);
        query.exec();

        m_pri.remindersDialog->onUpdate();
        m_pri.remindersDialog->sendNewValues();

        onClosePopup();
        break;
    case 4:
        query.prepare("UPDATE reminders SET datetime = ?, active = true WHERE id = ? AND phone_to = ?");
        query.addBindValue(QDateTime::QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0).addSecs(3600)));
        query.addBindValue(m_pri.id);
        query.addBindValue(m_pri.my_number);
        query.exec();

        m_pri.remindersDialog->onUpdate();
        m_pri.remindersDialog->sendNewValues();

        onClosePopup();
        break;
    case 5:
        query.prepare("UPDATE reminders SET datetime = ?, active = true WHERE id = ? AND phone_to = ?");
        query.addBindValue(QDateTime::QDateTime(QDate::currentDate().addDays(1), QTime(QTime::currentTime().hour(), QTime::currentTime().minute(), 0)));
        query.addBindValue(m_pri.id);
        query.addBindValue(m_pri.my_number);
        query.exec();

        m_pri.remindersDialog->onUpdate();
        m_pri.remindersDialog->sendNewValues();

        onClosePopup();
        break;
    default:
        break;
    }
}

void PopupReminder::onClosePopup()
{
    if (isVisible())
        m_timer.start();
}

void PopupReminder::closeAll()
{
    qDeleteAll(m_PopupReminders);
    m_PopupReminders.clear();
}

void PopupReminder::showReminder(RemindersDialog* receivedRemindersDialog, QString receivedNumber, QString receivedSelectedNumber, QString receivedId, QDateTime receivedDateTime, QString receivedNote)
{
    PopupReminderInfo pri;

    pri.remindersDialog = receivedRemindersDialog;
    pri.my_number = receivedNumber;
    pri.selectedNumber = receivedSelectedNumber;
    pri.id = receivedId;
    pri.dateTime = receivedDateTime;
    pri.note = receivedNote;
    pri.active = true;

    pri.text = tr("%1<br><br><b>%2</b>").arg(pri.dateTime.toString("dd.MM.yy hh:mm")).arg(pri.note);

    PopupReminder *reminder = new PopupReminder(pri);
    reminder->show();
    m_PopupReminders.append(reminder);
}

void PopupReminder::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
         onClosePopup();
    else
        QWidget::keyPressEvent(event);
}
