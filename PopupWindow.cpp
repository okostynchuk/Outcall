#include "PopupWindow.h"
#include "ui_PopupWindow.h"

#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QSqlQuery>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QTextCursor>
#include <QMessageBox>

QList<PopupWindow*> PopupWindow::m_PopupWindows;
int PopupWindow::m_nLastWindowPosition = 0;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800 //msec
#define TIME_TO_LIVE	60000 //msec

PopupWindow::PopupWindow(PWInformation& pwi, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::PopupWindow)
{
	m_pwi = pwi;

    ui->setupUi(this);

    userID = global::getSettingsValue("user_login", "settings").toString();

    QSqlDatabase db;
    QSqlQuery query(db);
    QString note;
    query.prepare("SELECT note FROM calls WHERE uniqueid = " + pwi.uniqueid);
    query.exec();
    while(query.next())
        note = query.value(0).toString();
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setText(note);

	ui->lblText->setOpenExternalLinks(true);
	setAttribute(Qt::WA_TranslucentBackground);

	ui->lblText->setText(pwi.text);
    ui->lblText->setTextInteractionFlags(Qt::TextSelectableByMouse);

	if (!pwi.avatar.isNull()) {
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

	nDesktopWidth=rcDesktop.width();
	nDesktopHeight=rcDesktop.height();
	nScreenWidth=rcScreen.width();
	nScreenHeight=rcScreen.height();

    bool bTaskbarOnRight=nDesktopWidth<=nScreenWidth && rcDesktop.left()==0;
    bool bTaskbarOnLeft=nDesktopWidth<=nScreenWidth && rcDesktop.left()!=0;
    bool bTaskBarOnTop=nDesktopHeight<=nScreenHeight && rcDesktop.top()!=0;

	int nTimeToShow = TIME_TO_SHOW;
	int nTimerDelay;

	m_nIncrement = 2;

	if (bTaskbarOnRight)
	{
		m_nStartPosX=(rcDesktop.right()-m_nLastWindowPosition*width());
		m_nStartPosY=rcDesktop.bottom()-height();
		m_nTaskbarPlacement=TASKBAR_ON_RIGHT;
        nTimerDelay=nTimeToShow/(width()/m_nIncrement);
	}
	else if (bTaskbarOnLeft)
	{
		m_nStartPosX=(rcDesktop.left()-width()+m_nLastWindowPosition*width());
		m_nStartPosY=rcDesktop.bottom()-height();
		m_nTaskbarPlacement=TASKBAR_ON_LEFT;
		nTimerDelay=nTimeToShow/(width()/m_nIncrement);
	}
	else if (bTaskBarOnTop)
	{
		m_nStartPosX=rcDesktop.right()-width();
		m_nStartPosY=(rcDesktop.top()-height()+m_nLastWindowPosition*height());
		m_nTaskbarPlacement=TASKBAR_ON_TOP;
		nTimerDelay=nTimeToShow/(height()/m_nIncrement);
	}
    else
	{
		m_nStartPosX=rcDesktop.right()-width();
		m_nStartPosY=(rcDesktop.bottom()-m_nLastWindowPosition*height());
		m_nTaskbarPlacement=TASKBAR_ON_BOTTOM;
		nTimerDelay=nTimeToShow/(height()/m_nIncrement);
	}

    m_nCurrentPosX=m_nStartPosX;
    m_nCurrentPosY=m_nStartPosY;

    move(m_nCurrentPosX, m_nCurrentPosY);

    //m_nLastWindowPosition++;

    m_bAppearing = true;
    m_timer.setInterval(nTimerDelay);
    m_timer.start();
}

PopupWindow::~PopupWindow()
{
    delete ui;
}

void PopupWindow::mousePressEvent(QMouseEvent *event)
{
    this->m_pwi.stopTimer = true;
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void PopupWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->m_pwi.stopTimer = true;
    move(event->globalX()-m_nMouseClick_X_Coordinate,event->globalY()-m_nMouseClick_Y_Coordinate);
}

void PopupWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PopupWindow::onPopupTimeout()
{
    if (isVisible() && this->m_pwi.stopTimer == false)
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
    if (m_PopupWindows.last() == this)
        m_nLastWindowPosition = m_PopupWindows.count() - 1;

    hide();
    m_timer.stop();
    m_PopupWindows.removeOne(this);
    delete this;
}

void PopupWindow::onTimer()
{
    if (m_bAppearing)
    {
        switch(m_nTaskbarPlacement)
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

void PopupWindow::showCallNotification(QString dateTime, QString uniqueid, QString number, QString caller, QString my_number)
{
	PWInformation pwi;
	pwi.type = PWPhoneCall;
    pwi.text = tr("<font size = 1>%1</font><br>Входящий звонок от:<br><b>%2</b>").arg(dateTime).arg(caller);
    pwi.uniqueid = uniqueid;
    pwi.number = number;
    pwi.my_number = my_number;
    QPixmap avatar;

    if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

    PopupWindow *popup = new PopupWindow(pwi);
    popup->receiveNumber(popup);
	popup->show();
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

    PopupWindow *popup = new PopupWindow(pwi);
	popup->show();
	m_PopupWindows.append(popup);
}

void PopupWindow::closeAll()
{
	qDeleteAll(m_PopupWindows);
	m_PopupWindows.clear();
	m_nLastWindowPosition = 0;
}

void PopupWindow::on_pushButton_close_clicked()
{
    if (m_PopupWindows.last() == this)
        m_nLastWindowPosition = m_PopupWindows.count() - 1;

    hide();
    m_timer.stop();
    m_PopupWindows.removeOne(this);
    delete this;
}

void PopupWindow::onAddPerson()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    popup->m_pwi.stopTimer = true;

    addContactDialog = new AddContactDialog;
    addContactDialog->setValuesPopupWindow(popup->m_pwi.number);
    connect(addContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addContactDialog->setProperty("qv_popup", qv_popup);
    addContactDialog->show();
    addContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onAddOrg()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    popup->m_pwi.stopTimer = true;

    addOrgContactDialog = new AddOrgContactDialog;
    addOrgContactDialog->setOrgValuesPopupWindow(popup->m_pwi.number);
    connect(addOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addOrgContactDialog->setProperty("qv_popup", qv_popup);
    addOrgContactDialog->show();
    addOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onAddPhoneNumberToContact()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    popup->m_pwi.stopTimer = true;

    addPhoneNumberToContactDialog = new AddPhoneNumberToContactDialog;
    addPhoneNumberToContactDialog->setPhoneNumberPopupWindow(popup->m_pwi.number);
    connect(addPhoneNumberToContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addPhoneNumberToContactDialog->setProperty("qv_popup", qv_popup);
    addPhoneNumberToContactDialog->show();
    addPhoneNumberToContactDialog->setAttribute(Qt::WA_DeleteOnClose);
}

void PopupWindow::onShowCard()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    popup->m_pwi.stopTimer = true;

    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT id, entry_type, entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + popup->m_pwi.number + "')");
    query.exec();
    query.first();
    QString updateID = query.value(0).toString();

    if (query.value(1).toString() == "person")
    {
         viewContactDialog = new ViewContactDialog;
         viewContactDialog->setValuesContacts(updateID);
         connect(viewContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
         viewContactDialog->setProperty("qv_popup", qv_popup);
         viewContactDialog->show();
         viewContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        viewOrgContactDialog->setProperty("qv_popup", qv_popup);
        viewOrgContactDialog->show();
        viewOrgContactDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
}

void PopupWindow::onSaveNote()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    popup->m_pwi.stopTimer = true;

    QSqlDatabase db;
    QSqlQuery query(db);

    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if(ui->textEdit->toPlainText().isEmpty() || ui->textEdit->toPlainText() == 0 )
        return;

    query.prepare("SELECT EXISTS (SELECT uniqueid, note FROM calls WHERE uniqueid = " + popup->m_pwi.uniqueid + " AND note = '" + ui->textEdit->toPlainText() + "')");
    query.exec();
    query.next();
    if(query.value(0) != 0)
    {
        setStyleSheet("QMessageBox{ color: #000000; }");
        QMessageBox::information(this, trUtf8("Уведомление"), trUtf8("Данная заметка уже существует!"), QMessageBox::Ok);
        return;
    }

    query.prepare("SELECT entry_name FROM entry_phone WHERE entry_phone = " + m_pwi.my_number);
    query.exec();
    query.next();
    QString author = query.value(0).toString();

    query.prepare("INSERT INTO calls (uniqueid, datetime, note, author) VALUES(?, ?, ?, ?)");
    query.addBindValue(popup->m_pwi.uniqueid);
    query.addBindValue(dateTime);
    query.addBindValue(ui->textEdit->toPlainText());
    query.addBindValue(author);
    query.exec();

    popup->ui->textEdit->setStyleSheet("border: 2px solid lightgreen; background-color: #1a1a1a; border-radius: 5px;");
}

void PopupWindow::onOpenAccess()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();

    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT entry_vybor_id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + popup->m_pwi.number + "')");
    query.exec();
    query.first();
    QString vyborID = query.value(0).toString();

    QSqlDatabase dbMSSQL = QSqlDatabase::database("Third");
    QSqlQuery query1(dbMSSQL);

    if (dbMSSQL.isOpen())
    {
        query1.prepare("INSERT INTO CallTable (UserID, ClientID)"
                   "VALUES (?, ?)");
        query1.addBindValue(userID);
        query1.addBindValue(vyborID);
        query1.exec();
    }
    else
    {
        QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Отсутствует подлючение к базе Access!"), QMessageBox::Ok);
    }
    popup->ui->openAccess->hide();
}

void PopupWindow::receiveData(bool update)
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();

    if (update)
    {
        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT id, entry_name FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + popup->m_pwi.number + "')");
        query.exec();

        if (query.next())
        {
            popup->ui->label->hide();
            popup->ui->addPersonButton->hide();
            popup->ui->addOrgButton->hide();
            popup->ui->addPhoneNumberButton->hide();
            popup->ui->showCardButton->show();
            popup->ui->lblText->setText(tr("Входящий звонок от:<br><b>%1 (%2)</b>").arg(query.value(1).toString()).arg(popup->m_pwi.number));
        }
        else
        {
            popup->ui->showCardButton->hide();
            popup->ui->label->show();
            popup->ui->addPersonButton->show();
            popup->ui->addOrgButton->show();
            popup->ui->lblText->setText(tr("Входящий звонок от:<br><b>Неизвестный (%1)</b>").arg(popup->m_pwi.number));
        }
    }
}

void PopupWindow::onTextChanged()
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    popup->m_pwi.stopTimer = true;

    popup->ui->textEdit->setStyleSheet("border: 2px solid grey; background-color: #1a1a1a; border-radius: 5px;");
}

void PopupWindow::timerStop(QString uniqueid)
{
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();

    if (popup->m_pwi.uniqueid == uniqueid)
        popup->m_pwi.stopTimer = true;
}

bool PopupWindow::isInnerPhone(QString *str)
{
    int pos = 0;

    QRegExpValidator validator(QRegExp("[2][0-9]{2}"));
    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;
    return false;
}

bool PopupWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object->objectName() == "textEdit")
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return)
            {
                object->setObjectName("textEdit2");
                return true;
            }
        }
    }
    else if (object->objectName() == "textEdit2")
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return)
            {
                object->setObjectName("textEdit");
                return true;
            }
        }
    }
    return false;
}

void PopupWindow::receiveNumber(PopupWindow *popup)
{
    QVariant qv_popup = qVariantFromValue((void *)popup);

    if (isInnerPhone(&popup->m_pwi.number))
    {
        popup->ui->label->hide();
        popup->ui->addPersonButton->hide();
        popup->ui->addOrgButton->hide();
        popup->ui->showCardButton->hide();
        popup->ui->addPhoneNumberButton->hide();
    }
    else
    {
        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + popup->m_pwi.number + "')");
        query.exec();

        if (query.next())
        {
            popup->ui->label->hide();
            popup->ui->addPersonButton->hide();
            popup->ui->addOrgButton->hide();
            popup->ui->addPhoneNumberButton->hide();
        }
        else
            popup->ui->showCardButton->hide();
    }

    popup->ui->textEdit->installEventFilter(this);

    connect(popup->ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSaveNote()));
    popup->ui->textEdit->setProperty("qv_popup", qv_popup);
    connect(g_pAsteriskManager, SIGNAL(callStart(QString)), this, SLOT(timerStop(QString)));
    g_pAsteriskManager->setProperty("qv_popup", qv_popup);
    connect(popup->ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    popup->ui->textEdit->setProperty("qv_popup", qv_popup);
    connect(popup->ui->addPersonButton, SIGNAL(clicked(bool)), this, SLOT(onAddPerson()));
    popup->ui->addPersonButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->addOrgButton, SIGNAL(clicked(bool)), this, SLOT(onAddOrg()));
    popup->ui->addOrgButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->addPhoneNumberButton, SIGNAL(clicked(bool)), this, SLOT(onAddPhoneNumberToContact()));
    popup->ui->addPhoneNumberButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->showCardButton, SIGNAL(clicked(bool)), this, SLOT(onShowCard()));
    popup->ui->showCardButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->saveNoteButton, SIGNAL(clicked(bool)), this, SLOT(onSaveNote()));
    popup->ui->saveNoteButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->openAccess, SIGNAL(clicked(bool)), this, SLOT(onOpenAccess()));
    popup->ui->openAccess->setProperty("qv_popup", qv_popup);
}
