#ifndef POPUPREMINDER_H
#define POPUPREMINDER_H

#include "RemindersDialog.h"
#include "EditReminderDialog.h"
#include "AsteriskManager.h"
#include "Global.h"
#include "ChooseNumber.h"

#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include <QComboBox>
#include <QPointer>

namespace Ui {
class PopupReminder;
}

class PopupReminder : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool);

private:
    struct PopupReminderInfo
    {
        QString text;
        RemindersDialog* remindersDialog;
        QString my_number;
        QString name;
        QString number;
        QStringList numbers;
        QString id;
        QString group_id;
        QDateTime dateTime;
        QString note;
        QString call_id;
        bool active;
    };

public:
    PopupReminder(PopupReminderInfo& pri, QWidget *parent = 0);
    ~PopupReminder();

    static void showReminder(RemindersDialog*, QString, QString, QDateTime, QString);
    static void closeAll();

private slots:
    void closeAndDestroy();
    void onOpenAccess();
    void onCall();
    void onTimer();
    void onClosePopup();
    void onSelectTime(int);

    bool isInternalPhone(QString* str);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PopupReminder *ui;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<EditReminderDialog> editReminderDialog;
    QPointer<ChooseNumber> chooseNumber;

    int m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
    int m_nCurrentPosX, m_nCurrentPosY;
    int m_nIncrement;

    bool m_bAppearing;

    QPoint position;

    QTimer m_timer;

    PopupReminderInfo m_pri;

    static QList<PopupReminder*> m_PopupReminders;
};

#endif // POPUPREMINDER_H
