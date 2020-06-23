#ifndef REMINDERSDIALOG_H
#define REMINDERSDIALOG_H

#include <QDialog>

namespace Ui {
class RemindersDialog;
}

class RemindersDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();

public slots:
    void receiveData(bool);

protected:
    void onSave();

private:
    Ui::RemindersDialog *ui;
};

#endif // REMINDERSDIALOG_H
