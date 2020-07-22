#ifndef REMINDERSDIALOG_H
#define REMINDERSDIALOG_H

#include "RemindersThread.h"
#include "AddReminderDialog.h"
#include "EditReminderDialog.h"
#include "AsteriskManager.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QCheckBox>
#include <QLayout>

namespace Ui {
class RemindersDialog;
}

class RemindersDialog : public QDialog
{
    Q_OBJECT

signals:
    void reminder(bool);

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();
     bool resizeColumns;

public slots:
    void onUpdate();
    void sendNewValues();
    void receiveData(bool);

protected:
    void loadActualReminders();
    void loadPastReminders();
    void showEvent(QShowEvent *event);

protected slots:
    void onTimer();
    void onAddReminder();
    void onEditReminder(const QModelIndex &);
    void deleteObjects();
    void changeState();
    void onNotify(QString, QDateTime, QString);

private:
    Ui::RemindersDialog *ui;
    QTimer timer;
    QString languages;
    QString my_number;
    int oldReceivedReminders;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    QThread* remindersThread;
    RemindersThread* remindersThreadManager;
    AddReminderDialog* addReminderDialog;
    EditReminderDialog* editReminderDialog;
    QWidget* addWidgetActive();
    QWidget* addWidgetCompleted();
    QWidget* addCheckBoxCompleted(int);
    QWidget* addCheckBoxActive(int);
    QList<QSqlQueryModel*> queriesActual;
    QList<QSqlQueryModel*> queriesPast;
    QList<QWidget*> widgetsActual;
    QList<QHBoxLayout*> layoutsActual;
    QList<QCheckBox*> boxesActual;
    QList<QWidget*> widgetsPast;
    QList<QHBoxLayout*> layoutsPast;
    QList<QCheckBox*> boxesPast;
};

#endif // REMINDERSDIALOG_H
