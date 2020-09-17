#include "PopupHelloWindow.h"
#include "ui_PopupHelloWindow.h"

#include "PopupWindow.h"
#include "ui_PopupWindow.h"

#include <QDesktopWidget>
#include <QMouseEvent>

QList<PopupHelloWindow*> PopupHelloWindow::m_PopupHelloWindows;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800 // msec
#define TIME_TO_LIVE	4000 // msec

PopupHelloWindow::PopupHelloWindow(const PWInformation& pwi, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopupHelloWindow)
{
    m_pwi = pwi;

    ui->setupUi(this);

    this->setStyleSheet("QMenu                         {margin: 0px; padding: 0px; background-color: #F2F2F2;}"
                        "QMenu::item                   {color: black; background-color: #F2F2F2;}"
                        "QMenu::separator              {background-color: #A9A9A9; black; height: 1px;}"
                        "QMenu::item:selected          {background-color: #18B7FF; color: #000000;}"
                        "QMenu::item:disabled          {background-color: #F2F2F2; color: #A9A9A9;}"
                        "QMenu::item:disabled:selected {background-color: #F2F2F2; color: #A9A9A9;}");

    setAttribute(Qt::WA_TranslucentBackground);

    ui->lblText->setText(pwi.text);

    if (!pwi.avatar.isNull())
    {
        ui->lblAvatar->setScaledContents(true);
        ui->lblAvatar->setPixmap(pwi.avatar);
    }

    ui->lblText->resize(ui->lblText->width(), 60);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    connect(&m_timer, &QTimer::timeout, this, &PopupHelloWindow::onTimer);

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

    bool bTaskbarOnRight = nDesktopWidth < nScreenWidth && rcDesktop.left() == 0;
    bool bTaskbarOnLeft = nDesktopWidth < nScreenWidth && rcDesktop.left() != 0;
    bool bTaskBarOnTop = nDesktopHeight < nScreenHeight && rcDesktop.top() != 0;

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
        m_nStartPosX = rcDesktop.left() - width();
        m_nStartPosY = rcDesktop.bottom() - height();
        m_nTaskbarPlacement = TASKBAR_ON_LEFT;
        nTimerDelay = nTimeToShow / (width() / m_nIncrement);
    }
    else if (bTaskBarOnTop)
    {
        m_nStartPosX = rcDesktop.right() - width();
        m_nStartPosY = rcDesktop.top() - height();
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

    move(m_nCurrentPosX, m_nCurrentPosY);

    m_bAppearing = true;

    m_timer.setInterval(nTimerDelay);
    m_timer.start();
}

PopupHelloWindow::~PopupHelloWindow()
{
    delete ui;
}

void PopupHelloWindow::changeEvent(QEvent* event)
{
    QDialog::changeEvent(event);

    switch (event->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void PopupHelloWindow::onPopupTimeout()
{
    if (isVisible())
        m_timer.start();
}

void PopupHelloWindow::startPopupWaitingTimer()
{
    m_bAppearing = false;

    m_timer.stop();

    int time2live = TIME_TO_LIVE;

    if (this->m_pwi.type == PWPhoneCall)
        time2live = global::getSettingsValue("call_popup_duration", "popup", "20").toInt() * 1000;

    QTimer::singleShot(time2live, this, SLOT(onPopupTimeout()));
}

void PopupHelloWindow::closeAndDestroy()
{
    hide();

    m_timer.stop();

    m_PopupHelloWindows.removeOne(this);

    delete this;
}

void PopupHelloWindow::onTimer()
{
    if (m_bAppearing) // APPEARING
    {
        switch (m_nTaskbarPlacement)
        {
            case TASKBAR_ON_BOTTOM:
                if (m_nCurrentPosY>(m_nStartPosY-height()))
                    m_nCurrentPosY-=m_nIncrement;
                else
                    startPopupWaitingTimer();
                break;
            case TASKBAR_ON_TOP:
                if ((m_nCurrentPosY-m_nStartPosY)<height())
                    m_nCurrentPosY+=m_nIncrement;
                else
                    startPopupWaitingTimer();
                break;
            case TASKBAR_ON_LEFT:
                if ((m_nCurrentPosX-m_nStartPosX)<width())
                    m_nCurrentPosX+=m_nIncrement;
                else
                    startPopupWaitingTimer();
                break;
            case TASKBAR_ON_RIGHT:
                if (m_nCurrentPosX>(m_nStartPosX-width()))
                    m_nCurrentPosX-=m_nIncrement;
                else
                    startPopupWaitingTimer();
                break;
        }
    }

    else // DISSAPPEARING
    {
        switch (m_nTaskbarPlacement)
        {
            case TASKBAR_ON_BOTTOM:
                if (m_nCurrentPosY<m_nStartPosY)
                    m_nCurrentPosY+=m_nIncrement;
                else
                {
                    closeAndDestroy();

                    return;
                }
                break;
            case TASKBAR_ON_TOP:
                if (m_nCurrentPosY>m_nStartPosY)
                    m_nCurrentPosY-=m_nIncrement;
                else
                {
                    closeAndDestroy();

                    return;
                }
                break;
            case TASKBAR_ON_LEFT:
                if (m_nCurrentPosX>m_nStartPosX)
                    m_nCurrentPosX-=m_nIncrement;
                else
                {
                    closeAndDestroy();

                    return;
                }
                break;
            case TASKBAR_ON_RIGHT:
                if (m_nCurrentPosX<m_nStartPosX)
                    m_nCurrentPosX+=m_nIncrement;
                else
                {
                    closeAndDestroy();

                    return;
                }
                break;
        }
    }

    move(m_nCurrentPosX, m_nCurrentPosY);
}

void PopupHelloWindow::showInformationMessage(QString caption, QString message, QPixmap avatar, PWType type)
{
    PWInformation pwi;

    pwi.type = type;

    if (caption != "")
        pwi.text = tr("<b>%1</b><br>%2").arg(caption).arg(message);
    else
        pwi.text = message;

    if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

    pwi.avatar = avatar;

    PopupHelloWindow* popup = new PopupHelloWindow(pwi);

    popup->show();

    m_PopupHelloWindows.append(popup);
}

void PopupHelloWindow::closeAll()
{
    for (int i = 0; i < m_PopupHelloWindows.size(); ++i)
        m_PopupHelloWindows[i]->deleteLater();

    m_PopupHelloWindows.clear();
}

void PopupHelloWindow::mousePressEvent(QMouseEvent*)
{
    m_bAppearing = false;

    m_timer.start();
}
