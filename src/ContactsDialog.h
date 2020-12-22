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

    QSqlDatabase m_db;

    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QModelIndexList m_selections;

    QPointer<QSqlQueryModel> m_queryModel;

    AddContactDialog* m_addContactDialog;
    AddOrgContactDialog* m_addOrgContactDialog;
    ViewContactDialog* m_viewContactDialog;
    ViewOrgContactDialog* m_viewOrgContactDialog;

    QWidget* addImageLabel(qint32 row_index);
    QWidget* addWidgetNote(qint32 row_index, bool url);

    QList<QWidget*> m_widgets;

    QString m_page;  
    QString m_go;

    bool m_filter;

    QByteArray m_geometry;
};

#endif // CONTACTSDIALOG_H
