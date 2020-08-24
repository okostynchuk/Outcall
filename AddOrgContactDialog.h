#ifndef ADDORGCONTACTDIALOG_H
#define ADDORGCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QSqlQuery>
#include <QLineEdit>

namespace Ui {
class AddOrgContactDialog;
}

class AddOrgContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public:
    explicit AddOrgContactDialog(QWidget *parent = 0);
    ~AddOrgContactDialog();

    void setOrgValuesCallHistory(QString &);
    void setOrgValuesPopupWindow(QString &);   

private slots:
    void onSave();

    bool isInnerPhone(QString *str);
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

private:
    Ui::AddOrgContactDialog *ui;

    QSqlQuery *query1;

    QList <QLineEdit *> phonesList;

    QValidator *validator;

    QStringList numbers;
};

#endif // ADDORGCONTACTDIALOG_H
