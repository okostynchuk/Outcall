/********************************************************************************
** Form generated from reading UI file 'PopupWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POPUPWINDOW_H
#define UI_POPUPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_PopupWindow
{
public:
    QLabel *lblBackground;
    QLabel *lblText;
    QLabel *lblAvatar;

    void setupUi(QDialog *PopupWindow)
    {
        if (PopupWindow->objectName().isEmpty())
            PopupWindow->setObjectName(QStringLiteral("PopupWindow"));
        PopupWindow->resize(236, 71);
        lblBackground = new QLabel(PopupWindow);
        lblBackground->setObjectName(QStringLiteral("lblBackground"));
        lblBackground->setEnabled(true);
        lblBackground->setGeometry(QRect(0, 0, 241, 71));
        lblBackground->setStyleSheet(QLatin1String("#lblBackground {\n"
"background-color: rgba(0, 0, 0, 150);\n"
"border-radius: 15px;\n"
"}"));
        lblBackground->setScaledContents(true);
        lblText = new QLabel(PopupWindow);
        lblText->setObjectName(QStringLiteral("lblText"));
        lblText->setGeometry(QRect(60, 6, 171, 39));
        lblText->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);"));
        lblText->setTextFormat(Qt::RichText);
        lblText->setAlignment(Qt::AlignCenter);
        lblText->setWordWrap(true);
        lblAvatar = new QLabel(PopupWindow);
        lblAvatar->setObjectName(QStringLiteral("lblAvatar"));
        lblAvatar->setGeometry(QRect(20, 10, 32, 32));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblAvatar->sizePolicy().hasHeightForWidth());
        lblAvatar->setSizePolicy(sizePolicy);
        lblAvatar->setMinimumSize(QSize(32, 32));
        lblAvatar->setMaximumSize(QSize(24, 24));
        lblAvatar->setCursor(QCursor(Qt::PointingHandCursor));
        lblAvatar->setStyleSheet(QStringLiteral("/*background-color: rgb(255, 255, 255);*/"));
        lblAvatar->setPixmap(QPixmap(QString::fromUtf8(":/images/outcall-logo.png")));
        lblAvatar->setScaledContents(true);

        retranslateUi(PopupWindow);

        QMetaObject::connectSlotsByName(PopupWindow);
    } // setupUi

    void retranslateUi(QDialog *PopupWindow)
    {
        PopupWindow->setWindowTitle(QString());
        lblBackground->setText(QString());
        lblText->setText(QApplication::translate("PopupWindow", "Входящий звонок от: <b>Денис Комадарик (1031)</b>", 0));
#ifndef QT_NO_TOOLTIP
        lblAvatar->setToolTip(QApplication::translate("PopupWindow", "Chat History", 0));
#endif // QT_NO_TOOLTIP
        lblAvatar->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PopupWindow: public Ui_PopupWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POPUPWINDOW_H
