/*
 * Класс служит для оповещения пользователя о делегированном ему напоминании.
 */

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

#define TIME_TO_SHOW    800 // msec

PopupNotification::PopupNotification(const PopupNotificationInfo& pni, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PopupNotification)
{
    m_pni = pni;

    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);

    ui->label->setText(tr("Новое напоминание от ") + m_pni.number);

    QString note = m_pni.text;


    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
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

    for (int i = 0; i < hrefs.length(); ++i)
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
                     size, QString(firstCharList.at(i) + "<a href='" + hrefsNoCharacters.at(i) + "'>" + hrefsNoCharacters.at(i) + "</a>" + lastCharList.at(i)));
    }

    ui->textBrowser->setText(note);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    connect(&m_timer, &QTimer::timeout, this, &PopupNotification::onTimer);

    quint32 nDesktopHeight;
    quint32 nDesktopWidth;
    quint32 nScreenWidth;
    quint32 nScreenHeight;

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

    qint32 nTimeToShow = TIME_TO_SHOW;
    qint32 nTimerDelay;

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

/**
 * Выполняет сохранение позиции нажатия мышью по окну.
 */
void PopupNotification::mousePressEvent(QMouseEvent* event)
{
    position = event->globalPos();
}

/**
 * Выполняет установку нулевой позиции при отжатии кнопки мыши.
 */
void PopupNotification::mouseReleaseEvent(QMouseEvent* event)
{
    (void) event;

    position = QPoint();
}

/**
 * Выполняет изменение позиции окна на экране.
 */
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

/**
 * Выполняет операцию для последующего закрытия окна при нажатии кнопки "Х".
 */
void PopupNotification::on_pushButton_close_clicked()
{
    onClosePopup();
}

/**
 * Выполняет операции динамического появления и последующего закрытия окна.
 */
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

/**
 * Выполняет запуск таймера для последующего закрытия окна.
 */
void PopupNotification::onClosePopup()
{
    QSqlQuery query(db);

    query.prepare("UPDATE reminders SET viewed = true WHERE id = ?");
    query.addBindValue(m_pni.id);
    query.exec();

    if (isVisible())
        m_timer.start();
}

/**
 * Выполняет закрытие и удаление объекта окна.
 */
void PopupNotification::closeAndDestroy()
{
    hide();

    m_timer.stop();

    if (m_PopupNotifications.length() == 1)
        emit m_pni.remindersDialog->reminders(false);

    m_PopupNotifications.removeOne(this);

    delete this;
}

/**
 * Выполняет закрытие и удаление всех объектов окон.
 */
void PopupNotification::closeAll()
{
    for (qint32 i = 0; i < m_PopupNotifications.size(); ++i)
        m_PopupNotifications[i]->deleteLater();

    m_PopupNotifications.clear();
}

/**
 * Выполняет создание окна и отображение в нём полученной информации из класса RemindersDialog.
 */
void PopupNotification::showReminderNotification(RemindersDialog* remindersDialog, const QString& id, const QString& number, const QString& note)
{
    PopupNotificationInfo pni;

    pni.remindersDialog = remindersDialog;
    pni.id = id;
    pni.number = number;
    pni.note = note;

    pni.text = "<b> " + pni.note + " </b>";

    PopupNotification* notification = new PopupNotification(pni);

    notification->show();

    m_PopupNotifications.append(notification);
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void PopupNotification::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        onClosePopup();
    else
        QWidget::keyPressEvent(event);
}
