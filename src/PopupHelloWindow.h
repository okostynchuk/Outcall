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

private:
    struct PopupHelloWindowInfo
    {
        QString text;
        QString extension;

        QPixmap avatar;
    };

public:
    PopupHelloWindow(const PopupHelloWindowInfo& phwi, QWidget* parent = 0);
    ~PopupHelloWindow();

    static void showInformationMessage(const QString& caption, const QString& message, QPixmap avatar = QPixmap());
    static void closeAll();

private slots:
    void startPopupWaitingTimer();
    void closeAndDestroy();
    void onTimer();
    void onPopupTimeout();

    void mousePressEvent(QMouseEvent*);

private:
    Ui::PopupHelloWindow* ui;

    qint32 m_nStartPosX, m_nStartPosY, m_nTaskbarPlacement;
    qint32 m_nCurrentPosX, m_nCurrentPosY;
    qint32 m_nIncrement;

    bool m_bAppearing;

    QTimer m_timer;

    PopupHelloWindowInfo m_phwi;

    static QList<PopupHelloWindow*> m_PopupHelloWindows;
};

#endif // POPUPWINDOW_H
