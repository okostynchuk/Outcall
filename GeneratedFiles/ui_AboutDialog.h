/********************************************************************************
** Form generated from reading UI file 'AboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QOutCallLabel.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QLabel *label;
    QOutCallLabel *lblAvatar;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QPushButton *pushButton;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QStringLiteral("AboutDialog"));
        AboutDialog->resize(350, 115);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AboutDialog->sizePolicy().hasHeightForWidth());
        AboutDialog->setSizePolicy(sizePolicy);
        AboutDialog->setMinimumSize(QSize(350, 115));
        AboutDialog->setMaximumSize(QSize(350, 115));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        AboutDialog->setWindowIcon(icon);
        label = new QLabel(AboutDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(180, 10, 161, 39));
        lblAvatar = new QOutCallLabel(AboutDialog);
        lblAvatar->setObjectName(QStringLiteral("lblAvatar"));
        lblAvatar->setGeometry(QRect(10, 10, 161, 71));
        sizePolicy.setHeightForWidth(lblAvatar->sizePolicy().hasHeightForWidth());
        lblAvatar->setSizePolicy(sizePolicy);
        lblAvatar->setMinimumSize(QSize(32, 32));
        lblAvatar->setCursor(QCursor(Qt::PointingHandCursor));
        lblAvatar->setStyleSheet(QStringLiteral("/*background-color: rgb(255, 255, 255);*/"));
        lblAvatar->setPixmap(QPixmap(QString::fromUtf8(":/images/bicom-logo.png")));
        lblAvatar->setScaledContents(true);
        lblAvatar->setOpenExternalLinks(true);
        layoutWidget = new QWidget(AboutDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 80, 331, 31));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setOpenExternalLinks(true);

        horizontalLayout->addWidget(label_3);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton);


        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "About OutCALL", 0));
        label->setText(QApplication::translate("AboutDialog", "<html><head/><body><p><span style=\" font-size:9pt; font-weight:600;\">OutCALL, Version 2.0</span></p><p>Built on Apr 15 2015</p></body></html>", 0));
#ifndef QT_NO_TOOLTIP
        lblAvatar->setToolTip(QApplication::translate("AboutDialog", "Chat History", 0));
#endif // QT_NO_TOOLTIP
        lblAvatar->setText(QString());
        label_3->setText(QApplication::translate("AboutDialog", "<html><head/><body><p><a href=\"http://www.bicomsystems.com/\"><span style=\" text-decoration: underline; color:#0000ff;\">Copyright (c) Bicom Systems Ltd. 2003-2015</span></a></p></body></html>", 0));
        pushButton->setText(QApplication::translate("AboutDialog", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
