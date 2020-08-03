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
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QValidator>

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
    void receiveData(bool);

protected slots:
    void deleteObjects();
    void onUpdate();
    void onAddPerson();
    void onAddOrg();
    void onComboBoxListSelected();
    void onComboBoxSelected();
    void showCard(const QModelIndex &);
    void on_searchButton_clicked();
    void searchFunction();

private slots:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::ContactsDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    ViewContactDialog *viewContactDialog;
    ViewOrgContactDialog *viewOrgContactDialog;
    QValidator *validator;
    QWidget* addImageLabel(int &);
    QString page;
    int count;
    int remainder;
    QString pages;
    QSqlDatabase db;
    QSqlQuery query;
    QString go;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    bool filter;
    QList<QSqlQueryModel*> queries;
    QList<QWidget*> widgets;
    QList<QBoxLayout*> layouts;
    QList<QLabel*> labels;
};

#endif // CONTACTSDIALOG_H
