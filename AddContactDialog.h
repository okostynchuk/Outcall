#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QSqlQuery>
#include <QLineEdit>
#include <QKeyEvent>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public slots:
    void receiveOrgID(QString &);

public:
    explicit AddContactDialog(QWidget *parent = 0);
    ~AddContactDialog();

    void setValues(QString &);

private slots:
    void onSave();
    void onTextChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isInnerPhone(QString *str);
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

    void keyPressEvent(QKeyEvent* event);

private:
    Ui::AddContactDialog *ui;

    QList <QLineEdit *> phonesList;

    AddOrgToPerson *addOrgToPerson;

    QValidator *validator;
};

#endif // ADDCONTACTDIALOG_H
