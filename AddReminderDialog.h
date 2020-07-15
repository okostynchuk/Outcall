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

protected slots:
    void onSave();
    bool eventFilter(QObject *object, QEvent *event);
    void onTextChanged();

private:
    Ui::AddReminderDialog *ui;
    QString my_number;
};

#endif // ADDREMINDERDIALOG_H
