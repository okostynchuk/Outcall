#ifndef ADDORGCONTACTDIALOG_H
#define ADDORGCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QSqlQuery>
#include <QLineEdit>
#include <QKeyEvent>

namespace Ui {
class AddOrgContactDialog;
}

class AddOrgContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public:
    explicit AddOrgContactDialog(QWidget *parent = 0);
    ~AddOrgContactDialog();

    void setOrgValuesCallHistory(QString);
    void setOrgValuesPopupWindow(QString);

private slots:
    void onSave();
    void onTextChanged();

    bool isInternalPhone(QString* str);
    bool isPhone(QString* str);
    bool isVyborID(QString* str);

    void keyPressEvent(QKeyEvent*);

private:
    Ui::AddOrgContactDialog *ui;

    QList<QLineEdit*> phonesList;

    QValidator* validator;

    QStringList numbers;
};

#endif // ADDORGCONTACTDIALOG_H
