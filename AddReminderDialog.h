#ifndef ADDREMINDERDIALOG_H
#define ADDREMINDERDIALOG_H

#include <QDialog>

namespace Ui {
class AddReminderDialog;
}

class AddReminderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddReminderDialog(QWidget *parent = nullptr);
    ~AddReminderDialog();

private:
    Ui::AddReminderDialog *ui;
};

#endif // ADDREMINDERDIALOG_H
