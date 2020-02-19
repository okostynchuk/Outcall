#ifndef EDITORGCONTACTDIALOG_H
#define EDITORGCONTACTDIALOG_H

#include "EditContactDialog.h"
#include "ViewContactDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>
#include <QScrollBar>
#include <QStringList>
#include <QHeaderView>

namespace Ui {
class EditOrgContactDialog;
}

class EditOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditOrgContactDialog(QWidget *parent = 0);
    void setOrgValuesContacts(QString &);
    void setOrgValuesCallHistory(QString &);
    void setOrgValuesPopupWindow(QString &);
    ~EditOrgContactDialog();

signals:
    void sendData(bool);

public slots:
    void recieveData(bool);

protected slots:
    void onSave();
    void getID(const QModelIndex &index);
    void onEdit();
    void onUpdate();
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();
    void showCard(const QModelIndex &index);    
    void onSectionClicked (int logicalIndex);
    void searchFunction();
    void closeEvent(QCloseEvent *);

private slots:
    void on_pushButton_clicked();

private:
    Ui::EditOrgContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QString number;
    QSqlQueryModel *query_model;
    EditContactDialog *editContactDialog;
    ViewContactDialog *viewContactDialog;
    QHeaderView *m_horiz_header;
    QScrollBar *verticalScroll;
    QScrollBar *horizontalScroll;
    QString update;
    QString id;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    QString sort;
    QStringList numbers;
    bool updateOnClose;
    bool filter;
    int valueV;
    int valueH;
};

#endif // EDITORGCONTACTDIALOG_H
