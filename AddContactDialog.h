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

signals:
    void sendData(bool);

public slots:
    void receiveOrgID(QString &);

public:
    explicit AddContactDialog(QWidget *parent = 0);
    ~AddContactDialog();

    void setValuesCallHistory(QString &);
    void setValuesPopupWindow(QString &);

private slots:
    void onSave();
    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString *str);
    bool isVyborID(QString *str);

private:
    Ui::AddContactDialog *ui;

    AddOrgToPerson *addOrgToPerson;

    QValidator *validator;

    QStringList numbers;
};

#endif // ADDCONTACTDIALOG_H
