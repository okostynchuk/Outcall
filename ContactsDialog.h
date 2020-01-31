#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "EditContactDialog.h"
#include "EditOrgContactDialog.h"
#include "ViewContactDialog.h"
#include "ViewOrgContactDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTreeView>
#include <QAbstractProxyModel>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QScrollBar>

namespace Ui {
class ContactsDialog;
}

class ContactsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactsDialog(QWidget *parent = 0);
    ~ContactsDialog();

public slots:
    void recieveData(bool);

protected slots:
    void deleteObjects();
    void onUpdate();
    void onAddPerson();
    void onAddOrg();
    void onEdit();
    void onTableClicked(const QModelIndex &);
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();
    void on_sortButton_clicked();
    void setSortingEnabled();
    void showCard(const QModelIndex &);
    void onSectionClicked ( int logicalIndex );

private:
    Ui::ContactsDialog *ui;
    QSqlQueryModel *query1;    
    QSqlQueryModel *query2;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    ViewContactDialog *viewContactDialog;
    ViewOrgContactDialog *viewOrgContactDialog;

    QScrollBar *verticalScroll;
    QScrollBar *horizontalScroll;
    QWidget* addImageLabel(int &);
    QWidget* createEditButton(int &);
    QString update;
    int counter;
    QList<QPushButton*> buttons;
    QList<QWidget*> widgets;
    QList<QBoxLayout*> layouts;
    QList<QLabel*> labels;

};

#endif // CONTACTSDIALOG_H
