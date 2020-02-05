#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include "Global.h"
#include "CallHistoryDialog.h"
#include "OutCALL.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "EditContactDialog.h"
#include "EditOrgContactDialog.h"
#include "ViewContactDialog.h"
#include "ViewOrgContactDialog.h"

#include <QDialog>
#include <QTimer>
#include <QTextEdit>
#include <QMap>

class CallHistoryDialog;
class ContactsDialog;

namespace Ui {
    class PopupWindow;
}

class PopupWindow : public QDialog
{
    Q_OBJECT

public:
    enum PWType
    {
        PWPhoneCall,
        PWInformationMessage
	};

public slots:
    void recieveNumber(PopupWindow*, QString);
    void recieveData(bool);

signals:
    void sendSignal(bool);

private:
    /**
     * @brief The PWInformation struct / popup window information
     */
    struct PWInformation
    {
		PWType type;
		QString text;
		QPixmap avatar;
		QString extension;
	};

public:
	PopupWindow(const PWInformation& pwi, QWidget *parent = 0);
    ~PopupWindow();
    static void showCallNotification(QString number, QString caller);
    static void showInformationMessage(QString caption, QString message, QPixmap avatar=QPixmap(), PWType type = PWInformationMessage);
    static void closeAll();
    void onSave();

protected:
    void changeEvent(QEvent *e);

protected slots:
    void onCallDeteceted(const QMap<QString, QVariant> &call, AsteriskManager::CallState state);
    void onCallReceived(const QMap<QString, QVariant> &call);
    QString getUpdateId(QString &);

private slots:
    void onTimer();
    void onPopupTimeout();
    void on_pushButton_close_clicked();
    void onAddPerson();
    void onAddOrg();
    void onEdit();
    void onShowCard();

private:
    void startPopupWaitingTimer();
    void closeAndDestroy();

private:
    Ui::PopupWindow *ui;
    QTextEdit *note;
	int m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
	int m_nCurrentPosX, m_nCurrentPosY;
	int m_nIncrement; // px
	bool m_bAppearing;
	QTimer m_timer;
	PWInformation m_pwi;
	static QList<PopupWindow*> m_PopupWindows;
	static int m_nLastWindowPosition;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
    CallHistoryDialog *m_callHistoryDialog;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    ViewContactDialog *viewContactDialog;
    ViewOrgContactDialog *viewOrgContactDialog;
};

#endif // POPUPWINDOW_H
