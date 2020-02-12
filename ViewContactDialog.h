#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include "Global.h"
#include "SettingsDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <QList>
#include <QWidget>
#include <QLabel>

namespace Ui {
class ViewContactDialog;
}

class ViewContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewContactDialog(QWidget *parent = 0);
    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);
    enum Calls
    {
        MISSED = 0,
        RECIEVED = 1,
        PLACED = 2
    };
    void addCall(const QMap<QString, QVariant> &call, Calls calls);
    ~ViewContactDialog();

protected slots:
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void deleteObjects();

private:
    Ui::ViewContactDialog *ui;
    SettingsDialog *settingsDialog;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QValidator *validator;
    QString updateID;
    QString uniqueid;
    QString number;
    QString my_number;
    int count2 = 1;
    QString contact_number;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QWidget* loadNote();
    QList<QWidget*> widgets;
    QList<QLabel*> notes;
};

#endif // VIEWCONTACTDIALOG_H
