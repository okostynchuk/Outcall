#ifndef ADDORGCONTACDIALOG_H
#define ADDORGCONTACDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>

namespace Ui {
class AddOrgContacDialog;
}

class AddOrgContacDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOrgContacDialog(QWidget *parent = 0);
    ~AddOrgContacDialog();

protected:
    void onSave();

private:
    Ui::AddOrgContacDialog *ui;
    QSqlQueryModel *query1;
};

#endif // ADDORGCONTACDIALOG_H
