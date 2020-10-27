#ifndef ADDPERSONTOORG_H
#define ADDPERSONTOORG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>
#include <QSqlQuery>
#include <QPointer>

namespace Ui {
class AddPersonToOrg;
}

class AddPersonToOrg : public QDialog
{
    Q_OBJECT

signals:
    void newPerson();

public:
    explicit AddPersonToOrg(QWidget* parent = 0);
    ~AddPersonToOrg();

    void setOrgId(const QString& orgId);

private slots:
    void loadPersons();

    void currentIndexChanged();
    void addPerson(const QModelIndex& index);
    void searchFunction();

    void on_searchButton_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();
    void on_lineEdit_returnPressed();

private:
    Ui::AddPersonToOrg* ui;

    QPointer<QSqlQueryModel> queryModel;

    QValidator* validator;

    QSqlDatabase db;

    QString orgId;
    QString orgName;
    QString page;
    QString go;

    qint32 count;
    qint32 remainder;

    bool filter;
};

#endif // ADDPERSONTOORG_H
