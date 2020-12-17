#ifndef ADDORGTOPERSON_H
#define ADDORGTOPERSON_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QValidator>
#include <QPointer>

namespace Ui {
class AddOrgToPerson;
}

class AddOrgToPerson : public QDialog
{
    Q_OBJECT

signals:
    void sendOrgName(const QString& id, const QString& name);

public:
    explicit AddOrgToPerson(QWidget* parent = 0);
    ~AddOrgToPerson();

private slots:
    void loadOrgs();

    void currentIndexChanged();
    void getOrgName(const QModelIndex& index);
    void searchFunction();

    void on_searchButton_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::AddOrgToPerson* ui;

    QPointer<QSqlQueryModel> queryModel;

    QValidator* validator;

    QSqlDatabase db;

    QString page;
    QString go;

    bool filter;
};

#endif // ADDORGTOPERSON_H
