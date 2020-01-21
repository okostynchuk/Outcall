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
    void AddOrgContactDialog::setValuesCallHistory(QString &);
    ~AddOrgContactDialog();

protected:
    void onSave();
    void addOrgContact();
    void updateOrgContact();

private:
    Ui::AddOrgContactDialog *ui;
    QSqlQueryModel *query1;

};

#endif // ADDORGCONTACTDIALOG_H
