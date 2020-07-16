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

namespace Ui {
class PopupReminder;
}

class PopupReminder : public QDialog
{
    Q_OBJECT

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
        QDateTime dateTime;
        QString note;
        QString call_id;
        bool active;
    };

    void closeAndDestroy();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void receiveData(bool);
    void receiveNumber(QString &);

public:
    PopupReminder(PopupReminderInfo& pri, QWidget *parent = 0);
    ~PopupReminder();
    static void showReminder(RemindersDialog*, QString, QString, QDateTime, QString);
    static void closeAll();

private slots:
    bool isInnerPhone(QString *str);
    void onOpenAccess();
    void onCall();
    void onTimer();
    void onClosePopup();
    void onSelectTime();
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PopupReminder *ui;

    EditReminderDialog *editReminderDialog;
    ChooseNumber *chooseNumber;

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
