#ifndef EDITORGCONTACTDIALOG_H
#define EDITORGCONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QValidator>
#include <QKeyEvent>

namespace Ui {
class EditOrgContactDialog;
}

class EditOrgContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool, int, int);

public slots:
    void setPos(int, int);

public:
    explicit EditOrgContactDialog(QWidget *parent = 0);
    ~EditOrgContactDialog();

    void setOrgValuesContacts(QString);

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();

    bool isPhone(QString*);

    void keyPressEvent(QKeyEvent*);

private:
    Ui::EditOrgContactDialog *ui;

    QList<QLineEdit*> phonesList;

    QStringList oldPhonesList;

    QValidator* phonesValidator;
    QValidator* vyborIdValidator;

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
