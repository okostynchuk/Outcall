#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>

namespace Ui {
class ViewContactDialog;
}

class ViewContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewContactDialog(QWidget *parent = 0);
    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);
    ~ViewContactDialog();

private:
    Ui::ViewContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
};

#endif // VIEWCONTACTDIALOG_H


