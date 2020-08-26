#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include "Global.h"
#include "OutCALL.h"
#include "CallHistoryDialog.h"
#include "PlaceCallDialog.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "AddPhoneNumberToContactDialog.h"
#include "ViewContactDialog.h"
#include "ViewOrgContactDialog.h"
#include "SettingsDialog.h"
#include "AddReminderDialog.h"
#include "NotesDialog.h"

#include <QDialog>
#include <QTimer>
#include <QTextEdit>
#include <QMap>
#include <QSqlQuery>

namespace Ui {
    class PopupWindow;
}

class PopupWindow : public QDialog
{
    Q_OBJECT

signals:
    void sendSignal(bool);

public slots:
    void receiveNumber(PopupWindow*);
    void receiveData(bool);
    void timerStop(QString);

public:
    enum PWType
    {
        PWPhoneCall,
        PWInformationMessage
    };

private:
    struct PWInformation
    {
        PWType type;
        QString text;
        QPixmap avatar;
        QString uniqueid;
        QString number;
        QString my_number;
        bool stopTimer = false;
    };

public:
    PopupWindow(PWInformation& pwi, QWidget *parent = 0);
    ~PopupWindow();

    static void showCallNotification(QString dateTime, QString uniqueid, QString number, QString caller, QString my_number);
    static void showInformationMessage(QString caption, QString message, QPixmap avatar=QPixmap(), PWType type = PWInformationMessage);
    static void closeAll();

private slots:
    void startPopupWaitingTimer();
    void closeAndDestroy();
    void onPopupTimeout();
    void onTimer();
    void on_pushButton_close_clicked();
    void onAddPerson();
    void onAddOrg();
    void onAddPhoneNumberToContact();
    void onShowCard();
    void onSaveNote();
    void onTextChanged();
    void onOpenAccess();
    void onAddReminder();
    void onViewNotes();

    bool isInnerPhone(QString* str);

    void changeEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PopupWindow *ui;

    AddContactDialog* addContactDialog;
    AddOrgContactDialog* addOrgContactDialog;
    AddPhoneNumberToContactDialog* addPhoneNumberToContactDialog;
    ViewContactDialog* viewContactDialog;
    ViewOrgContactDialog* viewOrgContactDialog;
    AddReminderDialog* addReminderDialog;
    NotesDialog* notesDialog;

    QPoint position;
    QString userID;
    QString author;
	int m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
	int m_nCurrentPosX, m_nCurrentPosY;
    int m_nIncrement;
	bool m_bAppearing;
	QTimer m_timer;

	PWInformation m_pwi;

    static QList<PopupWindow*> m_PopupWindows;
};

#endif // POPUPWINDOW_H
