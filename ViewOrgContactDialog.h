#ifndef VIEWORGCONTACTDIALOG_H
#define VIEWORGCONTACTDIALOG_H

#include "ViewContactDialog.h"

#include <QDialog>
#include <QValidator>
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
    ~ViewOrgContactDialog();

protected slots:
    void onUpdate();
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();
    void onTableClicked(const QModelIndex &index);
    void showCard(const QModelIndex &index);

private:
    Ui::ViewOrgContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QSqlQueryModel *query_model;
    ViewContactDialog *viewContactDialog;
    QScrollBar *verticalScroll;
    QScrollBar *horizontalScroll;
    QString update;
};

#endif // VIEWORGCONTACTDIALOG_H
