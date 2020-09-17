#ifndef CALLHISTORYDIALOG_H
#define CALLHISTORYDIALOG_H

#include "AsteriskManager.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "EditContactDialog.h"
#include "EditOrgContactDialog.h"
#include "AddPhoneNumberToContactDialog.h"
#include "SettingsDialog.h"
#include "PlayAudioDialog.h"
#include "NotesDialog.h"
#include "OutCALL.h"
#include "Global.h"

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class CallHistoryDialog;
}

class CallHistoryDialog : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool);
    void receiveDataFromNotes();

public:
    explicit CallHistoryDialog(QWidget *parent = 0);
    ~CallHistoryDialog();

private slots:
    void loadAllCalls();
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();

    void onAddPhoneNumberToContact();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onAddContact();
    void onAddOrgContact();
    void onCallClicked();
    void onUpdate();
    void onUpdateClick();
    void updateCount();
    void tabSelected();
    void daysChanged();
    void clearSelections();
    void setPage();
    void disableButtons();
    void setHeadersNonClickable();

    void deleteObjects();

    void editContact(QString);
    void editOrgContact(QString);

    QString getUpdateId(QString);

    void addNote(const QModelIndex &index);
    void getData(const QModelIndex &index);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    bool isInternalPhone(QString* str);
    bool checkNumber(QString);

    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Ui::CallHistoryDialog *ui;

    QSqlDatabase db;
    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QSqlQueryModel* queryModel;

    QValidator* validator;

    QPointer<PlayAudioDialog> playAudioDialog;

    AddContactDialog* addContactDialog;
    AddOrgContactDialog* addOrgContactDialog;
    EditContactDialog* editContactDialog;
    EditOrgContactDialog* editOrgContactDialog;
    AddPhoneNumberToContactDialog* addPhoneNumberToContactDialog;
    NotesDialog* notesDialog;

    QModelIndexList selectionAll;
    QModelIndexList selectionMissed;
    QModelIndexList selectionReceived;
    QModelIndexList selectionPlaced;

    QString recordpath;
    QString state_call;
    QString page;
    QString pages;
    QString go;
    QString days;
    QString number;
    QString my_number;
    QString my_group;
    QString extfield;
    QString src;
    QString dst;
    QString uniqueid;
    QString dialogStatus;
    QString phone;
    int count;
    int remainder;

    QWidget* loadStatus();
    QWidget* loadNote();
    QWidget* loadName();

    QList<QSqlQueryModel*> queriesAll;
    QList<QSqlQueryModel*> queriesMissed;
    QList<QSqlQueryModel*> queriesReceived;
    QList<QSqlQueryModel*> queriesPlaced;

    QList<QHBoxLayout*> layoutsAllNotes;
    QList<QHBoxLayout*> layoutsMissedNotes;
    QList<QHBoxLayout*> layoutsReceivedNotes;
    QList<QHBoxLayout*> layoutsPlacedNotes;

    QList<QHBoxLayout*> layoutsAllName;
    QList<QHBoxLayout*> layoutsMissedName;
    QList<QHBoxLayout*> layoutsReceivedName;
    QList<QHBoxLayout*> layoutsPlacedName;
    QList<QHBoxLayout*> layoutsStatus;

    QList<QWidget*> widgetsStatus;

    QList<QWidget*> widgetsAllNotes;
    QList<QWidget*> widgetsMissedNotes;
    QList<QWidget*> widgetsReceivedNotes;
    QList<QWidget*> widgetsPlacedNotes;

    QList<QWidget*> widgetsAllName;
    QList<QWidget*> widgetsMissedName;
    QList<QWidget*> widgetsReceivedName;
    QList<QWidget*> widgetsPlacedName;

    QList<QWidget*> widgetsBusy;
    QList<QWidget*> widgetsCancel;

    QList<QLabel*> labelsAllName;
    QList<QLabel*> labelsMissedName;
    QList<QLabel*> labelsReceivedName;
    QList<QLabel*> labelsPlacedName;
    QList<QLabel*> labelsStatus;
    QList<QLabel*> notesAll;
    QList<QLabel*> notesMissed;
    QList<QLabel*> notesReceived;
    QList<QLabel*> notesPlaced;
};

#endif // CALLHISTORYDIALOG_H
