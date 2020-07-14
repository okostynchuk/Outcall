#ifndef CALLHISTORYDIALOG_H
#define CALLHISTORYDIALOG_H

#include "AsteriskManager.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "EditContactDialog.h"
#include "EditOrgContactDialog.h"
#include "SettingsDialog.h"
#include "NotesDialog.h"
#include "OutCALL.h"
#include "Global.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <QTextEdit>
#include <QList>

namespace Ui {
class CallHistoryDialog;
}

class CallHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CallHistoryDialog(QWidget *parent = 0);
    ~CallHistoryDialog();
    enum Calls
    {
        MISSED = 0,
        RECIEVED = 1,
        PLACED = 2
    };
    void addCall(const QMap<QString, QVariant> &, Calls);

public slots:
    void receiveDataToAllCalls();
    void receiveDataToMissed();
    void receiveDataToReceived();
    void receiveDataToPlaced();

protected slots:
    void onAddContact();
    void onAddOrgContact();
    void onCallClicked();
    bool checkNumber(QString &);
    void onUpdate();
    void deleteObjects();
    void deleteNameObjects();
    void deleteObjectsOfAllCalls();
    void deleteMissedObjects();
    void deleteReceivedObjects();
    void deletePlacedObjects();
    void deleteMissedStatusObjects();
    void deleteBusyStatusObjects();
    void deleteCancelStatusObjects();
    void deleteReceivedStatusObjects();
    void editContact(QString &);
    void editOrgContact(QString &);
    QString getUpdateId(QString &);
    void addNotes(const QModelIndex &);
    void addNoteToMissed(const QModelIndex &);
    void addNoteToReceived(const QModelIndex &);
    void addNoteToPlaced(const QModelIndex &);
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void getNumber(const QModelIndex &index);
    void getNumberMissed(const QModelIndex &index);
    void getNumberReceived(const QModelIndex &index);
    void getNumberPlaced(const QModelIndex &index);
    void loadAllCalls();
    void tabSelected();

private slots:
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

protected:
   void CallHistoryDialog::showEvent(QShowEvent *);

private:
    Ui::CallHistoryDialog *ui;
    QSqlQuery query;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    SettingsDialog *settingsDialog;
    NotesDialog *notesDialog;
    QValidator *validator;
    QString days;
    QString number;
    QString my_number;
    QString my_group;
    QString extfield1;
    QString src;
    QString uniqueid;
    QString dialogStatus;
    QString callerNum;
    QString state_call;
    QString page;
    QString pages;
    QString go;
    int count;
    int remainder;
    int missed_count = 0;
    QWidget* loadMissedStatus();
    QWidget* loadBusyStatus();
    QWidget* loadCancelStatus();
    QWidget* loadReceivedStatus();
    QWidget* loadAllNotes();
    QWidget* loadMissedNote();
    QWidget* loadReceivedNote();
    QWidget* loadPlacedNote();
    QWidget* loadName();
    QList<QHBoxLayout*> layoutsName;
    QList<QHBoxLayout*> layoutsMissedStatus;
    QList<QHBoxLayout*> layoutsBusyStatus;
    QList<QHBoxLayout*> layoutsCancelStatus;
    QList<QHBoxLayout*> layoutsReceivedStatus;
    QList<QWidget*> widgetsMissedStatus;
    QList<QWidget*> widgetsBusyStatus;
    QList<QWidget*> widgetsCancelStatus;
    QList<QWidget*> widgetsReceivedStatus;
    QList<QWidget*> widgets;
    QList<QWidget*> widgetsName;
    QList<QWidget*> widgetsMissed;
    QList<QWidget*> widgetsBusy;
    QList<QWidget*> widgetsCancel;
    QList<QWidget*> widgetsReceived;
    QList<QWidget*> widgetsPlaced;
    QList<QLabel*> labelsName;
    QList<QLabel*> labelsMissedStatus;
    QList<QLabel*> labelsBusyStatus;
    QList<QLabel*> labelsCancelStatus;
    QList<QLabel*> labelsReceivedStatus;
    QList<QLabel*> notes;
    QList<QLabel*> notesMissed;
    QList<QLabel*> notesReceived;
    QList<QLabel*> notesPlaced;
};

#endif // CALLHISTORYDIALOG_H
