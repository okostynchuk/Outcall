/********************************************************************************
** Form generated from reading UI file 'CallHistoryDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALLHISTORYDIALOG_H
#define UI_CALLHISTORYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CallHistoryDialog
{
public:
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBoxCalls;
    QVBoxLayout *verticalLayout_4;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_6;
    QTreeWidget *treeWidgetMissed;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_7;
    QTreeWidget *treeWidgetReceived;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_8;
    QTreeWidget *treeWidgetPlaced;
    QHBoxLayout *horizontalLayout;
    QPushButton *callButton;
    QPushButton *removeButton;
    QPushButton *addContactButton;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *closeButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *CallHistoryDialog)
    {
        if (CallHistoryDialog->objectName().isEmpty())
            CallHistoryDialog->setObjectName(QStringLiteral("CallHistoryDialog"));
        CallHistoryDialog->resize(600, 508);
        CallHistoryDialog->setMinimumSize(QSize(600, 500));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        CallHistoryDialog->setWindowIcon(icon);
        verticalLayout_5 = new QVBoxLayout(CallHistoryDialog);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBoxCalls = new QGroupBox(CallHistoryDialog);
        groupBoxCalls->setObjectName(QStringLiteral("groupBoxCalls"));
        verticalLayout_4 = new QVBoxLayout(groupBoxCalls);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        tabWidget = new QTabWidget(groupBoxCalls);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_6 = new QVBoxLayout(tab);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        treeWidgetMissed = new QTreeWidget(tab);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(3, QStringLiteral("Date & Time"));
        __qtreewidgetitem->setText(2, QStringLiteral("To"));
        __qtreewidgetitem->setText(1, QStringLiteral("Caller Number"));
        treeWidgetMissed->setHeaderItem(__qtreewidgetitem);
        treeWidgetMissed->setObjectName(QStringLiteral("treeWidgetMissed"));
        treeWidgetMissed->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeWidgetMissed->setRootIsDecorated(false);
        treeWidgetMissed->setColumnCount(4);

        verticalLayout_6->addWidget(treeWidgetMissed);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_7 = new QVBoxLayout(tab_2);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        treeWidgetReceived = new QTreeWidget(tab_2);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(3, QStringLiteral("Date & Time"));
        __qtreewidgetitem1->setText(2, QStringLiteral("To"));
        __qtreewidgetitem1->setText(1, QStringLiteral("Caller Number"));
        treeWidgetReceived->setHeaderItem(__qtreewidgetitem1);
        treeWidgetReceived->setObjectName(QStringLiteral("treeWidgetReceived"));
        treeWidgetReceived->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeWidgetReceived->setRootIsDecorated(false);
        treeWidgetReceived->setColumnCount(4);

        verticalLayout_7->addWidget(treeWidgetReceived);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_8 = new QVBoxLayout(tab_3);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        treeWidgetPlaced = new QTreeWidget(tab_3);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(2, QStringLiteral("Date & Time"));
        __qtreewidgetitem2->setText(1, QStringLiteral("To"));
        __qtreewidgetitem2->setText(0, QStringLiteral("From"));
        treeWidgetPlaced->setHeaderItem(__qtreewidgetitem2);
        treeWidgetPlaced->setObjectName(QStringLiteral("treeWidgetPlaced"));
        treeWidgetPlaced->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeWidgetPlaced->setRootIsDecorated(false);
        treeWidgetPlaced->setColumnCount(3);

        verticalLayout_8->addWidget(treeWidgetPlaced);

        tabWidget->addTab(tab_3, QString());

        verticalLayout_4->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        callButton = new QPushButton(groupBoxCalls);
        callButton->setObjectName(QStringLiteral("callButton"));

        horizontalLayout->addWidget(callButton);

        removeButton = new QPushButton(groupBoxCalls);
        removeButton->setObjectName(QStringLiteral("removeButton"));

        horizontalLayout->addWidget(removeButton);

        addContactButton = new QPushButton(groupBoxCalls);
        addContactButton->setObjectName(QStringLiteral("addContactButton"));

        horizontalLayout->addWidget(addContactButton);


        verticalLayout_4->addLayout(horizontalLayout);


        verticalLayout_5->addWidget(groupBoxCalls);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(128, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        closeButton = new QPushButton(CallHistoryDialog);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        horizontalLayout_3->addWidget(closeButton);

        horizontalSpacer_2 = new QSpacerItem(128, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_3);


        retranslateUi(CallHistoryDialog);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(CallHistoryDialog);
    } // setupUi

    void retranslateUi(QDialog *CallHistoryDialog)
    {
        CallHistoryDialog->setWindowTitle(QApplication::translate("CallHistoryDialog", "OutCALL - Call History", 0));
        groupBoxCalls->setTitle(QApplication::translate("CallHistoryDialog", " Calls", 0));
        QTreeWidgetItem *___qtreewidgetitem = treeWidgetMissed->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("CallHistoryDialog", "Caller Name", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("CallHistoryDialog", "Missed", 0));
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidgetReceived->headerItem();
        ___qtreewidgetitem1->setText(0, QApplication::translate("CallHistoryDialog", "Caller Name", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("CallHistoryDialog", "Received", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("CallHistoryDialog", "Placed", 0));
        callButton->setText(QApplication::translate("CallHistoryDialog", "Call", 0));
        removeButton->setText(QApplication::translate("CallHistoryDialog", "Remove selection", 0));
        addContactButton->setText(QApplication::translate("CallHistoryDialog", "Add Outlok Contact", 0));
        closeButton->setText(QApplication::translate("CallHistoryDialog", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class CallHistoryDialog: public Ui_CallHistoryDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALLHISTORYDIALOG_H
