#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QSqlQuery>
#include <QLineEdit>
#include <QKeyEvent>
#include <QPointer>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool update);

public slots:
    void receiveOrgName(const QString& id, const QString& name);

public:
    explicit AddContactDialog(QWidget* parent = 0);
    ~AddContactDialog();

    void setValues(const QString& number);

private slots:
    void onSave();
    void onTextChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString* str);

    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    Ui::AddContactDialog* ui;

    QSqlDatabase db;

    QPointer<AddOrgToPerson> addOrgToPerson;

    QList<QLineEdit*> phonesList;

    QValidator* Validator;
    QValidator* vyborIdValidator;
    //QValidator* vyborIdValidator;

    QString orgId;
};

#endif // ADDCONTACTDIALOG_H
