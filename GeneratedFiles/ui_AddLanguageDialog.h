/********************************************************************************
** Form generated from reading UI file 'AddLanguageDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDLANGUAGEDIALOG_H
#define UI_ADDLANGUAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddLanguageDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QListWidget *listWidgetLanguage;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *addLanguageBtn;
    QPushButton *cancelBtn;

    void setupUi(QDialog *AddLanguageDialog)
    {
        if (AddLanguageDialog->objectName().isEmpty())
            AddLanguageDialog->setObjectName(QStringLiteral("AddLanguageDialog"));
        AddLanguageDialog->resize(379, 421);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        AddLanguageDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(AddLanguageDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(AddLanguageDialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        listWidgetLanguage = new QListWidget(AddLanguageDialog);
        listWidgetLanguage->setObjectName(QStringLiteral("listWidgetLanguage"));

        verticalLayout->addWidget(listWidgetLanguage);

        widget = new QWidget(AddLanguageDialog);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 0, -1, 0);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        addLanguageBtn = new QPushButton(widget);
        addLanguageBtn->setObjectName(QStringLiteral("addLanguageBtn"));

        horizontalLayout_2->addWidget(addLanguageBtn);

        cancelBtn = new QPushButton(widget);
        cancelBtn->setObjectName(QStringLiteral("cancelBtn"));

        horizontalLayout_2->addWidget(cancelBtn);


        verticalLayout->addWidget(widget);


        retranslateUi(AddLanguageDialog);

        QMetaObject::connectSlotsByName(AddLanguageDialog);
    } // setupUi

    void retranslateUi(QDialog *AddLanguageDialog)
    {
        AddLanguageDialog->setWindowTitle(QApplication::translate("AddLanguageDialog", "Add new language", 0));
        label->setText(QApplication::translate("AddLanguageDialog", "Please select language to add:", 0));
        addLanguageBtn->setText(QApplication::translate("AddLanguageDialog", "Add language", 0));
        cancelBtn->setText(QApplication::translate("AddLanguageDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class AddLanguageDialog: public Ui_AddLanguageDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDLANGUAGEDIALOG_H
