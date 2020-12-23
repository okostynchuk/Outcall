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
#include <QPointer>

namespace Ui {
class PopupReminder;
}

class PopupReminder : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool update);

private:
    struct PopupReminderInfo
    {
        RemindersDialog* remindersDialog;

        QString text;
        QString name;
        QString number;
        QStringList numbers;
        QString id;
        QString group_id;
        QString note;
        QString call_id;

        QDateTime dateTime;

        bool active;
    };

public:
    PopupReminder(const PopupReminderInfo& pri, QWidget* parent = 0);
    ~PopupReminder();

    static void showReminder(RemindersDialog* remindersDialog, const QString& id, const QDateTime& dateTime, const QString& note);
    static void closeAll();

private slots:
    void closeAndDestroy();
    void onOpenAccess();
    void onCall();
    void onTimer();
    void onClosePopup();
    void onSelectTime(qint32 index);

    bool isInternalPhone(QString* str);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PopupReminder* ui;

    QSqlDatabase m_db;
    QSqlDatabase m_dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<EditReminderDialog> m_editReminderDialog;
    QPointer<ChooseNumber> m_chooseNumber;

    qint32 m_startPosX;
    qint32 m_startPosY;
    qint32 m_taskbarPlacement;
    qint32 m_currentPosX;
    qint32 m_currentPosY;
    qint32 m_increment;

    bool m_appearing;

    QPoint m_position;

    QTimer m_timer;

    PopupReminderInfo m_pri;

    static QList<PopupReminder*> s_popupReminders;
};

#endif // POPUPREMINDER_H
