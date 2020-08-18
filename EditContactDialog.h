#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include "AddOrgToPerson.h"

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
    void sendData(bool, int, int);

public slots:
    void receiveOrgID(QString &);

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isInnerPhone(QString *str);
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

    void setPos(int, int);

    bool eventFilter(QObject *object, QEvent *event);

public:
    explicit EditContactDialog(QWidget *parent = 0);
    ~EditContactDialog();

    void setValuesContacts(QString &);

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
