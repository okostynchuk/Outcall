#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = 0);
    ~AddContactDialog();

protected:
    void onSave();

private:
    Ui::AddContactDialog *ui;
    QSqlQueryModel *query1;
};

#endif // ADDCONTACTDIALOG_H
