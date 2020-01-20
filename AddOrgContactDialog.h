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
    ~AddOrgContactDialog();

protected:
    void onSave();

private:
    Ui::AddOrgContactDialog *ui;
    QSqlQueryModel *query1;

};

#endif // ADDORGCONTACTDIALOG_H
