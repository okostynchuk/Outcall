/*
 * Класс служит для оповещения пользователя о звонке,
 * для добавления / редактирования списка контактов,
 * для добавления напоминания.
 */

#include "PopupWindow.h"
#include "ui_PopupWindow.h"

#include "Global.h"

#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QSqlQuery>
#include <QDateTime>
#include <QTextCursor>
#include <QMessageBox>

QList<PopupWindow*> PopupWindow::s_popupWindows;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800   // msec
#define TIME_TO_LIVE	60000 // msec

PopupWindow::PopupWindow(const PopupWindowInfo& pwi, QWidget* parent) :
    QDialog(parent),
	ui(new Ui::PopupWindow)
{
	m_pwi = pwi;

    ui->setupUi(this);

    this->installEventFilter(this);

    //QList<QLabel*> employees;
    QMap<QString, QLabel*> m_managers;

    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM groups");
    query.exec();

    while(query.next())
    {
        QLabel* label = new QLabel;

        m_managers.insert(query.value(0).toString(), label);
        label->setText(query.value(1).toString() + " (" + query.value(0).toString() + "): ");

        ui->verticalLayout->addWidget(label);
    }

//    if (isInternalPhone(&m_pwi.number))
//    {
//        ui->addPersonButton->hide();
//        ui->addOrgButton->hide();
//        ui->showCardButton->hide();
//        ui->addPhoneNumberButton->hide();
//        ui->openAccessButton->hide();
//    }
//    else
//    {
        query.prepare("SELECT id, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
        query.exec();

        if (query.next())
        {
            ui->addPersonButton->hide();
            ui->addOrgButton->hide();
            ui->addPhoneNumberButton->hide();

            if (query.value(1) == 0)
                ui->openAccessButton->hide();

            query.prepare("SELECT group_number, manager_number FROM managers WHERE entry_id = '" + query.value(0).toString() + "' ORDER BY group_number");
            query.exec();

            while (query.next())
                if (m_managers.keys().contains(query.value(0).toString()))
                {
                    QString str = m_managers.value(query.value(0).toString())->text();
                    str.append(query.value(1).toString());
                    m_managers.value(query.value(0).toString())->setText(str);
                    //m_managers.value(query.value(0).toString())->setText(query.value(1).toString());
                }
        }
        else
        {
            ui->openAccessButton->hide();
            ui->showCardButton->hide();
        }
    //}

    if (!g_ordersDbOpened)
        ui->openAccessButton->hide();

    connect(g_asteriskManager,       &AsteriskManager::callStart, this, &PopupWindow::onCallStart);

    connect(ui->textEdit,             &QTextEdit::textChanged,   this, &PopupWindow::onTextChanged);
    connect(ui->textEdit,             &QTextEdit::cursorPositionChanged, this, &PopupWindow::onCursorPosChanged);

    connect(ui->addOrgButton,         &QAbstractButton::clicked, this, &PopupWindow::onAddOrg);
    connect(ui->saveNoteButton,       &QAbstractButton::clicked, this, &PopupWindow::onSaveNote);
    connect(ui->showCardButton,       &QAbstractButton::clicked, this, &PopupWindow::onShowCard);
    connect(ui->viewNotesButton,      &QAbstractButton::clicked, this, &PopupWindow::onViewNotes);
    connect(ui->addPersonButton,      &QAbstractButton::clicked, this, &PopupWindow::onAddPerson);
    connect(ui->openAccessButton,     &QAbstractButton::clicked, this, &PopupWindow::onOpenAccess);
    connect(ui->addReminderButton,    &QAbstractButton::clicked, this, &PopupWindow::onAddReminder);
    connect(ui->addPhoneNumberButton, &QAbstractButton::clicked, this, &PopupWindow::onAddPhoneNumberToContact);

    QString note;

    query.prepare("SELECT note FROM calls WHERE uniqueid = " + m_pwi.uniqueid);
    query.exec();

    if (query.next())
        note = query.value(0).toString();

    ui->textEdit->setText(note);

	setAttribute(Qt::WA_TranslucentBackground);

    ui->lblText->setText(m_pwi.text);

    if (!pwi.avatar.isNull())
    {
		ui->lblAvatar->setScaledContents(true);
		ui->lblAvatar->setPixmap(pwi.avatar);
    }

    ui->lblText->resize(ui->lblText->width(), 60);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    connect(&m_timer, &QTimer::timeout, this, &PopupWindow::onTimer);

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

PopupWindow::~PopupWindow()
{
    delete ui;
}

/**
 * Изменяет позицию курсора.
 */
void PopupWindow::onCursorPosChanged()
{
    if (m_textCursor.isNull())
    {
        m_textCursor = ui->textEdit->textCursor();
        m_textCursor.movePosition(QTextCursor::End);
    }
    else
        m_textCursor = ui->textEdit->textCursor();
}

/**
 * Выполняет обработку совершения операций с привязанным объектом.
 */
bool PopupWindow::eventFilter(QObject*, QEvent* event)
{
    if (event && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);

        if (keyEvent && (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab))
        {
            if (ui->textEdit->hasFocus())
                ui->textEdit->setTextCursor(m_textCursor);

            return true;
        }
    }
    else if (event && event->type() == QEvent::WindowActivate)
    {
        if (ui->textEdit->toPlainText().trimmed().isEmpty())
            ui->textEdit->setFocus();
        else
            ui->saveNoteButton->setFocus();

        return true;
    }

    return false;
}

/**
 * Захват точки нажатия мышью по окну.
 */
void PopupWindow::mousePressEvent(QMouseEvent* event)
{
    m_pwi.stopTimer = true;

    m_position = event->globalPos();
}

/**
 * Устанавка нулевой позиции при отпускании клика.
 */
void PopupWindow::mouseReleaseEvent(QMouseEvent* event)
{
    (void) event;
    m_position = QPoint();
}

/**
 * Реализация изменения позиции окна на экране.
 */
void PopupWindow::mouseMoveEvent(QMouseEvent* event)
{
    m_pwi.stopTimer = true;

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
 * Реализация запуска таймера.
 */
void PopupWindow::onPopupTimeout()
{
    if (isVisible() && m_pwi.stopTimer == false)
        m_timer.start();
}

/**
 * Реализация остановки таймера.
 */
void PopupWindow::startPopupWaitingTimer()
{
    m_appearing = false;

    m_timer.stop();

    qint32 time2live = TIME_TO_LIVE;

    QTimer::singleShot(time2live, this, SLOT(onPopupTimeout()));
}

/**
 * Закрытие окна.
 */
void PopupWindow::closeAndDestroy()
{
    hide();

    m_timer.stop();

    s_popupWindows.removeOne(this);

    this->deleteLater();
}

/**
 * Реализация движения окна при открытии / закрытии окна.
 */
void PopupWindow::onTimer()
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
 * Реализация появления окна оповещения о звонке.
 */
void PopupWindow::showCall(const QString& dateTime, const QString& uniqueid, const QString& number, const QString& caller)
{
    PopupWindowInfo pwi;

    pwi.text = caller;
    pwi.uniqueid = uniqueid;
    pwi.number = number;

    QPixmap avatar;

    if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

    PopupWindow* popup = new PopupWindow(pwi);

    popup->ui->timeLabel->setText("<font size = 1>" + dateTime + "</font>");

    popup->show();

    s_popupWindows.append(popup);
}

/**
 * Удаление объектов и установка начальной позиции окна.
 */
void PopupWindow::closeAll()
{
    for (qint32 i = 0; i < s_popupWindows.size(); ++i)
        s_popupWindows[i]->deleteLater();

    s_popupWindows.clear();
}

/**
 * Закрытие окна по кнопке.
 */
void PopupWindow::on_closeButton_clicked()
{
    closeAndDestroy();
}

/**
 * Реализация кнопки добавления физ. лица.
 */
void PopupWindow::onAddPerson()
{
    m_pwi.stopTimer = true;

    if (!m_addContactDialog.isNull())
        m_addContactDialog->close();

    m_addContactDialog = new AddContactDialog;
    m_addContactDialog->setValues(m_pwi.number);
    connect(m_addContactDialog, &AddContactDialog::sendData, this, &PopupWindow::receiveData);
    m_addContactDialog->show();
    m_addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Реализация кнопки добавления организации.
 */
void PopupWindow::onAddOrg()
{
    m_pwi.stopTimer = true;

    if (!m_addOrgContactDialog.isNull())
        m_addOrgContactDialog->close();

    m_addOrgContactDialog = new AddOrgContactDialog;
    m_addOrgContactDialog->setValues(m_pwi.number);
    connect(m_addOrgContactDialog, &AddOrgContactDialog::sendData, this, &PopupWindow::receiveData);
    m_addOrgContactDialog->show();
    m_addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Реализация кнопки добавления номера к существующему контакту.
 */
void PopupWindow::onAddPhoneNumberToContact()
{
    m_pwi.stopTimer = true;

    if (!m_addPhoneNumberToContactDialog.isNull())
        m_addPhoneNumberToContactDialog->close();

    m_addPhoneNumberToContactDialog = new AddPhoneNumberToContactDialog;
    m_addPhoneNumberToContactDialog->setPhoneNumber(m_pwi.number);
    connect(m_addPhoneNumberToContactDialog, &AddPhoneNumberToContactDialog::sendData, this, &PopupWindow::receiveData);
    m_addPhoneNumberToContactDialog->show();
    m_addPhoneNumberToContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Реализация кнопки открытия карточки контакта.
 */
void PopupWindow::onShowCard()
{
    m_pwi.stopTimer = true;

    QSqlQuery query(m_db);

    query.prepare("SELECT id, entry_type, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
    query.exec();
    query.first();

    QString contactId = query.value(0).toString();

    if (query.value(1).toString() == "person")
    {
        if (!m_viewContactDialog.isNull())
            m_viewContactDialog->close();

        m_viewContactDialog = new ViewContactDialog;
        m_viewContactDialog->setValues(contactId);
        connect(m_viewContactDialog, &ViewContactDialog::sendData, this, &PopupWindow::receiveData);
        m_viewContactDialog->show();
        m_viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        if (!m_viewOrgContactDialog.isNull())
            m_viewOrgContactDialog->close();

        m_viewOrgContactDialog = new ViewOrgContactDialog;
        m_viewOrgContactDialog->setValues(contactId);
        connect(m_viewOrgContactDialog, &ViewOrgContactDialog::sendData, this, &PopupWindow::receiveData);
        m_viewOrgContactDialog->show();
        m_viewOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Реализация кнопки добавления напоминания к звонку.
 */
void PopupWindow::onAddReminder()
{
    m_pwi.stopTimer = true;

    if (!m_addReminderDialog.isNull())
        m_addReminderDialog->close();

    m_addReminderDialog = new AddReminderDialog;
    m_addReminderDialog->setCallId(m_pwi.uniqueid);
    m_addReminderDialog->show();
    m_addReminderDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Реализация кнопки просмотра комментариев по контакту.
 */
void PopupWindow::onViewNotes()
{
    m_pwi.stopTimer = true;

    if (!m_notesDialog.isNull())
        m_notesDialog->close();

    m_notesDialog = new NotesDialog;
    m_notesDialog->setValues(m_pwi.uniqueid, m_pwi.number);
    m_notesDialog->hideAddNote();
    m_notesDialog->show();
    m_notesDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Реализация кнопки открытия базы заказов;
 */
void PopupWindow::onOpenAccess()
{
    m_pwi.stopTimer = true;

    QSqlQuery query(m_db);

    query.prepare("SELECT entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
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
        setStyleSheet("QMessageBox{ color: #000000; }");

        MsgBoxError(tr("Отсутствует подключение к базе заказов!"));
    }
}

/**
 * Выполняет запрос на обновление списка контактов.
 */
void PopupWindow::receiveData(bool update)
{
    if (update)
    {
        if (isInternalPhone(&m_pwi.number))
        {
            ui->addPersonButton->hide();
            ui->addOrgButton->hide();
            ui->showCardButton->hide();
            ui->addPhoneNumberButton->hide();
            ui->openAccessButton->hide();
        }
        else
        {
            QSqlQuery query(m_db);

            query.prepare("SELECT id, entry_name, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
            query.exec();

            if (query.next())
            {
                if (!m_addContactDialog.isNull())
                    m_addContactDialog->close();

                if (!m_addOrgContactDialog.isNull())
                    m_addOrgContactDialog->close();

                if (!m_addPhoneNumberToContactDialog.isNull())
                    m_addPhoneNumberToContactDialog->close();

                ui->addPersonButton->hide();
                ui->addOrgButton->hide();
                ui->addPhoneNumberButton->hide();

                ui->showCardButton->show();
                ui->openAccessButton->show();

                if (query.value(2) == 0)
                    ui->openAccessButton->hide();
                else
                    ui->openAccessButton->show();

                ui->lblText->setText("<b style='color:white'>" + m_pwi.number + "</b><br><b>" + query.value(1).toString() + "</b>");
                m_pwi.text = ("<b style='color:white'>" + m_pwi.number + "</b><br><b>" + query.value(1).toString() + "</b>");
            }
            else
            {
                if (!m_viewContactDialog.isNull())
                    m_viewContactDialog->close();

                if (!m_viewOrgContactDialog.isNull())
                    m_viewOrgContactDialog->close();

                ui->openAccessButton->hide();
                ui->showCardButton->hide();

                ui->addPersonButton->show();
                ui->addOrgButton->show();
                ui->addPhoneNumberButton->show();

                ui->lblText->setText("<b style='color:white'>" + m_pwi.number + "</b><br><b>" + tr("Неизвестный") + "</b>");
                m_pwi.text = ("<b style='color:white'>" + m_pwi.number + "</b><br><b>" + tr("Неизвестный") + "</b>");
            }
        }

        if (!g_ordersDbOpened)
            ui->openAccessButton->hide();
    }
}

/**
 * Реализация кнопки сохранения заметок в список контактов.
 */
void PopupWindow::onSaveNote()
{
    m_pwi.stopTimer = true;

    QSqlQuery query(m_db);

    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if (ui->textEdit->toPlainText().trimmed().isEmpty())
        return;

    query.prepare("INSERT INTO calls (uniqueid, datetime, note, author, phone_number) VALUES(?, ?, ?, ?, ?)");
    query.addBindValue(m_pwi.uniqueid);
    query.addBindValue(dateTime);
    query.addBindValue(ui->textEdit->toPlainText().trimmed());
    query.addBindValue(g_personalNumberName);
    query.addBindValue(m_pwi.number);
    query.exec();

    ui->textEdit->setStyleSheet("border: 2px solid lightgreen; background-color: #1a1a1a; border-right-color: transparent;");
    ui->saveNoteButton->setStyleSheet("border: 2px solid lightgreen; background-color: #e1e1e1; border-left-color: transparent;");
}

/**
 * Выполнение остановки таймера и
 * изменение рамок окна ввода комментария при вводе текста.
 */
void PopupWindow::onTextChanged()
{
    m_pwi.stopTimer = true;

    qint32 maxTextLength = 255;

    if (ui->textEdit->toPlainText().length() > maxTextLength)
    {
        qint32 diff = ui->textEdit->toPlainText().length() - maxTextLength;

        QString newStr = ui->textEdit->toPlainText();
        newStr.chop(diff);

        ui->textEdit->setText(newStr);

        QTextCursor cursor(ui->textEdit->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

        ui->textEdit->setTextCursor(cursor);
    }

    ui->textEdit->setStyleSheet("border: 2px solid grey; background-color: #1a1a1a; border-right-color: transparent;");
    ui->saveNoteButton->setStyleSheet("background-color: #e1e1e1; border-style: solid; border-width: 2px; border-top-right-radius: 7px; border-bottom-right-radius: 7px; border-color: grey; border-left-color: transparent;");
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Enter.
 */
void PopupWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
         closeAndDestroy();
    else if (event->key() == Qt::Key_Return)
    {
        if (ui->textEdit->hasFocus())
            return;
        else
            ui->saveNoteButton->click();
    }
    else
        QDialog::keyPressEvent(event);
}

/**
 * Изменение иконки на окне в случае ответа на звонок.
 */
void PopupWindow::onCallStart(const QString& uniqueid)
{
    if (m_pwi.uniqueid == uniqueid)
    {
        m_pwi.stopTimer = true;

        ui->lblAvatar->setPixmap(QPixmap(":/images/connected.png"));
    }
}

/**
 * Выполняет проверку на внутренний номер.
 */
bool PopupWindow::isInternalPhone(QString* str)
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
