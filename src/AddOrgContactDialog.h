#ifndef ADDORGCONTACTDIALOG_H
#define ADDORGCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QSqlQuery>
#include <QLineEdit>
#include <QKeyEvent>

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

    void setValues(QString);

private slots:
    void onSave();
    void onTextChanged();

    bool isPhone(QString* str);

    void keyPressEvent(QKeyEvent*);

private:
    Ui::AddOrgContactDialog *ui;

    QSqlDatabase db;

    QList<QLineEdit*> phonesList;

    QValidator* phonesValidator;
    QValidator* vyborIdValidator;

    QStringList numbers;
};

#endif // ADDORGCONTACTDIALOG_H
