#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class ContactsDialog;
}

class ContactsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactsDialog(QWidget *parent = 0);
    ~ContactsDialog();

protected slots:
    void onUpdate();
    void onAddPerson();
    void onAddOrg();
    void onEdit();
    void onTableClicked(const QModelIndex &);
    void onComboBoxSelected();
    void tableUpdate();
    void on_lineEdit_returnPressed();

private:
    Ui::ContactsDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;

    QWidget* addImageLabel(int &i) const;
    QWidget* createEditButton(int &i) const;
};

#endif // CONTACTSDIALOG_H
