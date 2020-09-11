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

QList<PopupWindow*> PopupWindow::m_PopupWindows;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800 // msec
#define TIME_TO_LIVE	60000 // msec

PopupWindow::PopupWindow(PWInformation& pwi, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::PopupWindow)
{
	m_pwi = pwi;

    ui->setupUi(this);

    this->setStyleSheet("QMenu                       {margin: 0px; padding: 0px; background-color: #F2F2F2;}"
                      "QMenu::item                   {color: black; background-color: #F2F2F2;}"
                      "QMenu::separator              {background-color: #A9A9A9; black; height: 1px;}"
                      "QMenu::item:selected          {background-color: #18B7FF; color: #000000;}"
                      "QMenu::item:disabled          {background-color: #F2F2F2; color: #A9A9A9;}"
                      "QMenu::item:disabled:selected {background-color: #F2F2F2; color: #A9A9A9;}");

    userID = global::getSettingsValue("user_login", "settings").toString();

    author = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    QSqlQuery query(db);

    QString note;

    query.prepare("SELECT note FROM calls WHERE uniqueid = " + m_pwi.uniqueid);
    query.exec();

    if (query.next())
        note = query.value(0).toString();

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);

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

PopupWindow::~PopupWindow()
{
    delete ui;
}

void PopupWindow::mousePressEvent(QMouseEvent* event)
{
    m_pwi.stopTimer = true;

    position = event->globalPos();
}

void PopupWindow::mouseReleaseEvent(QMouseEvent* event)
{
    (void) event;
    position = QPoint();
}

void PopupWindow::mouseMoveEvent(QMouseEvent* event)
{
    m_pwi.stopTimer = true;

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

void PopupWindow::changeEvent(QEvent* event)
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

void PopupWindow::onPopupTimeout()
{
    if (isVisible() && m_pwi.stopTimer == false)
        m_timer.start();
}

void PopupWindow::startPopupWaitingTimer()
{
    m_bAppearing = false;

    m_timer.stop();

    int time2live = TIME_TO_LIVE;

    QTimer::singleShot(time2live, this, SLOT(onPopupTimeout()));
}

void PopupWindow::closeAndDestroy()
{
    hide();

    m_timer.stop();

    m_PopupWindows.removeOne(this);

    delete this;
}

void PopupWindow::onTimer()
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

void PopupWindow::showCallNotification(QString dateTime, QString uniqueid, QString number, QString caller, QString my_number)
{
	PWInformation pwi;

	pwi.type = PWPhoneCall;
    pwi.text = caller;
    pwi.uniqueid = uniqueid;
    pwi.number = number;
    pwi.my_number = my_number;

    QPixmap avatar;

    if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

    PopupWindow* popup = new PopupWindow(pwi);

    popup->controlPopup();
	popup->show();

    popup->ui->timeLabel->setText(tr("<font size = 1>%1</font>").arg(dateTime));

	m_PopupWindows.append(popup);
}

void PopupWindow::showInformationMessage(QString caption, QString message, QPixmap avatar, PWType type)
{
	PWInformation pwi;

	pwi.type = type;

	if (caption!="")
		pwi.text = tr("<b>%1</b><br>%2").arg(caption).arg(message);
	else
		pwi.text = message;

	if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

	pwi.avatar = avatar;

    PopupWindow* popup = new PopupWindow(pwi);

	popup->show();

	m_PopupWindows.append(popup);
}

void PopupWindow::closeAll()
{
    for (int i = 0; i < m_PopupWindows.size(); ++i)
        m_PopupWindows[i]->deleteLater();

    m_PopupWindows.clear();
}

void PopupWindow::on_closeButton_clicked()
{
    closeAndDestroy();
}

void PopupWindow::onAddPerson()
{
    m_pwi.stopTimer = true;

    if (!addContactDialog.isNull())
        addContactDialog.data()->close();

    addContactDialog = new AddContactDialog;
    addContactDialog.data()->setValues(m_pwi.number);
    connect(addContactDialog.data(), &AddContactDialog::sendData, this, &PopupWindow::receiveData);
    addContactDialog.data()->show();
    addContactDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onAddOrg()
{
    m_pwi.stopTimer = true;

    if (!addOrgContactDialog.isNull())
        addOrgContactDialog.data()->close();

    addOrgContactDialog = new AddOrgContactDialog;
    addOrgContactDialog.data()->setOrgValuesPopupWindow(m_pwi.number);
    connect(addOrgContactDialog.data(), &AddOrgContactDialog::sendData, this, &PopupWindow::receiveData);
    addOrgContactDialog.data()->show();
    addOrgContactDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onAddPhoneNumberToContact()
{
    m_pwi.stopTimer = true;

    if (!addPhoneNumberToContactDialog.isNull())
        addPhoneNumberToContactDialog.data()->close();

    addPhoneNumberToContactDialog = new AddPhoneNumberToContactDialog;
    addPhoneNumberToContactDialog.data()->setPhoneNumber(m_pwi.number);
    connect(addPhoneNumberToContactDialog.data(), &AddPhoneNumberToContactDialog::sendData, this, &PopupWindow::receiveData);
    addPhoneNumberToContactDialog.data()->show();
    addPhoneNumberToContactDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onShowCard()
{
    m_pwi.stopTimer = true;

    QSqlQuery query(db);

    query.prepare("SELECT id, entry_type, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
    query.exec();
    query.first();

    QString updateID = query.value(0).toString();

    if (query.value(1).toString() == "person")
    {
        if (!viewContactDialog.isNull())
            viewContactDialog.data()->close();

        viewContactDialog = new ViewContactDialog;
        viewContactDialog.data()->setValuesContacts(updateID);
        connect(viewContactDialog.data(), &ViewContactDialog::sendData, this, &PopupWindow::receiveData);
        viewContactDialog.data()->show();
        viewContactDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        if (!viewOrgContactDialog.isNull())
            viewOrgContactDialog.data()->close();

        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog.data()->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog.data(), &ViewOrgContactDialog::sendData, this, &PopupWindow::receiveData);
        viewOrgContactDialog.data()->show();
        viewOrgContactDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void PopupWindow::onAddReminder()
{
    m_pwi.stopTimer = true;

    if (!addReminderDialog.isNull())
        addReminderDialog.data()->close();

    addReminderDialog = new AddReminderDialog;
    addReminderDialog.data()->setCallId(m_pwi.uniqueid);
    addReminderDialog.data()->show();
    addReminderDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onViewNotes()
{
    m_pwi.stopTimer = true;

    QString loadState;

    if (!notesDialog.isNull())
        notesDialog.data()->close();

    notesDialog = new NotesDialog;
    notesDialog.data()->receiveData(m_pwi.uniqueid, m_pwi.number, loadState);
    notesDialog.data()->hideAddNote();
    notesDialog.data()->show();
    notesDialog.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onOpenAccess()
{
    m_pwi.stopTimer = true;

    QSqlQuery query(db);

    query.prepare("SELECT entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
    query.exec();
    query.first();

    QString vyborID = query.value(0).toString();

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

        QMessageBox::critical(this, tr("Ошибка"), tr("Отсутствует подключение к базе заказов!"), QMessageBox::Ok);
    }
}

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
            QSqlQuery query(db);

            query.prepare("SELECT id, entry_name, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
            query.exec();

            if (query.next())
            {
                if (!addContactDialog.isNull())
                    addContactDialog.data()->close();

                if (!addOrgContactDialog.isNull())
                    addOrgContactDialog.data()->close();

                if (!addPhoneNumberToContactDialog.isNull())
                    addPhoneNumberToContactDialog.data()->close();

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
                if (!viewContactDialog.isNull())
                    viewContactDialog.data()->close();

                if (!viewOrgContactDialog.isNull())
                    viewOrgContactDialog.data()->close();

                ui->openAccessButton->hide();
                ui->showCardButton->hide();

                ui->addPersonButton->show();
                ui->addOrgButton->show();
                ui->addPhoneNumberButton->show();

                ui->lblText->setText("<b style='color:white'>" + m_pwi.number + "</b><br><b>" + tr("Неизвестный") + "</b>");
                m_pwi.text = ("<b style='color:white'>" + m_pwi.number + "</b><br><b>" + tr("Неизвестный") + "</b>");
            }
        }

        if (!MSSQLopened)
            ui->openAccessButton->hide();
    }
}

void PopupWindow::onSaveNote()
{
    m_pwi.stopTimer = true;

    QSqlQuery query(db);

    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if (ui->textEdit->toPlainText().trimmed().isEmpty())
        return;

    query.prepare("INSERT INTO calls (uniqueid, datetime, note, author, phone_number) VALUES(?, ?, ?, ?, ?)");
    query.addBindValue(m_pwi.uniqueid);
    query.addBindValue(dateTime);
    query.addBindValue(ui->textEdit->toPlainText().trimmed());
    query.addBindValue(author);
    query.addBindValue(m_pwi.number);
    query.exec();

    ui->textEdit->setStyleSheet("border: 2px solid lightgreen; background-color: #1a1a1a; border-right-color: transparent;");
    ui->saveNoteButton->setStyleSheet("border: 2px solid lightgreen; background-color: #e1e1e1; border-left-color: transparent;");
}

void PopupWindow::onTextChanged()
{
    m_pwi.stopTimer = true;

    if (ui->textEdit->toPlainText().trimmed().length() > 255)
    {
        ui->textEdit->textCursor().deletePreviousChar();

        return;
    }

    ui->textEdit->setStyleSheet("border: 2px solid grey; background-color: #1a1a1a;");
    ui->saveNoteButton->setStyleSheet("background-color: #e1e1e1; border-style: solid; border-width: 2px; border-top-right-radius: 7px; border-bottom-right-radius: 7px; border-color: grey; border-left-color: transparent;");
}

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

void PopupWindow::onCallStart(QString uniqueid)
{
    ui->lblAvatar->setPixmap(QPixmap(":/images/connected.png"));

    if (m_pwi.uniqueid == uniqueid)
        m_pwi.stopTimer = true;
}

bool PopupWindow::isInternalPhone(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("^[0-9]{4}$"));
    QRegularExpressionValidator validator2(QRegularExpression("^[2][0-9]{2}$"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void PopupWindow::controlPopup()
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
        QSqlQuery query(db);

        query.prepare("SELECT id, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + m_pwi.number + "')");
        query.exec();

        if (query.next())
        {
            ui->addPersonButton->hide();
            ui->addOrgButton->hide();
            ui->addPhoneNumberButton->hide();

            if (query.value(1) == 0)
                ui->openAccessButton->hide();
        }
        else
        {
            ui->openAccessButton->hide();
            ui->showCardButton->hide();
        }
    }

    if (!MSSQLopened)
        ui->openAccessButton->hide();

    connect(g_pAsteriskManager, &AsteriskManager::callStart, this, &PopupWindow::onCallStart);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &PopupWindow::onTextChanged);
    connect(ui->addPersonButton, &QAbstractButton::clicked, this, &PopupWindow::onAddPerson);
    connect(ui->addOrgButton, &QAbstractButton::clicked, this, &PopupWindow::onAddOrg);
    connect(ui->addPhoneNumberButton, &QAbstractButton::clicked, this, &PopupWindow::onAddPhoneNumberToContact);
    connect(ui->showCardButton, &QAbstractButton::clicked, this, &PopupWindow::onShowCard);
    connect(ui->saveNoteButton, &QAbstractButton::clicked, this, &PopupWindow::onSaveNote);
    connect(ui->openAccessButton, &QAbstractButton::clicked, this, &PopupWindow::onOpenAccess);
    connect(ui->addReminderButton, &QAbstractButton::clicked, this, &PopupWindow::onAddReminder);
    connect(ui->viewNotesButton, &QAbstractButton::clicked, this, &PopupWindow::onViewNotes);
}
