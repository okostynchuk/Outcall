#ifndef INTERNALCONTACTSDIALOG_H
#define INTERNALCONTACTSDIALOG_H

#include "AsteriskManager.h"
#include "Global.h"
#include "AddReminderDialog.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QDebug>

class AddReminderDialog;

namespace Ui {
class InternalContactsDialog;
}

class InternalContactsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InternalContactsDialog(QWidget* parent = 0);
    ~InternalContactsDialog();

private slots:
    void loadContacts();
    void onExtenStatusChanged(QString exten, QString state);

    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);

    void on_callButton_clicked();
    void on_addReminderButton_clicked();
    void on_listWidget_clicked();

private:
    Ui::InternalContactsDialog* ui;

    AddReminderDialog* addReminderDialog;

    QValidator* validator;

    QString my_number;
    QString my_exten;

    QStringList extensions;

    QMap<QString, QString> stateList;
    QMap<QString, int> indexes;
};

#endif // INTERNALCONTACTSDIALOG_H
