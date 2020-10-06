#ifndef POPUPHELLOWINDOW_H
#define POPUPHELLOWINDOW_H

#include "Global.h"

#include <QDialog>
#include <QTimer>

namespace Ui {
    class PopupHelloWindow;
}

class PopupHelloWindow : public QDialog
{
    Q_OBJECT

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
        QString extension;
    };

public:
    PopupHelloWindow(const PWInformation& pwi, QWidget *parent = 0);
    ~PopupHelloWindow();

    static void showInformationMessage(QString caption, QString message, QPixmap avatar=QPixmap(), PWType type = PWInformationMessage);
    static void closeAll();

private slots:
    void startPopupWaitingTimer();
    void closeAndDestroy();
    void onTimer();
    void onPopupTimeout();

    void mousePressEvent(QMouseEvent*);

private:
    Ui::PopupHelloWindow *ui;

    qint32 m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
    qint32 m_nCurrentPosX, m_nCurrentPosY;
    qint32 m_nIncrement;

    bool m_bAppearing;

    QTimer m_timer;

    PWInformation m_pwi;

    static QList<PopupHelloWindow*> m_PopupHelloWindows;
};

#endif // POPUPWINDOW_H
