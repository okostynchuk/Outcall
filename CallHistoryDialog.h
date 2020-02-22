#ifndef CALLHISTORYDIALOG_H
#define CALLHISTORYDIALOG_H

#include "AsteriskManager.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "EditContactDialog.h"
#include "EditOrgContactDialog.h"
#include "SettingsDialog.h"
#include "AddNoteDialog.h"
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
    void deleteMissedObjects();
    void deleteReceivedObjects();
    void deletePlacedObjects();
    void editContact(QString &);
    void editOrgContact(QString &);
    QString getUpdateId(QString &);
    void addNoteToMissed(const QModelIndex &);
    void addNoteToReceived(const QModelIndex &);
    void addNoteToPlaced(const QModelIndex &);
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void getNumberMissed(const QModelIndex &index);
    void getNumberReceived(const QModelIndex &index);
    void getNumberPlaced(const QModelIndex &index);

protected:
   void CallHistoryDialog::showEvent(QShowEvent *);

private:
    Ui::CallHistoryDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    SettingsDialog *settingsDialog;
    AddNoteDialog *addNoteDialog;
    QString days;
    QString number;
    QString my_number;
    QString uniqueid;
    QString callerNum;
    int missed_count = 0;
    QString state_call;
    QWidget* loadMissedNote();
    QWidget* loadReceivedNote();
    QWidget* loadPlacedNote();
    QList<QWidget*> widgetsMissed;
    QList<QWidget*> widgetsReceived;
    QList<QWidget*> widgetsPlaced;
    QList<QLabel*> notesMissed;
    QList<QLabel*> notesReceived;
    QList<QLabel*> notesPlaced;
};

#endif // CALLHISTORYDIALOG_H
