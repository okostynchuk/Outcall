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
#include <QPointer>

namespace Ui {
    class PopupWindow;
}

class PopupWindow : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool);

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
    void controlPopup();
    void startPopupWaitingTimer();
    void closeAndDestroy();
    void onPopupTimeout();
    void onTimer();
    void onAddPerson();
    void onAddOrg();
    void onAddPhoneNumberToContact();
    void onShowCard();
    void onSaveNote();
    void onTextChanged();
    void onOpenAccess();
    void onAddReminder();
    void onViewNotes();
    void onCallStart(QString);

    void on_closeButton_clicked();

    bool isInternalPhone(QString* str);

    void changeEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PopupWindow *ui;

    QSqlDatabase db;

    QPointer<AddContactDialog> addContactDialog;
    QPointer<AddOrgContactDialog> addOrgContactDialog;
    QPointer<AddPhoneNumberToContactDialog> addPhoneNumberToContactDialog;
    QPointer<ViewContactDialog> viewContactDialog;
    QPointer<ViewOrgContactDialog> viewOrgContactDialog;
    QPointer<AddReminderDialog> addReminderDialog;
    QPointer<NotesDialog> notesDialog;

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
