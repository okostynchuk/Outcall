#ifndef ADDPHONENUMBERTOCONTACTDIALOG_H
#define ADDPHONENUMBERTOCONTACTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QBoxLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QValidator>

namespace Ui {
class AddPhoneNumberToContactDialog;
}

class AddPhoneNumberToContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public:
    explicit AddPhoneNumberToContactDialog(QWidget *parent = 0);
    ~AddPhoneNumberToContactDialog();

    void setPhoneNumber(QString);

private slots:
    void deleteObjects();
    void onUpdate();
    void currentIndexChanged();
    void addPhoneNumber(const QModelIndex &index);
    void on_searchButton_clicked();
    void searchFunction();

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::AddPhoneNumberToContactDialog *ui;

    QSqlDatabase db;

    QSqlQueryModel* queryModel;

    QValidator* validator;

    QList<QSqlQueryModel*> queries;

    QString phoneNumber;
    QString page;
    qint32 count;
    qint32 remainder;
    QSqlQuery query;
    QString go;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    bool filter;
};

#endif // ADDPHONENUMBERTOCONTACTDIALOG_H
