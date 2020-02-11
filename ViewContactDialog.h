#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

//#include "Global.h"
//#include "OutCALL.h"

#include <QDialog>
#include <QValidator>
#include <QSqlQueryModel>
#include <QTableView>
#include <QTextEdit>
#include <QList>
#include <QMap>
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

private:
    Ui::ViewContactDialog *ui;
    QSqlQuery *query;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QValidator *validator;
    QString updateID;
    QString uniqueid;
    QString contact_number;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QWidget* loadNote(int &);
    QList<QWidget*> widgets;
    QList<QLabel*> notes;
};

#endif // VIEWCONTACTDIALOG_H
