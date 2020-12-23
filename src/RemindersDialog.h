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

    QSqlDatabase m_db;
	
    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QThread* m_remindersThread;
    RemindersThreadManager* m_remindersThreadManager;

    AddReminderDialog* m_addReminderDialog;
    EditReminderDialog* m_editReminderDialog;

    QPointer<QSqlQueryModelReminders> m_queryModel;

    bool m_showReminder;

    QTimer m_timer;

    QString m_page;
    QString m_go;

    qint32 m_oldActiveReminders;
    qint32 m_oldReceivedReminders;
    qint32 m_verticalScrollBar;
    qint32 m_horizontalScrollBar;

    QWidget* addWidgetActive();
    QWidget* addWidgetCompleted();
    QWidget* addWidgetContent(qint32 row_index, bool url);

    QWidget* addCheckBoxViewed(qint32 row_index);
    QWidget* addCheckBoxCompleted(qint32 row_index);
    QWidget* addCheckBoxActive(qint32 row_index);

    QWidget* addPushButtonGroup(qint32 row_index);

    QModelIndexList m_selections;

    QList<QWidget*> m_widgets;

    QByteArray m_geometry;
};

#endif // REMINDERSDIALOG_H
