#ifndef ADDORGCONTACTDIALOG_H
#define ADDORGCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QStringList>

namespace Ui {
class AddOrgContactDialog;
}

class AddOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOrgContactDialog(QWidget *parent = 0);
    void setOrgValuesCallHistory(QString &);
    void setOrgValuesPopupWindow(QString &);
    ~AddOrgContactDialog();

protected:
    void onSave();
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

private:
    Ui::AddOrgContactDialog *ui;
    QValidator *validator;
    QStringList numbers;

signals:
    void sendData(bool);
};

#endif // ADDORGCONTACTDIALOG_H
