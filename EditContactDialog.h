#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QStringList>

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditContactDialog(QWidget *parent = 0);
    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);
    void setValuesPopupWindow(QString &number);
    ~EditContactDialog();

public slots:
    void receiveOrgID(QString &);

protected slots:
    void onSave();

private:
    Ui::EditContactDialog *ui;
    AddOrgToPerson *addOrgToPerson;
    QValidator *validator;
    QValidator *validator2;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QString number;
    QStringList numbers;

private slots:
    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

signals:
    void sendData(bool);
};

#endif // EDITCONTACTDIALOG_H
