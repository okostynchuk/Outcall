#ifndef EDITREMINDERDIALOG_H
#define EDITREMINDERDIALOG_H

#include "AsteriskManager.h"
#include "Global.h"

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

    void setValuesReminders(QString, QDateTime, QString);

private slots:
    void onSave();
    void onTextChanged();

    void on_add5MinButton_clicked();
    void on_add10MinButton_clicked();
    void on_add30MinButton_clicked();
    void on_add60MinButton_clicked();

    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::EditReminderDialog *ui;

    QString my_number;
    QString phone_from;
    QString phone_to;
    QString callId;
    QString id;
    QDateTime oldDateTime;
    QString oldNote;
};

#endif // EDITREMINDERDIALOG_H
