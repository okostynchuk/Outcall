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
    void sendData(bool update);

public:
    explicit AddOrgContactDialog(QWidget* parent = 0);
    ~AddOrgContactDialog();

    void setValues(const QString& number);

private slots:
    void onSave();
    void onTextChanged();

    bool isPhone(QString* str);

    void keyPressEvent(QKeyEvent* event);

private:
    Ui::AddOrgContactDialog* ui;

    QSqlDatabase db;

    QList<QLineEdit*> phonesList;

    QValidator* validator;
};

#endif // ADDORGCONTACTDIALOG_H
