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

    /**
     * @brief The PWInformation struct / popup window information
     */
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

protected:
    void changeEvent(QEvent *e);

private slots:
    void onTimer();
    void onPopupTimeout();
    virtual void mousePressEvent(QMouseEvent *evet);

private:
    void startPopupWaitingTimer();
    void closeAndDestroy();

private:
    Ui::PopupHelloWindow *ui;

    int m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
    int m_nCurrentPosX, m_nCurrentPosY;
    int m_nIncrement;
    bool m_bAppearing;

    QTimer m_timer;
    PWInformation m_pwi;

    static QList<PopupHelloWindow*> m_PopupHelloWindows;
};

#endif // POPUPWINDOW_H
