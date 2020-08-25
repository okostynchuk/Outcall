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
    void sendNewValues();
    void receiveData(bool);
    void showReminders(bool);

    void onUpdate();

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();

    bool resizeCells;

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
    void onTabChanged();

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::RemindersDialog *ui;
	
    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");
	
    QValidator* validator;

    QThread* remindersThread;
    RemindersThread* remindersThreadManager;

    AddReminderDialog* addReminderDialog;
    EditReminderDialog* editReminderDialog;

    QSqlQueryModelReminders* queryModel;

    QModelIndexList selectionRelevant;
    QModelIndexList selectionIrrelevant;
    QModelIndexList selectionDelegated;

    bool showReminder;
    QTimer timer;
    QString languages;
    QString my_number;
    QString page;
    QString pages;
    QString go;
    int count;
    int remainder;
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
