#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTreeView>
#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>

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
    void on_lineEdit_returnPressed();
    void onSort();

private:
    Ui::ContactsDialog *ui;
    QSqlQueryModel *query1;    
    QSqlQueryModel *query2;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    QTreeView *treeView;
    QSortFilterProxyModel *proxyModel;
    QWidget* addImageLabel(int &i) const;
    QWidget* createEditButton(int &i) const;
    QString update;
};

#endif // CONTACTSDIALOG_H
