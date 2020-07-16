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
    void onUpdateClick();
    void updateCount();
    void deleteObjects();
    void deleteNameObjects();
    void deleteObjectsOfAllCalls();
    void deleteMissedObjects();
    void deleteReceivedObjects();
    void deletePlacedObjects();
    void deleteStatusObjects();
    void editContact(QString &);
    void editOrgContact(QString &);
    QString getUpdateId(QString &);
    void addNotes(const QModelIndex &);
    void addNoteToMissed(const QModelIndex &);
    void addNoteToReceived(const QModelIndex &);
    void addNoteToPlaced(const QModelIndex &);
    void loadAllCalls();
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void getNumber(const QModelIndex &index);
    void getNumberMissed(const QModelIndex &index);
    void getNumberReceived(const QModelIndex &index);
    void getNumberPlaced(const QModelIndex &index);
    void tabSelected();
    void daysChanged();

protected:
   void showEvent(QShowEvent *);
   void closeEvent(QCloseEvent *);

private slots:
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

private:
    Ui::CallHistoryDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    QValidator *validator;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    NotesDialog *notesDialog;
    QString state_call;
    QString page;
    QString pages;
    QString go;
    QString days;
    QString number;
    QString my_number;
    QString my_group;
    QString extfield1;
    QString src;
    QString uniqueid;
    QString dialogStatus;
    QString callerNum;
    int count;
    int i=0;
    int remainder;
    int missed_count = 0;
    QWidget* loadStatus(QString &);
    QWidget* loadAllNotes();
    QWidget* loadMissedNote();
    QWidget* loadReceivedNote();
    QWidget* loadPlacedNote();
    QWidget* loadName();
    QList<QHBoxLayout*> layoutsName;
    QList<QHBoxLayout*> layoutsStatus;
    QList<QWidget*> widgetsStatus;
    QList<QWidget*> widgets;
    QList<QWidget*> widgetsName;
    QList<QWidget*> widgetsMissed;
    QList<QWidget*> widgetsBusy;
    QList<QWidget*> widgetsCancel;
    QList<QWidget*> widgetsReceived;
    QList<QWidget*> widgetsPlaced;
    QList<QLabel*> labelsName;
    QList<QLabel*> labelsStatus;
    QList<QLabel*> notes;
    QList<QLabel*> notesMissed;
    QList<QLabel*> notesReceived;
    QList<QLabel*> notesPlaced;
};

#endif // CALLHISTORYDIALOG_H
