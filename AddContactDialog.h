#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QWidget>
#include <QStringList>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = 0);
    void setValuesCallHistory(QString &);
    void setValuesPopupWindow(QString &);
    ~AddContactDialog();

public slots:
    void receiveOrgID(QString &);

protected slots:
    void onSave();

private:
    Ui::AddContactDialog *ui;
    AddOrgToPerson *addOrgToPerson;
    QValidator *validator;
    QValidator *validator2;
    QStringList numbers;

private slots:
    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

signals:
    void sendData(bool);
};

#endif // ADDCONTACTDIALOG_H
