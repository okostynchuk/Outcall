#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QWidget>
#include <QStringList>

namespace Ui {
class AddContactDialog;
}

class AddContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddContactDialog(QWidget *parent = 0);
    void setValuesCallHistory(QString &);
    void setValuesPopupWindow(QString &);
    ~AddContactDialog();

protected:
    void onSave();
    void onComboBoxSelected();

private:
    Ui::AddContactDialog *ui;
    QValidator *validator;
    QStringList numbers;

signals:
    void sendData(bool);
};

#endif // ADDCONTACTDIALOG_H
