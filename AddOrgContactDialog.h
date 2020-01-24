#ifndef ADDORGCONTACTDIALOG_H
#define ADDORGCONTACTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>

namespace Ui {
class AddOrgContactDialog;
}

class AddOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOrgContactDialog(QWidget *parent = 0);
    void setOrgValuesContacts(QString &);
    void setOrgValuesCallHistory(QString &);
    ~AddOrgContactDialog();

protected:
    void onSave();
    void addOrgContact();
    void updateOrgContact();

private:
    Ui::AddOrgContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
};

#endif // ADDORGCONTACTDIALOG_H
