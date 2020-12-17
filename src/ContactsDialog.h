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
    void receiveData(bool update);

public:
    explicit ContactsDialog(QWidget* parent = 0);
    ~ContactsDialog();

private slots:
    void loadContacts();
    void onUpdate();
    void onAddPerson();
    void onAddOrg();
    void currentIndexChanged();
    void showCard(const QModelIndex& index);
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
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::ContactsDialog* ui;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QModelIndexList selectionModel;

    QPointer<QSqlQueryModel> queryModel;

    AddContactDialog* addContactDialog;
    AddOrgContactDialog* addOrgContactDialog;
    ViewContactDialog* viewContactDialog;
    ViewOrgContactDialog* viewOrgContactDialog;

    QValidator* validator;

    QWidget* addImageLabel(qint32 row_index);
    QWidget* addWidgetNote(qint32 row_index, bool url);

    QList<QWidget*> widgets;

    QSqlDatabase db;

    QString page;  
    QString go;

    bool filter;

    QByteArray geometry;
};

#endif // CONTACTSDIALOG_H
