#ifndef REMINDERSDIALOG_H
#define REMINDERSDIALOG_H

#include <QDialog>

namespace Ui {
class RemindersDialog;
}

class RemindersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();

protected:
    void onSave();

private:
    Ui::RemindersDialog *ui;
};

#endif // REMINDERSDIALOG_H
