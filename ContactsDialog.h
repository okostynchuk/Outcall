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
    void onComboBoxListSelected();
    void onComboBoxSelected();
    void showCard(const QModelIndex &);
    void onSectionClicked(int);
    void on_searchButton_clicked();
    void searchFunction();

private slots:
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

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
    QString page;
    QString go;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    QString sort;
    bool filter;
    int valueV;
    int valueH;
    QList<QSqlQueryModel*> queries;
    QList<QWidget*> widgets;
    QList<QBoxLayout*> layouts;
    QList<QLabel*> labels;
};

#endif // CONTACTSDIALOG_H
