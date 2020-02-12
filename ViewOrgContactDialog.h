#ifndef VIEWORGCONTACTDIALOG_H
#define VIEWORGCONTACTDIALOG_H

#include "ViewContactDialog.h"
#include "SettingsDialog.h"
#include "Global.h"

#include <QDialog>
#include <QValidator>
#include <QHeaderView>
#include <QSqlQueryModel>
#include <QScrollBar>
#include <QTableView>
#include <QTextEdit>
#include <QList>
#include <QMap>
#include <QWidget>
#include <QLabel>

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
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void deleteObjects();

private:
    Ui::ViewOrgContactDialog *ui;
    SettingsDialog *settingsDialog;
    QSqlQuery *query;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QValidator *validator;
    QHeaderView *m_horiz_header;
    QHeaderView *m_horiz_header1;
    QString updateID;
    QString number;
    QString my_number;
    QString uniqueid;
    int count2 = 1;
    QString contact_number;
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
    QWidget* loadNote(int &);
    QList<QWidget*> widgets;
    QList<QLabel*> notes;

};

#endif // VIEWORGCONTACTDIALOG_H
