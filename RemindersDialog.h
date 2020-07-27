#ifndef REMINDERSDIALOG_H
#define REMINDERSDIALOG_H

#include "RemindersThread.h"
#include "AddReminderDialog.h"
#include "EditReminderDialog.h"
#include "AsteriskManager.h"
#include "QSqlQueryModelReminders.h"

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
    void loadRelevantReminders();
    void loadIrrelevantReminders();
    void loadDelegatedReminders();
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

    QSqlQueryModelReminders *query1;
    QSqlQueryModelReminders *query2;

    QThread* remindersThread;
    RemindersThread* remindersThreadManager;
    AddReminderDialog* addReminderDialog;
    EditReminderDialog* editReminderDialog;

    QWidget* addWidgetActive();
    QWidget* addWidgetCompleted();
    QWidget* addCheckBoxViewed(int);
    QWidget* addCheckBoxCompleted(int);
    QWidget* addCheckBoxActive(int);

    QList<QSqlQueryModel*> queriesRelevant;
    QList<QWidget*> widgetsRelevant;
    QList<QHBoxLayout*> layoutsRelevant;
    QList<QCheckBox*> boxesRelevant;

    QList<QSqlQueryModel*> queriesIrrelevant;
    QList<QWidget*> widgetsIrrelevant;
    QList<QHBoxLayout*> layoutsIrrelevant;
    QList<QCheckBox*> boxesIrrelevant;

    QList<QSqlQueryModel*> queriesDelegated;
    QList<QWidget*> widgetsDelegated;
    QList<QHBoxLayout*> layoutsDelegated;
    QList<QCheckBox*> boxesDelegated;
};

#endif // REMINDERSDIALOG_H
