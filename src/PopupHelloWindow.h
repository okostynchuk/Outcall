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

    qint32 m_startPosX;
    qint32 m_startPosY;
    qint32 m_taskbarPlacement;
    qint32 m_currentPosX;
    qint32 m_currentPosY;
    qint32 m_increment;

    bool m_appearing;

    QTimer m_timer;

    PopupHelloWindowInfo m_phwi;

    static QList<PopupHelloWindow*> s_popupHelloWindows;
};

#endif // POPUPWINDOW_H
