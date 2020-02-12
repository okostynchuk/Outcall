
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
#include <QHeaderView>
#include <QBoxLayout>
#include <QTableView>
#include <QScrollBar>
#include <QTimer>

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
    void onEdit(const QModelIndex &);
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();
    void showCard(const QModelIndex &);
    void onSectionClicked(int);
    void onSortingSectionClicked(int);
    void clearEditText();

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
    QHeaderView *m_horiz_header;
    QWidget* addImageLabel(int &);
    QString update;
    int valueV;
    int valueH;
    QTimer timer;
    QList<QWidget*> widgets;
    QList<QBoxLayout*> layouts;
    QList<QLabel*> labels;

};

#endif // CONTACTSDIALOG_H
