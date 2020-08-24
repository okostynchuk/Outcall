#ifndef ADDREMINDERDIALOG_H
#define ADDREMINDERDIALOG_H

#include "Global.h"
#include "AsteriskManager.h"
#include "ChooseEmployee.h"

#include <QDialog>

namespace Ui {
class AddReminderDialog;
}

class AddReminderDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public slots:
    void receiveEmployee(QStringList);

public:
    explicit AddReminderDialog(QWidget *parent = nullptr);
    ~AddReminderDialog();

    void setCallId(QString);

private slots:
    void onChooseEmployee();
    void onSave();
    void onTextChanged();

    void on_add10MinButton_clicked();
    void on_add30MinButton_clicked();
    void on_add5MinButton_clicked();
    void on_add60MinButton_clicked();

    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::AddReminderDialog *ui;

    ChooseEmployee *chooseEmployee;

    QStringList employee;

    QString my_number;
    QString callId;
};

#endif // ADDREMINDERDIALOG_H
