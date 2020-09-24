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
#include <QSqlQuery>
#include <QValidator>

namespace Ui {
class ContactsDialog;
}

class ContactsDialog : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool);

public:
    explicit ContactsDialog(QWidget *parent = 0);
    ~ContactsDialog();

private slots:
    void loadContacts();
    void onUpdate();
    void onAddPerson();
    void onAddOrg();
    void currentIndexChanged();
    void showCard(const QModelIndex &);
    void searchFunction();

    void deleteObjects();

    void on_updateButton_clicked();
    void on_searchButton_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Ui::ContactsDialog *ui;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QModelIndexList selectionModel;

    QSqlQueryModel* queryModel;

    AddContactDialog* addContactDialog;
    AddOrgContactDialog* addOrgContactDialog;
    ViewContactDialog* viewContactDialog;
    ViewOrgContactDialog* viewOrgContactDialog;

    QValidator* validator;

    QWidget* addImageLabel(int);
    QWidget* addWidgetNote(int, QString);

    QList<QSqlQueryModel*> queries;
    QList<QWidget*> widgets;
    QList<QHBoxLayout*> layouts;
    QList<QLabel*> labels;

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
};

#endif // CONTACTSDIALOG_H
