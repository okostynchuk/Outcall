#ifndef POPUPNOTIFICATION_H
#define POPUPNOTIFICATION_H

#include "RemindersDialog.h"

#include <QDialog>
#include <QTimer>

namespace Ui {
class PopupNotification;
}

class PopupNotification : public QDialog
{
    Q_OBJECT

signals:
    void reminders(bool change);

private:
    struct PopupNotificationInfo
    {
        RemindersDialog* remindersDialog;
        QString id;
        QString number;
        QString note;
        QString text;
    };

public:
    PopupNotification(const PopupNotificationInfo& pni, QWidget* parent = 0);
    ~PopupNotification();

    static void showReminderNotification(RemindersDialog* remindersDialog, const QString& id, const QString& number, const QString& note);
    static void closeAll();

private slots:
    void closeAndDestroy();
    void onTimer();
    void onClosePopup();

    void on_pushButton_close_clicked();

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PopupNotification* ui;

    QSqlDatabase db;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    qint32 m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
    qint32 m_nCurrentPosX, m_nCurrentPosY;
    qint32 m_nIncrement;

    bool m_bAppearing;

    QPoint position;

    QTimer m_timer;

    PopupNotificationInfo m_pni;

    static QList<PopupNotification*> m_PopupNotifications;
};

#endif // POPUPNOTIFICATION_H
