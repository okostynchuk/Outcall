/********************************************************************************
** Form generated from reading UI file 'AddExtensionDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDEXTENSIONDIALOG_H
#define UI_ADDEXTENSIONDIALOG_H

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
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AddExtensionDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *extenLine;
    QLabel *label_2;
    QComboBox *protocol;
    QHBoxLayout *horizontalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *AddExtensionDialog)
    {
        if (AddExtensionDialog->objectName().isEmpty())
            AddExtensionDialog->setObjectName(QStringLiteral("AddExtensionDialog"));
        AddExtensionDialog->resize(459, 195);
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/outcall-logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        AddExtensionDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(AddExtensionDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        label = new QLabel(AddExtensionDialog);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        extenLine = new QLineEdit(AddExtensionDialog);
        extenLine->setObjectName(QStringLiteral("extenLine"));

        verticalLayout->addWidget(extenLine);

        label_2 = new QLabel(AddExtensionDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        protocol = new QComboBox(AddExtensionDialog);
        protocol->setObjectName(QStringLiteral("protocol"));

        verticalLayout->addWidget(protocol);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        buttonBox = new QDialogButtonBox(AddExtensionDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(AddExtensionDialog);

        QMetaObject::connectSlotsByName(AddExtensionDialog);
    } // setupUi

    void retranslateUi(QDialog *AddExtensionDialog)
    {
        AddExtensionDialog->setWindowTitle(QString());
        label->setText(QApplication::translate("AddExtensionDialog", "Extension:", 0));
        label_2->setText(QApplication::translate("AddExtensionDialog", "Protocol:", 0));
        protocol->clear();
        protocol->insertItems(0, QStringList()
         << QApplication::translate("AddExtensionDialog", "SIP", 0)
         << QApplication::translate("AddExtensionDialog", "PJSIP", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class AddExtensionDialog: public Ui_AddExtensionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDEXTENSIONDIALOG_H
