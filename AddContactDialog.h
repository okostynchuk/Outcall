#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = 0);
    void setValuesCallHistory(QString &);
    ~AddContactDialog();

protected:
    void onSave();
    void addContact();
    void updateContact();
    void setValues();

private:
    Ui::AddContactDialog *ui;
    QSqlQueryModel *query1;
};

#endif // ADDCONTACTDIALOG_H
