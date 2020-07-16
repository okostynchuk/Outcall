#ifndef EDITREMINDERDIALOG_H
#define EDITREMINDERDIALOG_H

#include <AsteriskManager.h>

#include <QDialog>
#include <QDateTime>

namespace Ui {
class EditReminderDialog;
}

class EditReminderDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);

public:
    explicit EditReminderDialog(QWidget *parent = 0);
    ~EditReminderDialog();
    void setValuesReminders(QString, QString, QDateTime, QString);

protected slots:
    void onSave();
    void onTextChanged();
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::EditReminderDialog *ui;
    QString my_number;
    QString selectedNumber;
    QString callId;
    QString id;
    QDateTime oldDateTime;
    QString oldNote;
};

#endif // EDITREMINDERDIALOG_H
