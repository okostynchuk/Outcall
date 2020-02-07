#ifndef VIEWORGCONTACTDIALOG_H
#define VIEWORGCONTACTDIALOG_H

#include "ViewContactDialog.h"

#include <QDialog>
#include <QValidator>
#include <QHeaderView>
#include <QSqlQueryModel>
#include <QScrollBar>

namespace Ui {
class ViewOrgContactDialog;
}

class ViewOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewOrgContactDialog(QWidget *parent = 0);
    void setOrgValuesContacts(QString &);
    void setOrgValuesCallHistory(QString &);
    enum Calls
    {
        MISSED = 0,
        RECIEVED = 1,
        PLACED = 2
    };
    void addCall(const QMap<QString, QVariant> &call, Calls calls);

    ~ViewOrgContactDialog();

protected slots:
    void onUpdate();
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();
    void onTableClicked(const QModelIndex &index);
    void showCard(const QModelIndex &index);
    void onSectionClicked (int logicalIndex);
    void onSortingSectionClicked (int logicalIndex);
    void clearEditText();

private:
    Ui::ViewOrgContactDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QValidator *validator;
    QHeaderView *m_horiz_header;
    QHeaderView *m_horiz_header1;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    bool counter;
    int counter1;
    QSqlQueryModel *query_model;
    ViewContactDialog *viewContactDialog;
    QScrollBar *verticalScroll;
    QScrollBar *horizontalScroll;
    QString update;

};

#endif // VIEWORGCONTACTDIALOG_H
