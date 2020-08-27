#ifndef ADDORGTOPERSON_H
#define ADDORGTOPERSON_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QBoxLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QValidator>

namespace Ui {
class AddOrgToPerson;
}

class AddOrgToPerson : public QDialog
{
    Q_OBJECT

signals:
    void sendOrgID(QString);

public:
    explicit AddOrgToPerson(QWidget *parent = 0);
    ~AddOrgToPerson();

private slots:
    void deleteObjects();
    void onUpdate();
    void onComboBoxListSelected();
    void onComboBoxSelected();
    void getOrgID(const QModelIndex &index);
    void searchFunction();

    void on_searchButton_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::AddOrgToPerson *ui;

    QSqlQueryModel* queryModel;

    QValidator* validator;

    QList<QSqlQueryModel*> queries;

    QString page;
    int count;
    int remainder;
    QString pages;
    QSqlDatabase db;
    QSqlQuery query;
    QString go;
    QString entry_name;
    QString entry_city;
    bool filter;
};

#endif // ADDORGTOPERSON_H
