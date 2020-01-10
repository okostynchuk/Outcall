/********************************************************************************
** Form generated from reading UI file 'SpeedDialDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPEEDDIALDIALOG_H
#define UI_SPEEDDIALDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SpeedDialDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *codeLabel;
    QComboBox *codeBox;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_2;
    QLabel *phoneLabel;
    QLineEdit *phoneEdit;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SpeedDialDialog)
    {
        if (SpeedDialDialog->objectName().isEmpty())
            SpeedDialDialog->setObjectName(QStringLiteral("SpeedDialDialog"));
        SpeedDialDialog->resize(357, 110);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SpeedDialDialog->sizePolicy().hasHeightForWidth());
        SpeedDialDialog->setSizePolicy(sizePolicy);
        SpeedDialDialog->setMinimumSize(QSize(350, 110));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        SpeedDialDialog->setWindowIcon(icon);
        verticalLayout_3 = new QVBoxLayout(SpeedDialDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        codeLabel = new QLabel(SpeedDialDialog);
        codeLabel->setObjectName(QStringLiteral("codeLabel"));

        verticalLayout->addWidget(codeLabel);

        codeBox = new QComboBox(SpeedDialDialog);
        codeBox->setObjectName(QStringLiteral("codeBox"));

        verticalLayout->addWidget(codeBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        phoneLabel = new QLabel(SpeedDialDialog);
        phoneLabel->setObjectName(QStringLiteral("phoneLabel"));

        verticalLayout_2->addWidget(phoneLabel);

        phoneEdit = new QLineEdit(SpeedDialDialog);
        phoneEdit->setObjectName(QStringLiteral("phoneEdit"));

        verticalLayout_2->addWidget(phoneEdit);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(SpeedDialDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buttonBox->sizePolicy().hasHeightForWidth());
        buttonBox->setSizePolicy(sizePolicy1);
        buttonBox->setLayoutDirection(Qt::LeftToRight);
        buttonBox->setAutoFillBackground(false);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_3->addWidget(buttonBox);


        retranslateUi(SpeedDialDialog);

        QMetaObject::connectSlotsByName(SpeedDialDialog);
    } // setupUi

    void retranslateUi(QDialog *SpeedDialDialog)
    {
        SpeedDialDialog->setWindowTitle(QApplication::translate("SpeedDialDialog", "Speed Dial", 0));
        codeLabel->setText(QApplication::translate("SpeedDialDialog", "Code:", 0));
        phoneLabel->setText(QApplication::translate("SpeedDialDialog", "Phone:", 0));
    } // retranslateUi

};

namespace Ui {
    class SpeedDialDialog: public Ui_SpeedDialDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPEEDDIALDIALOG_H
