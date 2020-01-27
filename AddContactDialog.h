#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
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

private:
    Ui::AddContactDialog *ui;
    QValidator *validator;
};

#endif // ADDCONTACTDIALOG_H
