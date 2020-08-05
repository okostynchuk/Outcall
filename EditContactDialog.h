#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include "AddOrgToPerson.h"
//#include "ViewContactDialog.h" //1!!!!!

#include <QDialog>
#include <QValidator>
#include <QStringList>

class ViewContactDialog;

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public slots:
    void receiveOrgID(QString &);

public:
    explicit EditContactDialog(QWidget *parent = 0);
    ~EditContactDialog();

    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);
    void setValuesPopupWindow(QString &number);

private slots:
    void onSave();
    void onReturn();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString *str);
    bool isVyborID(QString *str);

    void setPos(int, int);

private:
    Ui::EditContactDialog *ui;

    AddOrgToPerson *addOrgToPerson;
    ViewContactDialog *viewContactDialog;

    QValidator *validator;

    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QString number;
    QStringList numbers;
};

#endif // EDITCONTACTDIALOG_H
