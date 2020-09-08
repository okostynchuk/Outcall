#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QLineEdit>
#include <QKeyEvent>
#include <QPointer>
#include <QLineEdit>

class ViewContactDialog;

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool, int, int);

public slots:
    void receiveOrgName(QString, QString);
    void setPos(int, int);

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString*);

    void keyPressEvent(QKeyEvent*);
    void closeEvent(QCloseEvent*);

public:
    explicit EditContactDialog(QWidget *parent = 0);
    ~EditContactDialog();

    void setValuesContacts(QString);

private:
    Ui::EditContactDialog *ui;

    QSqlDatabase db;

    QPointer<AddOrgToPerson> addOrgToPerson;

    QList<QLineEdit*> phonesList;

    QStringList oldPhonesList;

    QValidator* phonesValidator;
    QValidator* vyborIdValidator;

    QString updateID;
    QString orgID;
    QString number;  
};

#endif // EDITCONTACTDIALOG_H
