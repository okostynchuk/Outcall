#ifndef ADDREMINDERDIALOG_H
#define ADDREMINDERDIALOG_H

#include "Global.h"
#include "AsteriskManager.h"

#include <QDialog>

namespace Ui {
class AddReminderDialog;
}

class AddReminderDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public:
    explicit AddReminderDialog(QWidget *parent = nullptr);
    ~AddReminderDialog();
    void setCallId(QString);

protected slots:
    void onSave();
    bool eventFilter(QObject *object, QEvent *event);
    void onTextChanged();

private slots:
    void on_add10MinButton_clicked();
    void on_add30MinButton_clicked();
    void on_add5MinButton_clicked();
    void on_add60MinButton_clicked();

private:
    Ui::AddReminderDialog *ui;
    QString my_number;
    QString callId;
};

#endif // ADDREMINDERDIALOG_H
