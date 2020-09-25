#ifndef ADDPERSONTOORG_H
#define ADDPERSONTOORG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>
#include <QSqlQuery>

namespace Ui {
class AddPersonToOrg;
}

class AddPersonToOrg : public QDialog
{
    Q_OBJECT

signals:
    void newPerson();

public:
    explicit AddPersonToOrg(QWidget *parent = 0);
    ~AddPersonToOrg();

    void setOrgId(QString);

private slots:
    void deleteObjects();
    void onUpdate();
    void currentIndexChanged();
    void addPerson(const QModelIndex &index);
    void searchFunction();

    void on_searchButton_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::AddPersonToOrg *ui;

    QSqlQueryModel* queryModel;

    QValidator* validator;

    QList<QSqlQueryModel*> queries;

    QString orgId;
    QString orgName;
    QString page;
    int count;
    int remainder;
    QString pages;
    QSqlDatabase db;
    QSqlQuery query;
    QString go;
    bool filter;
};

#endif // ADDPERSONTOORG_H
