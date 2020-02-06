#ifndef PLACECALLDIALOG_H
#define PLACECALLDIALOG_H

#include "ChooseNumber.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QBoxLayout>
#include <QString>
#include <QLabel>
#include <QTableView>
#include <QScrollBar>
#include <QAbstractProxyModel>

class Contact;
class QTreeWidgetItem;
class ChooseNumber;

namespace Ui {
class PlaceCallDialog;
}

class PlaceCallDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaceCallDialog(QWidget *parent = 0);
    void show();
    void setOrgValuesContacts(QString &, const QModelIndex &);
    ~PlaceCallDialog();

protected:
     void clearCallTree();

public slots:
         void getValuesNumber(const QString &number);

protected slots:
    void onCallButton();
    void onCancelButton();
    void onUpdate();
    void modelNull();
    void onComboBoxSelected();
    void clearEditText();
    void onChangeContact(QString name);
    //void onContactIndexChange(const QString &name);
    //void onContactsLoaded(QList<Contact *> &contacts);
    void onSettingsChange();
    //void onItemDoubleClicked(QTreeWidgetItem * item, int);
    //void onItemClicked(QTreeWidgetItem * item, int);
    void showNumber(const QModelIndex &);
    //void setIndex(const QModelIndex &);


private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::PlaceCallDialog *ui;
    QList<Contact*> m_contacts;  /**< Contact list from Outlook */
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QString update;
    QString number;
    ChooseNumber *chooseNumber;
    QString updateID;
    QString updateID1;
};

#endif // PLACECALLDIALOG_H
