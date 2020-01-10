/********************************************************************************
** Form generated from reading UI file 'PlaceCallDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLACECALLDIALOG_H
#define UI_PLACECALLDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <searchbox.h>

QT_BEGIN_NAMESPACE

class Ui_PlaceCallDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QVBoxLayout *verticalLayout;
    QComboBox *fromBox;
    QHBoxLayout *horizontalLayout;
    SearchBox *searchLine;
    QComboBox *contactBox;
    QLineEdit *phoneLine;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *callButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *PlaceCallDialog)
    {
        if (PlaceCallDialog->objectName().isEmpty())
            PlaceCallDialog->setObjectName(QStringLiteral("PlaceCallDialog"));
        PlaceCallDialog->resize(397, 338);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PlaceCallDialog->sizePolicy().hasHeightForWidth());
        PlaceCallDialog->setSizePolicy(sizePolicy);
        PlaceCallDialog->setMinimumSize(QSize(350, 300));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        PlaceCallDialog->setWindowIcon(icon);
        verticalLayout_3 = new QVBoxLayout(PlaceCallDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(PlaceCallDialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(PlaceCallDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_2->addWidget(label_2);

        label_3 = new QLabel(PlaceCallDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_2->addWidget(label_3);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        fromBox = new QComboBox(PlaceCallDialog);
        fromBox->setObjectName(QStringLiteral("fromBox"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(fromBox->sizePolicy().hasHeightForWidth());
        fromBox->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(fromBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        searchLine = new SearchBox(PlaceCallDialog);
        searchLine->setObjectName(QStringLiteral("searchLine"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(searchLine->sizePolicy().hasHeightForWidth());
        searchLine->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(searchLine);

        contactBox = new QComboBox(PlaceCallDialog);
        contactBox->setObjectName(QStringLiteral("contactBox"));

        horizontalLayout->addWidget(contactBox);


        verticalLayout->addLayout(horizontalLayout);

        phoneLine = new QLineEdit(PlaceCallDialog);
        phoneLine->setObjectName(QStringLiteral("phoneLine"));

        verticalLayout->addWidget(phoneLine);


        horizontalLayout_2->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout_2);

        treeWidget = new QTreeWidget(PlaceCallDialog);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setRootIsDecorated(false);
        treeWidget->setColumnCount(2);
        treeWidget->header()->setStretchLastSection(true);

        verticalLayout_3->addWidget(treeWidget);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        callButton = new QPushButton(PlaceCallDialog);
        callButton->setObjectName(QStringLiteral("callButton"));

        horizontalLayout_3->addWidget(callButton);

        cancelButton = new QPushButton(PlaceCallDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        horizontalLayout_3->addWidget(cancelButton);


        verticalLayout_3->addLayout(horizontalLayout_3);


        retranslateUi(PlaceCallDialog);

        QMetaObject::connectSlotsByName(PlaceCallDialog);
    } // setupUi

    void retranslateUi(QDialog *PlaceCallDialog)
    {
        PlaceCallDialog->setWindowTitle(QApplication::translate("PlaceCallDialog", "OutCALL - Place a Call", 0));
        label->setText(QApplication::translate("PlaceCallDialog", "From:", 0));
        label_2->setText(QApplication::translate("PlaceCallDialog", "Contact:", 0));
        label_3->setText(QApplication::translate("PlaceCallDialog", "Phone:", 0));
        searchLine->setPlaceholderText(QApplication::translate("PlaceCallDialog", "Search", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("PlaceCallDialog", "Type", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("PlaceCallDialog", "Numbers", 0));
        callButton->setText(QApplication::translate("PlaceCallDialog", "Call", 0));
        cancelButton->setText(QApplication::translate("PlaceCallDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class PlaceCallDialog: public Ui_PlaceCallDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLACECALLDIALOG_H
