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
#include <QHBoxLayout>
#include <QScrollBar>

namespace Ui {
class RemindersDialog;
}

class RemindersDialog : public QDialog
{
    Q_OBJECT

signals:
    void reminders(bool);

public slots:
    void sendValues();
    void receiveData(bool);
    void showReminders(bool);

    void loadReminders();

public:
    explicit RemindersDialog(QWidget *parent = 0);
    ~RemindersDialog();

    bool resizeCells;

private slots:
    void onTimer();
    void onAddReminder();
    void onEditReminder(const QModelIndex &);
    void deleteObjects();
    void changeState();
    void onNotify(QString, QDateTime, QString);
    void onUpdateTab();
    void updateCount();
    void onTabChanged();
    void clearSelections();

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Ui::RemindersDialog *ui;

    QSqlDatabase db;
	
    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");
	
    QValidator* validator;

    QThread* remindersThread;
    RemindersThread* remindersThreadManager;

    AddReminderDialog* addReminderDialog;
    EditReminderDialog* editReminderDialog;

    QSqlQueryModelReminders* queryModel;

    bool showReminder;
    QTimer timer;
    QString my_number;
    QString page;
    QString go;
    int oldActiveReminders;
    int oldReceivedReminders;
    int verticalScrollBar;
    int horizontalScrollBar;

    QWidget* addWidgetActive();
    QWidget* addWidgetCompleted();
    QWidget* addWidgetContent(int, bool);

    QWidget* addCheckBoxViewed(int);
    QWidget* addCheckBoxCompleted(int);
    QWidget* addCheckBoxActive(int);

    QModelIndexList selections;

    QList<QSqlQueryModelReminders*> queries;

    QList<QWidget*> widgets;
    QList<QHBoxLayout*> layouts;
    QList<QCheckBox*> boxes;
    QList<QLabel*> labels;
};

#endif // REMINDERSDIALOG_H
