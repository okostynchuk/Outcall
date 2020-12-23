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

QList<PopupNotification*> PopupNotification::s_popupNotifications;

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

    connect(&m_timer, &QTimer::timeout, this, &PopupNotification::onTimer);

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
        m_startPosX = desktop.right();
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

PopupNotification::~PopupNotification()
{
    delete ui;
}

/**
 * Выполняет сохранение позиции нажатия мышью по окну.
 */
void PopupNotification::mousePressEvent(QMouseEvent* event)
{
    m_position = event->globalPos();
}

/**
 * Выполняет установку нулевой позиции при отжатии кнопки мыши.
 */
void PopupNotification::mouseReleaseEvent(QMouseEvent* event)
{
    (void) event;

    m_position = QPoint();
}

/**
 * Выполняет изменение позиции окна на экране.
 */
void PopupNotification::mouseMoveEvent(QMouseEvent* event)
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
 * Выполняет запуск таймера для последующего закрытия окна.
 */
void PopupNotification::onClosePopup()
{
    QSqlQuery query(m_db);

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

    if (s_popupNotifications.length() == 1)
        emit m_pni.remindersDialog->reminders(false);

    s_popupNotifications.removeOne(this);

    this->deleteLater();
}

/**
 * Выполняет закрытие и удаление всех объектов окон.
 */
void PopupNotification::closeAll()
{
    for (qint32 i = 0; i < s_popupNotifications.size(); ++i)
        s_popupNotifications[i]->deleteLater();

    s_popupNotifications.clear();
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

    s_popupNotifications.append(notification);
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
