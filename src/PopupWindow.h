#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include "Global.h"
#include "AsteriskManager.h"
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
#include <QTextCursor>

namespace Ui {
    class PopupWindow;
}

class PopupWindow : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool update);

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
    PopupWindow(const PWInformation& pwi, QWidget* parent = 0);
    ~PopupWindow();

    static void showCall(const QString& dateTime, const QString& uniqueid, const QString& number, const QString& caller, const QString& my_number);
    static void closeAll();

private slots:
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
    void onCallStart(const QString& uniqueid);
    void onCursorPosChanged();

    void on_closeButton_clicked();

    bool isInternalPhone(QString* str);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

    bool eventFilter(QObject*, QEvent* event);

private:
    Ui::PopupWindow* ui;

    QSqlDatabase db;

    QPointer<AddContactDialog> addContactDialog;
    QPointer<AddOrgContactDialog> addOrgContactDialog;
    QPointer<AddPhoneNumberToContactDialog> addPhoneNumberToContactDialog;
    QPointer<ViewContactDialog> viewContactDialog;
    QPointer<ViewOrgContactDialog> viewOrgContactDialog;
    QPointer<AddReminderDialog> addReminderDialog;
    QPointer<NotesDialog> notesDialog;

    QPoint position;

    QTextCursor textCursor;

    QStringList extensions = g_pAsteriskManager->extensionNumbers.values();;

    QString userId;
    QString author;

	qint32 m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
	qint32 m_nCurrentPosX, m_nCurrentPosY;
    qint32 m_nIncrement;

	bool m_bAppearing;

	QTimer m_timer;

	PWInformation m_pwi;

    static QList<PopupWindow*> m_PopupWindows;
};

#endif // POPUPWINDOW_H
