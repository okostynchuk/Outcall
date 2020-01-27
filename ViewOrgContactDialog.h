#ifndef VIEWORGCONTACTDIALOG_H
#define VIEWORGCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>

namespace Ui {
class ViewOrgContactDialog;
}

class ViewOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewOrgContactDialog(QWidget *parent = 0);
    void setOrgValuesContacts(QString &);
    void setOrgValuesCallHistory(QString &);
    ~ViewOrgContactDialog();

private:
    Ui::ViewOrgContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
};

#endif // VIEWORGCONTACTDIALOG_H
