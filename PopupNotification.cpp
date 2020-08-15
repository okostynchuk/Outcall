#include "PopupNotification.h"
#include "ui_PopupNotification.h"

#include "AsteriskManager.h"

#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSqlQuery>

QList<PopupNotification*> PopupNotification::m_PopupNotifications;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800 // msec

PopupNotification::PopupNotification(PopupNotificationInfo& pni, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopupNotification)
{
    m_pni = pni;

    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    ui->label->setText(tr("Новое напоминание от ") + m_pni.number);

    QString note = m_pni.text;

    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
    QStringList hrefs;

    while (hrefIterator.hasNext())
    {
        QRegularExpressionMatch match = hrefIterator.next();
        QString href = match.captured(1);

        if (!hrefs.contains(href))
            hrefs << href;
    }

    for (int i = 0; i < hrefs.length(); ++i)
        note.replace(QRegularExpression("(^|\\s)" + QRegularExpression::escape(hrefs.at(i)) + "(\\s|$)"), QString(" <a href='" + hrefs.at(i) + "' style='color: #ffb64f'>" + hrefs.at(i) + "</a> "));

    ui->lblText->setText(note);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    connect(&m_timer, &QTimer::timeout, this, &PopupNotification::onTimer);

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
        m_nStartPosX = rcDesktop.right();
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

PopupNotification::~PopupNotification()
{
    delete ui;
}

void PopupNotification::mousePressEvent(QMouseEvent* event)
{
    position = event->globalPos();
}

void PopupNotification::mouseReleaseEvent(QMouseEvent *event)
{
    (void) event;

    position = QPoint();
}

void PopupNotification::mouseMoveEvent(QMouseEvent* event)
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

void PopupNotification::on_pushButton_close_clicked()
{
    onClosePopup();
}

void PopupNotification::onTimer()
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

void PopupNotification::onClosePopup()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET viewed = true WHERE id = ?");
    query.addBindValue(m_pni.id);
    query.exec();

    if (isVisible())
        m_timer.start();
}

void PopupNotification::closeAndDestroy()
{
    hide();

    m_timer.stop();

    if (m_PopupNotifications.length() == 1)
        m_pni.remindersDialog->reminders(false);

    m_PopupNotifications.removeOne(this);

    delete this;
}

void PopupNotification::closeAll()
{
    for (int i = 0; i < m_PopupNotifications.size(); ++i)
        m_PopupNotifications[i]->deleteLater();

    m_PopupNotifications.clear();
}

void PopupNotification::showNotification(RemindersDialog* receivedRemindersDialog, QString receivedId, QString receivedNumber, QString receivedNote)
{
    PopupNotificationInfo pni;

    pni.remindersDialog = receivedRemindersDialog;
    pni.id = receivedId;
    pni.number = receivedNumber;
    pni.note = receivedNote;

    pni.text = "<b>" + pni.note + "</b>";

    PopupNotification* notification = new PopupNotification(pni);

    notification->show();

    m_PopupNotifications.append(notification);
}

void PopupNotification::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        onClosePopup();
    else
        QWidget::keyPressEvent(event);
}
