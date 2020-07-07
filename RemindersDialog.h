#ifndef REMINDERSDIALOG_H
#define REMINDERSDIALOG_H

#include "SettingsDialog.h"
#include "RemindersThread.h"
#include "EditReminderDialog.h"

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

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();

public slots:
    void onUpdate();
    void sendNewValues();
    void receiveData(bool);

protected:
    void loadActualReminders();
    void loadPastReminders();
    void showEvent(QShowEvent *event);

protected slots:
    void onEditReminder(const QModelIndex &);
    void onTextChanged();
    void deleteObjects();
    void changeState();
    void onSave();
    bool eventFilter(QObject *object, QEvent *event);
    void onNotify(QString reminderId, QDateTime reminderDateTime, QString reminderNote);

private:
    Ui::RemindersDialog *ui;
    QString my_number;
    SettingsDialog *settingsDialog;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    QThread* remindersThread;
    RemindersThread* remindersThreadManager;
    EditReminderDialog* editReminderDialog;
    QWidget* addCheckBox(int);
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
