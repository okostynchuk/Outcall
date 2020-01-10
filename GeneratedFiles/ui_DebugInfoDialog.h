/********************************************************************************
** Form generated from reading UI file 'DebugInfoDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGINFODIALOG_H
#define UI_DEBUGINFODIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DebugInfoDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPlainTextEdit *textEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *clear;
    QPushButton *exit;

    void setupUi(QDialog *DebugInfoDialog)
    {
        if (DebugInfoDialog->objectName().isEmpty())
            DebugInfoDialog->setObjectName(QStringLiteral("DebugInfoDialog"));
        DebugInfoDialog->resize(613, 479);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        DebugInfoDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(DebugInfoDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(DebugInfoDialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        textEdit = new QPlainTextEdit(DebugInfoDialog);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        verticalLayout->addWidget(textEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        clear = new QPushButton(DebugInfoDialog);
        clear->setObjectName(QStringLiteral("clear"));

        horizontalLayout->addWidget(clear);

        exit = new QPushButton(DebugInfoDialog);
        exit->setObjectName(QStringLiteral("exit"));

        horizontalLayout->addWidget(exit);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(DebugInfoDialog);

        QMetaObject::connectSlotsByName(DebugInfoDialog);
    } // setupUi

    void retranslateUi(QDialog *DebugInfoDialog)
    {
        DebugInfoDialog->setWindowTitle(QApplication::translate("DebugInfoDialog", "OutCALL - Debug Info", 0));
        label->setText(QApplication::translate("DebugInfoDialog", "Debug info:", 0));
        clear->setText(QApplication::translate("DebugInfoDialog", "Clear", 0));
        exit->setText(QApplication::translate("DebugInfoDialog", "Exit", 0));
    } // retranslateUi

};

namespace Ui {
    class DebugInfoDialog: public Ui_DebugInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGINFODIALOG_H
