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
    void reminders(bool);

public slots:
    void onUpdate();
    void sendNewValues();
    void receiveData(bool);

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();

    bool resizeColumns;

private slots:
    void loadRelevantReminders();
    void loadIrrelevantReminders();
    void loadDelegatedReminders();

    void onTimer();
    void onAddReminder();
    void onEditReminder(const QModelIndex &);
    void deleteObjects();
    void changeState();
    void onNotify(QString, QDateTime, QString);
    void onUpdateTab();

    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::RemindersDialog *ui;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QThread* remindersThread;
    RemindersThread* remindersThreadManager;
    AddReminderDialog* addReminderDialog;
    EditReminderDialog* editReminderDialog;

    QSqlQueryModelReminders *query1;
    QSqlQueryModelReminders *query2;

    QModelIndexList selectionRelevant;
    QModelIndexList selectionIrrelevant;
    QModelIndexList selectionDelegated;

    QTimer timer;
    QString languages;
    QString my_number;
    int oldReceivedReminders;

    QWidget* addWidgetActive();
    QWidget* addWidgetCompleted();
    QWidget* addWidgetContent(int, QString);
    QWidget* addCheckBoxViewed(int);
    QWidget* addCheckBoxCompleted(int);
    QWidget* addCheckBoxActive(int);

    QList<QSqlQueryModelReminders*> queriesRelevant;
    QList<QWidget*> widgetsRelevant;
    QList<QHBoxLayout*> layoutsRelevant;
    QList<QCheckBox*> boxesRelevant;
    QList<QLabel*> labelsRelevant;

    QList<QSqlQueryModelReminders*> queriesIrrelevant;
    QList<QWidget*> widgetsIrrelevant;
    QList<QHBoxLayout*> layoutsIrrelevant;
    QList<QCheckBox*> boxesIrrelevant;
    QList<QLabel*> labelsIrrelevant;

    QList<QSqlQueryModelReminders*> queriesDelegated;
    QList<QWidget*> widgetsDelegated;
    QList<QHBoxLayout*> layoutsDelegated;
    QList<QCheckBox*> boxesDelegated;
    QList<QLabel*> labelsDelegated;
};

#endif // REMINDERSDIALOG_H
