#ifndef REMINDERSDIALOG_H
#define REMINDERSDIALOG_H

#include "RemindersThreadManager.h"
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
    void reminders(bool change);

public slots:
    void sendValues();
    void receiveData(bool update);
    void showReminders(bool show);

    void loadReminders();

public:
    explicit RemindersDialog(QWidget* parent = 0);
    ~RemindersDialog();

    bool resizeCells;

private slots:
    void onTimer();
    void onAddReminder();
    void onEditReminder(const QModelIndex& index);
    void deleteObjects();
    void checkBoxStateChanged();
    void onNotify(const QString& id, const QDateTime& dateTime, const QString& note);
    void onUpdate();
    void updateCount();
    void onTabChanged();
    void clearSelections();

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::RemindersDialog* ui;

    QSqlDatabase db;
	
    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");
	
    QValidator* validator;

    QThread* remindersThread;
    RemindersThreadManager* remindersThreadManager;

    AddReminderDialog* addReminderDialog;
    EditReminderDialog* editReminderDialog;

    QPointer<QSqlQueryModelReminders> queryModel;

    bool showReminder;

    QTimer timer;

    QString my_number;
    QString page;
    QString go;

    qint32 oldActiveReminders;
    qint32 oldReceivedReminders;
    qint32 verticalScrollBar;
    qint32 horizontalScrollBar;

    QWidget* addWidgetActive();
    QWidget* addWidgetCompleted();
    QWidget* addWidgetContent(qint32 row_index, bool url);

    QWidget* addCheckBoxViewed(qint32 row_index);
    QWidget* addCheckBoxCompleted(qint32 row_index);
    QWidget* addCheckBoxActive(qint32 row_index);

    QWidget* addPushButtonGroup(qint32 row_index);

    QModelIndexList selections;

    QList<QWidget*> widgets;
};

#endif // REMINDERSDIALOG_H
