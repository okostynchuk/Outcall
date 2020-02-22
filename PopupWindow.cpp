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

QList<PopupWindow*> PopupWindow::m_PopupWindows;
int PopupWindow::m_nLastWindowPosition = 0;

#define TASKBAR_ON_TOP		1
#define TASKBAR_ON_LEFT		2
#define TASKBAR_ON_RIGHT	3
#define TASKBAR_ON_BOTTOM	4

#define TIME_TO_SHOW	800 //msec

PopupWindow::PopupWindow(const PWInformation& pwi, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::PopupWindow)
{
	m_pwi = pwi;

    ui->setupUi(this);

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

//    m_nLastWindowPosition++;

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
    m_nMouseClick_X_Coordinate = event->x();
    m_nMouseClick_Y_Coordinate = event->y();
}

void PopupWindow::mouseMoveEvent(QMouseEvent *event)
{
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
                m_timer.stop();
            break;
        case TASKBAR_ON_TOP:
            if ((m_nCurrentPosY-m_nStartPosY)<height())
                m_nCurrentPosY+=m_nIncrement;
            else
                m_timer.stop();
            break;
        case TASKBAR_ON_LEFT:
            if ((m_nCurrentPosX-m_nStartPosX)<width())
                m_nCurrentPosX+=m_nIncrement;
            else
                m_timer.stop();
            break;
        case TASKBAR_ON_RIGHT:
            if (m_nCurrentPosX>(m_nStartPosX-width()))
                m_nCurrentPosX-=m_nIncrement;
            else
                m_timer.stop();
            break;
        }
    }

    move(m_nCurrentPosX, m_nCurrentPosY);
}

void PopupWindow::showCallNotification(QString number, QString caller)
{
	PWInformation pwi;
	pwi.type = PWPhoneCall;
    pwi.text = tr("Входящий звонок от:<br><b>%1</b>").arg(caller);
    QPixmap avatar;

    if (avatar.isNull())
        avatar = QPixmap(":/images/outcall-logo.png");

    PopupWindow *popup = new PopupWindow(pwi);
    popup->recieveNumber(popup, number);
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
    QString number = sender()->property("number").toString();
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    addContactDialog = new AddContactDialog;
    addContactDialog->setValuesPopupWindow(number);
    connect(addContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addContactDialog->setProperty("number", number);
    addContactDialog->setProperty("qv_popup", qv_popup);
    addContactDialog->exec();
    addContactDialog->deleteLater();
}

void PopupWindow::onAddOrg()
{
    QString number = sender()->property("number").toString();
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    addOrgContactDialog = new AddOrgContactDialog;
    addOrgContactDialog->setOrgValuesPopupWindow(number);
    connect(addOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    addOrgContactDialog->setProperty("number", number);
    addOrgContactDialog->setProperty("qv_popup", qv_popup);
    addOrgContactDialog->exec();
    addOrgContactDialog->deleteLater();
}

void PopupWindow::onShowCard()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString number = sender()->property("number").toString();
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    QString updateID = getUpdateId(number);
    query.prepare("SELECT entry_type FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();
    if (query.value(0).toString() == "person")
    {
         viewContactDialog = new ViewContactDialog;
         viewContactDialog->setValuesContacts(updateID);
         connect(viewContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
         viewContactDialog->setProperty("number", number);
         viewContactDialog->setProperty("qv_popup", qv_popup);
         viewContactDialog->exec();
         viewContactDialog->deleteLater();
    }
    else
    {
        viewOrgContactDialog = new ViewOrgContactDialog;
        viewOrgContactDialog->setOrgValuesContacts(updateID);
        connect(viewOrgContactDialog, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
        viewOrgContactDialog->setProperty("number", number);
        viewOrgContactDialog->setProperty("qv_popup", qv_popup);
        viewOrgContactDialog->exec();
        viewOrgContactDialog->deleteLater();
    }
}

QString PopupWindow::getUpdateId(QString &number)
{
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();
    QString updateID = query.value(0).toString();
    return updateID;
}

void PopupWindow::receiveData(bool update)
{
    QString number = sender()->property("number").toString();
    QVariant qv_popup = sender()->property("qv_popup");
    PopupWindow *popup;
    popup = (PopupWindow*)qv_popup.value<void *>();
    if (update)
    {
        QSqlDatabase db;
        QSqlQuery query(db);
        query.prepare("SELECT id, entry_name FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
        query.exec();
        query.first();
        if (!query.value(0).isNull())
        {
            popup->ui->label->hide();
            popup->ui->addPersonButton->hide();
            popup->ui->addOrgButton->hide();
            popup->ui->showCardButton->show();
            popup->ui->lblText->setText(tr("Входящий звонок от:<br><b>%1 (%2)</b>").arg(query.value(1).toString()).arg(number));
        }
        else
        {
            popup->ui->showCardButton->hide();
            popup->ui->label->show();
            popup->ui->addPersonButton->show();
            popup->ui->addOrgButton->show();
            popup->ui->lblText->setText(tr("Входящий звонок от:<br><b>Неизвестный (%1)</b>").arg(number));
        }
    }
}

void PopupWindow::recieveNumber(PopupWindow *popup, QString number)
{
    QVariant qv_popup = qVariantFromValue((void *)popup);
    QSqlDatabase db;
    QSqlQuery query(db);
    query.prepare("SELECT id FROM entry WHERE id IN (SELECT entry_id FROM fones WHERE fone = '" + number + "')");
    query.exec();
    query.first();

    if (!query.value(0).isNull())
    {
        popup->ui->label->hide();
        popup->ui->addPersonButton->hide();
        popup->ui->addOrgButton->hide();
    }
    else
        popup->ui->showCardButton->hide();

    connect(popup->ui->addPersonButton, SIGNAL(clicked(bool)), this, SLOT(onAddPerson()));
    popup->ui->addPersonButton->setProperty("number", number);
    popup->ui->addPersonButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->addOrgButton, SIGNAL(clicked(bool)), this, SLOT(onAddOrg()));
    popup->ui->addOrgButton->setProperty("number", number);
    popup->ui->addOrgButton->setProperty("qv_popup", qv_popup);
    connect(popup->ui->showCardButton, SIGNAL(clicked(bool)), this, SLOT(onShowCard()));
    popup->ui->showCardButton->setProperty("number", number);
    popup->ui->showCardButton->setProperty("qv_popup", qv_popup);
}
