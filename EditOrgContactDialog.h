#ifndef EDITORGCONTACTDIALOG_H
#define EDITORGCONTACTDIALOG_H

#include "EditContactDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>
#include <QBoxLayout>

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
    ~EditOrgContactDialog();

protected slots:
    void onSave();
    void onEdit();
    void onUpdate();
    void deleteObjects();
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();

protected:
    bool eventFilter(QObject*, QEvent*event);

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
    QWidget* createEditButton(int &);
    QSqlQueryModel *query_model;
    EditContactDialog *editContactDialog;
    QString update;
    QList<QPushButton*> buttons;
    QList<QWidget*> widgets;
    QList<QBoxLayout*> layouts;
};

#endif // EDITORGCONTACTDIALOG_H
