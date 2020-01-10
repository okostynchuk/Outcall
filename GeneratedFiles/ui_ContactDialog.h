/********************************************************************************
** Form generated from reading UI file 'ContactDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTACTDIALOG_H
#define UI_CONTACTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ContactDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *contactName;
    QHBoxLayout *horizontalLayout_2;
    QLabel *fromLabel;
    QComboBox *extensionBox;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *callButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *ContactDialog)
    {
        if (ContactDialog->objectName().isEmpty())
            ContactDialog->setObjectName(QStringLiteral("ContactDialog"));
        ContactDialog->resize(372, 334);
        ContactDialog->setMinimumSize(QSize(300, 300));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        ContactDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(ContactDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        contactName = new QLabel(ContactDialog);
        contactName->setObjectName(QStringLiteral("contactName"));
        QFont font;
        font.setPointSize(14);
        contactName->setFont(font);
        contactName->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(contactName);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        fromLabel = new QLabel(ContactDialog);
        fromLabel->setObjectName(QStringLiteral("fromLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fromLabel->sizePolicy().hasHeightForWidth());
        fromLabel->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(fromLabel);

        extensionBox = new QComboBox(ContactDialog);
        extensionBox->setObjectName(QStringLiteral("extensionBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(extensionBox->sizePolicy().hasHeightForWidth());
        extensionBox->setSizePolicy(sizePolicy1);
        extensionBox->setMinimumSize(QSize(200, 0));

        horizontalLayout_2->addWidget(extensionBox);


        verticalLayout->addLayout(horizontalLayout_2);

        treeWidget = new QTreeWidget(ContactDialog);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setTextElideMode(Qt::ElideRight);
        treeWidget->setRootIsDecorated(false);
        treeWidget->setColumnCount(2);
        treeWidget->header()->setHighlightSections(false);

        verticalLayout->addWidget(treeWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        callButton = new QPushButton(ContactDialog);
        callButton->setObjectName(QStringLiteral("callButton"));

        horizontalLayout->addWidget(callButton);

        cancelButton = new QPushButton(ContactDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout->addWidget(cancelButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ContactDialog);

        QMetaObject::connectSlotsByName(ContactDialog);
    } // setupUi

    void retranslateUi(QDialog *ContactDialog)
    {
        ContactDialog->setWindowTitle(QApplication::translate("ContactDialog", "Call", 0));
        contactName->setText(QApplication::translate("ContactDialog", "My Contact", 0));
        fromLabel->setText(QApplication::translate("ContactDialog", "From:", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("ContactDialog", "Type", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("ContactDialog", "Number", 0));
        callButton->setText(QApplication::translate("ContactDialog", "Call", 0));
        cancelButton->setText(QApplication::translate("ContactDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class ContactDialog: public Ui_ContactDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTACTDIALOG_H
