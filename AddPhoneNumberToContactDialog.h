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
#include <QMessageBox>

namespace Ui {
class AddPhoneNumberToContactDialog;
}

class AddPhoneNumberToContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPhoneNumberToContactDialog(QWidget *parent = 0);
    void setPhoneNumberPopupWindow(QString &);
    ~AddPhoneNumberToContactDialog();

signals:
    void sendData(bool);

protected slots:
    void deleteObjects();
    void onUpdate();
    void onComboBoxListSelected();
    void onComboBoxSelected();
    void setPhoneNumber(const QModelIndex &index);
    void on_searchButton_clicked();
    void searchFunction();

private slots:
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::AddPhoneNumberToContactDialog *ui;
    QSqlQueryModel *query1;
    QValidator *validator;
    QString phoneNumber;
    QString page;
    int count;
    int remainder;
    QString pages;
    QSqlDatabase db;
    QSqlQuery query;
    QString go;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    bool filter;
    QList<QSqlQueryModel*> queries;
};

#endif // ADDPHONENUMBERTOCONTACTDIALOG_H
