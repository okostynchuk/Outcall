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
#include <QMap>
#include <QSqlQueryModel>
#include <QTreeWidget>

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
    void loadCalls(QString &);
    void clear();

protected slots:
    void onAddContact();
    void onAddOrgContact();
    void onRemoveButton();
    void onCallClicked();
    bool checkNumber(QString &);
    void onAddNotes();
    void editContact(QString &);
    QString getUpdateId(QString &);

private:
    Ui::CallHistoryDialog *ui;
    QSqlQuery *query;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    SettingsDialog *settingsDialog;
    AddNoteDialog *addNoteDialog;
    QString number;
    int missed_count = 0;
    QString state_call;
};

#endif // CALLHISTORYDIALOG_H
