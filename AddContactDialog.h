#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QSqlQuery>
#include <QLineEdit>
#include <QKeyEvent>
#include <QPointer>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public slots:
    void receiveOrgName(QString, QString);

public:
    explicit AddContactDialog(QWidget *parent = 0);
    ~AddContactDialog();

    void setValues(QString);

private slots:
    void onSave();
    void onTextChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString*);

    void keyPressEvent(QKeyEvent*);
    void closeEvent(QCloseEvent*);

private:
    Ui::AddContactDialog *ui;

    QSqlDatabase db;

    QPointer<AddOrgToPerson> addOrgToPerson;

    QList<QLineEdit*> phonesList;

    QValidator* phonesValidator;
    QValidator* vyborIdValidator;

    QString orgID;
};

#endif // ADDCONTACTDIALOG_H
