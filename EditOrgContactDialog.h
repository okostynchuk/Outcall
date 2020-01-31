#ifndef EDITORGCONTACTDIALOG_H
#define EDITORGCONTACTDIALOG_H

#include "EditContactDialog.h"
#include "ViewContactDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>
#include <QBoxLayout>
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
    ~EditOrgContactDialog();

protected slots:
    void onSave();
    void onEdit();
    void onUpdate();
    void deleteObjects();
    void onComboBoxSelected();
    void on_lineEdit_returnPressed();
    void onTableClicked(const QModelIndex &index);
    void showCard(const QModelIndex &index);    
    void onSectionClicked (int logicalIndex);
    void onSortingSectionClicked(int logicalIndex);
    void clearEditText();

private:
    Ui::EditOrgContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QWidget* createEditButton(int &);
    QSqlQueryModel *query_model;
    EditContactDialog *editContactDialog;
    ViewContactDialog *viewContactDialog;
    QHeaderView *m_horiz_header;
    QScrollBar *verticalScroll;
    QScrollBar *horizontalScroll;
    QString update;
    QStringList numbers;
    int counter;
    QList<QPushButton*> buttons;
    QList<QWidget*> widgets;
    QList<QBoxLayout*> layouts;

signals:
    void sendData(bool);
};

#endif // EDITORGCONTACTDIALOG_H
