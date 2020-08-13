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

signals:
    void sendData(bool, int, int);

public:
    explicit EditOrgContactDialog(QWidget *parent = 0);
    ~EditOrgContactDialog();

    void setOrgValuesContacts(QString &);
    void setOrgValuesCallHistory(QString &);
    void setOrgValuesPopupWindow(QString &);

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();

    bool isInnerPhone(QString *str);
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

    void setPos(int, int);

    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::EditOrgContactDialog *ui;

    QSqlQuery *query1;

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

#endif // EDITORGCONTACTDIALOG_H
