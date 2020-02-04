#ifndef ADDORGCONTACTDIALOG_H
#define ADDORGCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>

namespace Ui {
class AddOrgContactDialog;
}

class AddOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOrgContactDialog(QWidget *parent = 0);
    void setOrgValuesCallHistory(QString &);
    ~AddOrgContactDialog();

protected:
    void onSave();

private:
    Ui::AddOrgContactDialog *ui;
    QValidator *validator;
    bool eventFilter(QObject*, QEvent*event);
};

#endif // ADDORGCONTACTDIALOG_H
