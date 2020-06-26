#ifndef EDITORGCONTACTDIALOG_H
#define EDITORGCONTACTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QValidator>
#include <QStringList>

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
    void setOrgValuesPopupWindow(QString &);
    ~EditOrgContactDialog();

signals:
    void sendData(bool);

protected slots:
    void onSave();
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

private:
    Ui::EditOrgContactDialog *ui;
    QValidator *validator;
    QSqlQuery *query1;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QString number;
    QStringList numbers;
};

#endif // EDITORGCONTACTDIALOG_H
