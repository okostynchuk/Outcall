#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QEvent>
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
    ~AddContactDialog();

protected:
    void onSave();
    void onComboBoxSelected();
    bool eventFilter(QObject*, QEvent*event);

private:
    Ui::AddContactDialog *ui;
    QValidator *validator;
    //QLineEdit *line1;
    QStringList numbers;

signals:
    void sendData(bool);
};

#endif // ADDCONTACTDIALOG_H
