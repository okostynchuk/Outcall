#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include "Global.h"
#include "AsteriskManager.h"
#include "Outcall.h"
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

private:
    struct PopupWindowInfo
    {
        QString text;
        QString uniqueid;
        QString number;

        QPixmap avatar;

        bool stopTimer = false;
    };

public:
    PopupWindow(const PopupWindowInfo& pwi, QWidget* parent = 0);
    ~PopupWindow();

    static void showCall(const QString& dateTime, const QString& uniqueid, const QString& number, const QString& caller);
    static void closeAll();

private slots:
    void startPopupWaitingTimer();
    void closeAndDestroy();
    void onPopupTimeout();
    void onTimer();

    void onAddPerson();
    void onAddOrg();
    void onAddPhoneNumberToContact();
    void onAddReminder();
    void onShowCard();
    void onSaveNote();
    void onOpenAccess();
    void onViewNotes();

    void onTextChanged();     
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

    QSqlDatabase m_db;

    QPointer<AddContactDialog> m_addContactDialog;
    QPointer<AddOrgContactDialog> m_addOrgContactDialog;
    QPointer<AddPhoneNumberToContactDialog> m_addPhoneNumberToContactDialog;
    QPointer<ViewContactDialog> m_viewContactDialog;
    QPointer<ViewOrgContactDialog> m_viewOrgContactDialog;
    QPointer<AddReminderDialog> m_addReminderDialog;
    QPointer<NotesDialog> m_notesDialog;

    QPoint m_position;

    QTextCursor m_textCursor;

    qint32 m_startPosX;
    qint32 m_startPosY;
    qint32 m_taskbarPlacement;
    qint32 m_currentPosX;
    qint32 m_currentPosY;
    qint32 m_increment;

	bool m_appearing;

    QTimer m_timer;

    PopupWindowInfo m_pwi;

    static QList<PopupWindow*> s_popupWindows;
};

#endif // POPUPWINDOW_H
