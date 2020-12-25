/*
 * Класс служит для отображения окна приветствия приложения.
 */

#include "PopupHelloWindow.h"
#include "ui_PopupHelloWindow.h"

#include "PopupWindow.h"
#include "ui_PopupWindow.h"

#include <QDesktopWidget>
#include <QMouseEvent>

QList<PopupHelloWindow*> PopupHelloWindow::s_popupHelloWindows;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW    800 // msec
#define TIME_TO_LIVE    4000 // msec

PopupHelloWindow::PopupHelloWindow(const PopupHelloWindowInfo& phwi, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PopupHelloWindow)
{
    m_phwi = phwi;

    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    ui->lblText->setText(phwi.text);

    if (!phwi.avatar.isNull())
    {
        ui->lblAvatar->setScaledContents(true);
        ui->lblAvatar->setPixmap(phwi.avatar);
    }

    ui->lblText->resize(ui->lblText->width(), 60);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    connect(&m_timer, &QTimer::timeout, this, &PopupHelloWindow::onTimer);

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

    bool isTaskbarOnRight = desktopWidth < screenWidth && desktop.left() == 0;
    bool isTaskbarOnLeft = desktopWidth < screenWidth && desktop.left() != 0;
    bool isTaskBarOnTop = desktopHeight < screenHeight && desktop.top() != 0;

    qint32 timeToShow = TIME_TO_SHOW;
    qint32 timerDelay;

    m_increment = 2;

    if (isTaskbarOnRight)
    {
        m_startPosX = desktop.right();
        m_startPosY = desktop.bottom() - height();
        m_taskbarPlacement = TASKBAR_ON_RIGHT;
        timerDelay = timeToShow / (width() / m_increment);
    }
    else if (isTaskbarOnLeft)
    {
        m_startPosX = desktop.left() - width();
        m_startPosY = desktop.bottom() - height();
        m_taskbarPlacement = TASKBAR_ON_LEFT;
        timerDelay = timeToShow / (width() / m_increment);
    }
    else if (isTaskBarOnTop)
    {
        m_startPosX = desktop.right() - width();
        m_startPosY = desktop.top() - height();
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

    move(m_currentPosX, m_currentPosY);

    m_appearing = true;

    m_timer.setInterval(timerDelay);
    m_timer.start();
}

PopupHelloWindow::~PopupHelloWindow()
{
    delete ui;
}

/**
 * Выполняет запуск таймера для последующего закрытия окна.
 */
void PopupHelloWindow::onPopupTimeout()
{
    if (isVisible())
        m_timer.start();
}

/**
 * Выполняет запуск таймера существования окна.
 */
void PopupHelloWindow::startPopupWaitingTimer()
{
    m_appearing = false;

    m_timer.stop();

    qint32 time2live = TIME_TO_LIVE;

    QTimer::singleShot(time2live, this, SLOT(onPopupTimeout()));
}

/**
 * Выполняет закрытие и удаление объекта окна.
 */
void PopupHelloWindow::closeAndDestroy()
{
    hide();

    m_timer.stop();

    s_popupHelloWindows.removeOne(this);

    this->deleteLater();
}

/**
 * Выполняет операции динамического появления и последующего закрытия окна.
 */
void PopupHelloWindow::onTimer()
{
    if (m_appearing)
    {
        switch (m_taskbarPlacement)
        {
        case TASKBAR_ON_BOTTOM:
            if (m_currentPosY > (m_startPosY - height()))
                m_currentPosY -= m_increment;
            else
                startPopupWaitingTimer();
            break;
        case TASKBAR_ON_TOP:
            if ((m_currentPosY - m_startPosY) < height())
                m_currentPosY += m_increment;
            else
                startPopupWaitingTimer();
            break;
        case TASKBAR_ON_LEFT:
            if ((m_currentPosX - m_startPosX) < width())
                m_currentPosX += m_increment;
            else
                startPopupWaitingTimer();
            break;
        case TASKBAR_ON_RIGHT:
            if (m_currentPosX > (m_startPosX - width()))
                m_currentPosX -= m_increment;
            else
                startPopupWaitingTimer();
            break;
        }
    }
    else
    {
        switch (m_taskbarPlacement)
        {
        case TASKBAR_ON_BOTTOM:
            if (m_currentPosY < m_startPosY)
                m_currentPosY += m_increment;
            else
            {
                closeAndDestroy();
                return;
            }
            break;
        case TASKBAR_ON_TOP:
            if (m_currentPosY > m_startPosY)
                m_currentPosY -= m_increment;
            else
            {
                closeAndDestroy();
                return;
            }
            break;
        case TASKBAR_ON_LEFT:
            if (m_currentPosX > m_startPosX)
                m_currentPosX -= m_increment;
            else
            {
                closeAndDestroy();
                return;
            }
            break;
        case TASKBAR_ON_RIGHT:
            if (m_currentPosX < m_startPosX)
                m_currentPosX += m_increment;
            else
            {
                closeAndDestroy();
                return;
            }
            break;
        }
    }

    move(m_currentPosX, m_currentPosY);
}

/**
 * Выполняет создание окна и отображение в нём полученной информации из класса Outcall.
 */
void PopupHelloWindow::showInformationMessage(const QString& caption, const QString& message, QPixmap avatar)
{
    PopupHelloWindowInfo phwi;

    if (caption != "")
        phwi.text = tr("<b>%1</b><br>%2").arg(caption, message);
    else
        phwi.text = message;

    if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

    phwi.avatar = avatar;

    PopupHelloWindow* popup = new PopupHelloWindow(phwi);

    popup->show();

    s_popupHelloWindows.append(popup);
}

/**
 * Выполняет закрытие и удаление всех объектов окон.
 */
void PopupHelloWindow::closeAll()
{
    for (qint32 i = 0; i < s_popupHelloWindows.size(); ++i)
        s_popupHelloWindows[i]->deleteLater();

    s_popupHelloWindows.clear();
}

/**
 * Выполняет закрытие окна при нажатии по нему кнопкой мыши.
 */
void PopupHelloWindow::mousePressEvent(QMouseEvent*)
{
    m_appearing = false;

    m_timer.start();
}
