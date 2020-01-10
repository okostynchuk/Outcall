/********************************************************************************
** Form generated from reading UI file 'SettingsDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout_4;
    QTabWidget *tabWidget;
    QWidget *general;
    QVBoxLayout *verticalLayout_12;
    QGroupBox *generalGroupBox;
    QVBoxLayout *verticalLayout_13;
    QVBoxLayout *verticalLayout_14;
    QCheckBox *autoSignIn;
    QCheckBox *autoStartBox;
    QCheckBox *callerIDBox;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *callerIDList;
    QSpacerItem *horizontalSpacer;
    QGroupBox *languageGroup;
    QGridLayout *gridLayout;
    QComboBox *languageList;
    QPushButton *languageButton;
    QLabel *languageLabel;
    QSpacerItem *verticalSpacer_2;
    QWidget *dialingTab;
    QVBoxLayout *verticalLayout_8;
    QTabWidget *dialRulesTabs;
    QWidget *dialGeneralTab;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_9;
    QLabel *label;
    QLineEdit *contextLine;
    QLabel *label_2;
    QLineEdit *prefixLine;
    QLabel *internationalLabel;
    QLineEdit *internationalLine;
    QWidget *dialReplaceTab;
    QVBoxLayout *verticalLayout_10;
    QTreeWidget *replaceTree;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *replaceDialAddButton;
    QPushButton *replaceDialRemoveButton;
    QWidget *dialSpeedTab;
    QVBoxLayout *verticalLayout_11;
    QTreeWidget *speedTree;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *speedAddButton;
    QPushButton *speedEditButton;
    QPushButton *speedRemoveButton;
    QWidget *server;
    QFormLayout *formLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QLabel *serverLabel;
    QLineEdit *serverName;
    QLabel *userLabel;
    QLineEdit *userName;
    QLabel *passLabel;
    QLineEdit *password;
    QLabel *portLabel;
    QLineEdit *port;
    QWidget *extensions;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QTreeWidget *treeWidget;
    QVBoxLayout *verticalLayout_5;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *editButton;
    QSpacerItem *verticalSpacer;
    QWidget *outlook;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupBox_3;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_7;
    QCheckBox *enableOutlookBox;
    QCheckBox *refreshBox;
    QCheckBox *displayContactCallBox;
    QCheckBox *displayContactUnknownBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *poweredLabel;
    QLabel *bicomLink;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QStringLiteral("SettingsDialog"));
        SettingsDialog->resize(536, 467);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingsDialog->sizePolicy().hasHeightForWidth());
        SettingsDialog->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        SettingsDialog->setWindowIcon(icon);
        verticalLayout_4 = new QVBoxLayout(SettingsDialog);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        tabWidget = new QTabWidget(SettingsDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        general = new QWidget();
        general->setObjectName(QStringLiteral("general"));
        verticalLayout_12 = new QVBoxLayout(general);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        generalGroupBox = new QGroupBox(general);
        generalGroupBox->setObjectName(QStringLiteral("generalGroupBox"));
        verticalLayout_13 = new QVBoxLayout(generalGroupBox);
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setObjectName(QStringLiteral("verticalLayout_14"));
        autoSignIn = new QCheckBox(generalGroupBox);
        autoSignIn->setObjectName(QStringLiteral("autoSignIn"));
        autoSignIn->setChecked(true);

        verticalLayout_14->addWidget(autoSignIn);

        autoStartBox = new QCheckBox(generalGroupBox);
        autoStartBox->setObjectName(QStringLiteral("autoStartBox"));
        autoStartBox->setChecked(true);

        verticalLayout_14->addWidget(autoStartBox);

        callerIDBox = new QCheckBox(generalGroupBox);
        callerIDBox->setObjectName(QStringLiteral("callerIDBox"));

        verticalLayout_14->addWidget(callerIDBox);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalSpacer_2 = new QSpacerItem(18, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        callerIDList = new QComboBox(generalGroupBox);
        callerIDList->setObjectName(QStringLiteral("callerIDList"));

        horizontalLayout_6->addWidget(callerIDList);

        horizontalSpacer = new QSpacerItem(328, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);


        verticalLayout_14->addLayout(horizontalLayout_6);


        verticalLayout_13->addLayout(verticalLayout_14);

        languageGroup = new QGroupBox(generalGroupBox);
        languageGroup->setObjectName(QStringLiteral("languageGroup"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(languageGroup->sizePolicy().hasHeightForWidth());
        languageGroup->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(languageGroup);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        languageList = new QComboBox(languageGroup);
        languageList->setObjectName(QStringLiteral("languageList"));

        gridLayout->addWidget(languageList, 1, 0, 1, 1);

        languageButton = new QPushButton(languageGroup);
        languageButton->setObjectName(QStringLiteral("languageButton"));

        gridLayout->addWidget(languageButton, 1, 1, 1, 1);

        languageLabel = new QLabel(languageGroup);
        languageLabel->setObjectName(QStringLiteral("languageLabel"));

        gridLayout->addWidget(languageLabel, 1, 2, 1, 1);


        verticalLayout_13->addWidget(languageGroup);

        verticalSpacer_2 = new QSpacerItem(20, 166, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_13->addItem(verticalSpacer_2);


        verticalLayout_12->addWidget(generalGroupBox);

        tabWidget->addTab(general, QString());
        dialingTab = new QWidget();
        dialingTab->setObjectName(QStringLiteral("dialingTab"));
        verticalLayout_8 = new QVBoxLayout(dialingTab);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        dialRulesTabs = new QTabWidget(dialingTab);
        dialRulesTabs->setObjectName(QStringLiteral("dialRulesTabs"));
        dialGeneralTab = new QWidget();
        dialGeneralTab->setObjectName(QStringLiteral("dialGeneralTab"));
        layoutWidget = new QWidget(dialGeneralTab);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(9, 9, 471, 131));
        verticalLayout_9 = new QVBoxLayout(layoutWidget);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_9->addWidget(label);

        contextLine = new QLineEdit(layoutWidget);
        contextLine->setObjectName(QStringLiteral("contextLine"));

        verticalLayout_9->addWidget(contextLine);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout_9->addWidget(label_2);

        prefixLine = new QLineEdit(layoutWidget);
        prefixLine->setObjectName(QStringLiteral("prefixLine"));

        verticalLayout_9->addWidget(prefixLine);

        internationalLabel = new QLabel(layoutWidget);
        internationalLabel->setObjectName(QStringLiteral("internationalLabel"));

        verticalLayout_9->addWidget(internationalLabel);

        internationalLine = new QLineEdit(layoutWidget);
        internationalLine->setObjectName(QStringLiteral("internationalLine"));

        verticalLayout_9->addWidget(internationalLine);

        dialRulesTabs->addTab(dialGeneralTab, QString());
        dialReplaceTab = new QWidget();
        dialReplaceTab->setObjectName(QStringLiteral("dialReplaceTab"));
        verticalLayout_10 = new QVBoxLayout(dialReplaceTab);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        replaceTree = new QTreeWidget(dialReplaceTab);
        replaceTree->setObjectName(QStringLiteral("replaceTree"));
        replaceTree->setColumnCount(3);

        verticalLayout_10->addWidget(replaceTree);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        replaceDialAddButton = new QPushButton(dialReplaceTab);
        replaceDialAddButton->setObjectName(QStringLiteral("replaceDialAddButton"));

        horizontalLayout_3->addWidget(replaceDialAddButton);

        replaceDialRemoveButton = new QPushButton(dialReplaceTab);
        replaceDialRemoveButton->setObjectName(QStringLiteral("replaceDialRemoveButton"));

        horizontalLayout_3->addWidget(replaceDialRemoveButton);


        verticalLayout_10->addLayout(horizontalLayout_3);

        dialRulesTabs->addTab(dialReplaceTab, QString());
        dialSpeedTab = new QWidget();
        dialSpeedTab->setObjectName(QStringLiteral("dialSpeedTab"));
        verticalLayout_11 = new QVBoxLayout(dialSpeedTab);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        speedTree = new QTreeWidget(dialSpeedTab);
        speedTree->setObjectName(QStringLiteral("speedTree"));
        speedTree->setColumnCount(2);

        verticalLayout_11->addWidget(speedTree);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        speedAddButton = new QPushButton(dialSpeedTab);
        speedAddButton->setObjectName(QStringLiteral("speedAddButton"));

        horizontalLayout_4->addWidget(speedAddButton);

        speedEditButton = new QPushButton(dialSpeedTab);
        speedEditButton->setObjectName(QStringLiteral("speedEditButton"));

        horizontalLayout_4->addWidget(speedEditButton);

        speedRemoveButton = new QPushButton(dialSpeedTab);
        speedRemoveButton->setObjectName(QStringLiteral("speedRemoveButton"));

        horizontalLayout_4->addWidget(speedRemoveButton);


        verticalLayout_11->addLayout(horizontalLayout_4);

        dialRulesTabs->addTab(dialSpeedTab, QString());

        verticalLayout_8->addWidget(dialRulesTabs);

        tabWidget->addTab(dialingTab, QString());
        server = new QWidget();
        server->setObjectName(QStringLiteral("server"));
        formLayout = new QFormLayout(server);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        groupBox = new QGroupBox(server);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        serverLabel = new QLabel(groupBox);
        serverLabel->setObjectName(QStringLiteral("serverLabel"));

        verticalLayout_3->addWidget(serverLabel);

        serverName = new QLineEdit(groupBox);
        serverName->setObjectName(QStringLiteral("serverName"));

        verticalLayout_3->addWidget(serverName);

        userLabel = new QLabel(groupBox);
        userLabel->setObjectName(QStringLiteral("userLabel"));

        verticalLayout_3->addWidget(userLabel);

        userName = new QLineEdit(groupBox);
        userName->setObjectName(QStringLiteral("userName"));

        verticalLayout_3->addWidget(userName);

        passLabel = new QLabel(groupBox);
        passLabel->setObjectName(QStringLiteral("passLabel"));

        verticalLayout_3->addWidget(passLabel);

        password = new QLineEdit(groupBox);
        password->setObjectName(QStringLiteral("password"));
        password->setEchoMode(QLineEdit::Password);

        verticalLayout_3->addWidget(password);

        portLabel = new QLabel(groupBox);
        portLabel->setObjectName(QStringLiteral("portLabel"));

        verticalLayout_3->addWidget(portLabel);

        port = new QLineEdit(groupBox);
        port->setObjectName(QStringLiteral("port"));

        verticalLayout_3->addWidget(port);


        formLayout->setWidget(0, QFormLayout::SpanningRole, groupBox);

        tabWidget->addTab(server, QString());
        extensions = new QWidget();
        extensions->setObjectName(QStringLiteral("extensions"));
        verticalLayout_2 = new QVBoxLayout(extensions);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        groupBox_2 = new QGroupBox(extensions);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        treeWidget = new QTreeWidget(groupBox_2);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(1, QStringLiteral("Protocol"));
        __qtreewidgetitem->setText(0, QStringLiteral("Extension"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
        treeWidget->setRootIsDecorated(false);
        treeWidget->setColumnCount(2);

        horizontalLayout_2->addWidget(treeWidget);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        addButton = new QPushButton(groupBox_2);
        addButton->setObjectName(QStringLiteral("addButton"));

        verticalLayout_5->addWidget(addButton);

        removeButton = new QPushButton(groupBox_2);
        removeButton->setObjectName(QStringLiteral("removeButton"));

        verticalLayout_5->addWidget(removeButton);

        editButton = new QPushButton(groupBox_2);
        editButton->setObjectName(QStringLiteral("editButton"));

        verticalLayout_5->addWidget(editButton);

        verticalSpacer = new QSpacerItem(20, 158, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);


        horizontalLayout_2->addLayout(verticalLayout_5);


        verticalLayout_2->addWidget(groupBox_2);

        tabWidget->addTab(extensions, QString());
        outlook = new QWidget();
        outlook->setObjectName(QStringLiteral("outlook"));
        verticalLayout_6 = new QVBoxLayout(outlook);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        groupBox_3 = new QGroupBox(outlook);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        layoutWidget1 = new QWidget(groupBox_3);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(10, 30, 326, 88));
        verticalLayout_7 = new QVBoxLayout(layoutWidget1);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        enableOutlookBox = new QCheckBox(layoutWidget1);
        enableOutlookBox->setObjectName(QStringLiteral("enableOutlookBox"));
        enableOutlookBox->setChecked(true);

        verticalLayout_7->addWidget(enableOutlookBox);

        refreshBox = new QCheckBox(layoutWidget1);
        refreshBox->setObjectName(QStringLiteral("refreshBox"));
        refreshBox->setChecked(true);

        verticalLayout_7->addWidget(refreshBox);

        displayContactCallBox = new QCheckBox(layoutWidget1);
        displayContactCallBox->setObjectName(QStringLiteral("displayContactCallBox"));

        verticalLayout_7->addWidget(displayContactCallBox);

        displayContactUnknownBox = new QCheckBox(layoutWidget1);
        displayContactUnknownBox->setObjectName(QStringLiteral("displayContactUnknownBox"));

        verticalLayout_7->addWidget(displayContactUnknownBox);


        verticalLayout_6->addWidget(groupBox_3);

        tabWidget->addTab(outlook, QString());

        verticalLayout_4->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        poweredLabel = new QLabel(SettingsDialog);
        poweredLabel->setObjectName(QStringLiteral("poweredLabel"));

        verticalLayout->addWidget(poweredLabel);

        bicomLink = new QLabel(SettingsDialog);
        bicomLink->setObjectName(QStringLiteral("bicomLink"));
        bicomLink->setStyleSheet(QStringLiteral("color:rgb(0, 0, 255)"));
        bicomLink->setOpenExternalLinks(true);

        verticalLayout->addWidget(bicomLink);


        horizontalLayout->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout_4->addLayout(horizontalLayout);


        retranslateUi(SettingsDialog);

        tabWidget->setCurrentIndex(0);
        dialRulesTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "OutCALL  - Settings", 0));
        generalGroupBox->setTitle(QApplication::translate("SettingsDialog", "General Settings", 0));
        autoSignIn->setText(QApplication::translate("SettingsDialog", "Allow automatic Sign In when connected to the Internet", 0));
        autoStartBox->setText(QApplication::translate("SettingsDialog", "Launch application at startup", 0));
        callerIDBox->setText(QApplication::translate("SettingsDialog", "Minimum CallerID length for which to display popup notification:", 0));
        languageGroup->setTitle(QApplication::translate("SettingsDialog", "Language", 0));
        languageButton->setText(QApplication::translate("SettingsDialog", "Add another language", 0));
        languageLabel->setText(QApplication::translate("SettingsDialog", "(Note: application restart is required)", 0));
        tabWidget->setTabText(tabWidget->indexOf(general), QApplication::translate("SettingsDialog", "General", 0));
        label->setText(QApplication::translate("SettingsDialog", "Outgoing context:", 0));
        contextLine->setText(QApplication::translate("SettingsDialog", "default", 0));
        label_2->setText(QApplication::translate("SettingsDialog", "Outgoing prefix:", 0));
        prefixLine->setText(QString());
        internationalLabel->setText(QApplication::translate("SettingsDialog", "International dialling code:", 0));
        dialRulesTabs->setTabText(dialRulesTabs->indexOf(dialGeneralTab), QApplication::translate("SettingsDialog", "General", 0));
        QTreeWidgetItem *___qtreewidgetitem = replaceTree->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("SettingsDialog", "Regular expression", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("SettingsDialog", "Replacement text", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("SettingsDialog", "Text to replace", 0));
        replaceDialAddButton->setText(QApplication::translate("SettingsDialog", "Add", 0));
        replaceDialRemoveButton->setText(QApplication::translate("SettingsDialog", "Remove", 0));
        dialRulesTabs->setTabText(dialRulesTabs->indexOf(dialReplaceTab), QApplication::translate("SettingsDialog", "Replacement rules", 0));
        QTreeWidgetItem *___qtreewidgetitem1 = speedTree->headerItem();
        ___qtreewidgetitem1->setText(1, QApplication::translate("SettingsDialog", "Phone number", 0));
        ___qtreewidgetitem1->setText(0, QApplication::translate("SettingsDialog", "Code", 0));
        speedAddButton->setText(QApplication::translate("SettingsDialog", "Add", 0));
        speedEditButton->setText(QApplication::translate("SettingsDialog", "Edit", 0));
        speedRemoveButton->setText(QApplication::translate("SettingsDialog", "Remove", 0));
        dialRulesTabs->setTabText(dialRulesTabs->indexOf(dialSpeedTab), QApplication::translate("SettingsDialog", "Speed dial", 0));
        tabWidget->setTabText(tabWidget->indexOf(dialingTab), QApplication::translate("SettingsDialog", "Dialing rules", 0));
        groupBox->setTitle(QApplication::translate("SettingsDialog", "Server Settings", 0));
        serverLabel->setText(QApplication::translate("SettingsDialog", "Server:", 0));
        userLabel->setText(QApplication::translate("SettingsDialog", "Username:", 0));
        passLabel->setText(QApplication::translate("SettingsDialog", "Password:", 0));
        portLabel->setText(QApplication::translate("SettingsDialog", "Port:", 0));
        port->setText(QApplication::translate("SettingsDialog", "5038", 0));
        tabWidget->setTabText(tabWidget->indexOf(server), QApplication::translate("SettingsDialog", "Server", 0));
        groupBox_2->setTitle(QApplication::translate("SettingsDialog", "Extensions", 0));
        addButton->setText(QApplication::translate("SettingsDialog", "Add", 0));
        removeButton->setText(QApplication::translate("SettingsDialog", "Remove", 0));
        editButton->setText(QApplication::translate("SettingsDialog", "Edit", 0));
        tabWidget->setTabText(tabWidget->indexOf(extensions), QApplication::translate("SettingsDialog", "Extensions", 0));
        groupBox_3->setTitle(QApplication::translate("SettingsDialog", "Outlook settings", 0));
        enableOutlookBox->setText(QApplication::translate("SettingsDialog", "Enable Outlook plugin (Requires administrative privileges)", 0));
        refreshBox->setText(QApplication::translate("SettingsDialog", "Refresh contacts daily", 0));
        displayContactCallBox->setText(QApplication::translate("SettingsDialog", "Display contact information for inbound calls", 0));
        displayContactUnknownBox->setText(QApplication::translate("SettingsDialog", "Display new contact window for unknown callerID", 0));
        tabWidget->setTabText(tabWidget->indexOf(outlook), QApplication::translate("SettingsDialog", "Outlook", 0));
        poweredLabel->setText(QApplication::translate("SettingsDialog", "Powered by:", 0));
        bicomLink->setText(QApplication::translate("SettingsDialog", "<html><head/><body><p><a href=\"http://www.bicomsystems.com/\"><span style=\" text-decoration: underline; color:#0000ff;\">Bicom Systems Ltd.</span></a></p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
