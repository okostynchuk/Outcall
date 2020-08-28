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
    void reminders(bool);

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
    PopupNotification(PopupNotificationInfo& pni, QWidget *parent = 0);
    ~PopupNotification();

    static void showNotification(RemindersDialog*, QString, QString, QString);
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
    Ui::PopupNotification *ui;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    int m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
    int m_nCurrentPosX, m_nCurrentPosY;
    int m_nIncrement;

    bool m_bAppearing;

    QPoint position;

    QTimer m_timer;

    PopupNotificationInfo m_pni;

    static QList<PopupNotification*> m_PopupNotifications;
};

#endif // POPUPNOTIFICATION_H
