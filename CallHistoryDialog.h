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
#include <QTableView>
#include <QTextEdit>
#include <QList>

namespace Ui {
class CallHistoryDialog;
}

class CallHistoryDialog : public QDialog
{
    Q_OBJECT

public slots:
    void receiveData(bool);
    void playerClosed(bool);
    void receiveDataToAllCalls();
    void receiveDataToMissed();
    void receiveDataToReceived();
    void receiveDataToPlaced();

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

    void getDataAll(const QModelIndex &index);
    void getDataMissed(const QModelIndex &index);
    void getDataReceived(const QModelIndex &index);
    void getDataPlaced(const QModelIndex &index);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    bool isInnerPhone(QString *str);
    bool checkNumber(QString &);

    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::CallHistoryDialog *ui;

    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;

    QValidator *validator;

    PlayAudioDialog *playAudioDialog = nullptr;
    AddContactDialog *addContactDialog;
    AddOrgContactDialog *addOrgContactDialog;
    EditContactDialog *editContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    AddPhoneNumberToContactDialog *addPhoneNumberToContactDialog;
    NotesDialog *notesDialog;

    QString recordpath;
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
    int remainder;

    QWidget* loadStatus();
    QWidget* loadAllNotes();
    QWidget* loadMissedNote();
    QWidget* loadReceivedNote();
    QWidget* loadPlacedNote();
    QWidget* loadName();

    QList<QHBoxLayout*> layoutsAllName;
    QList<QHBoxLayout*> layoutsMissedName;
    QList<QHBoxLayout*> layoutsReceivedName;
    QList<QHBoxLayout*> layoutsPlacedName;
    QList<QHBoxLayout*> layoutsStatus;

    QList<QWidget*> widgetsStatus;
    QList<QWidget*> widgets;
    QList<QWidget*> widgetsAllName;
    QList<QWidget*> widgetsMissedName;
    QList<QWidget*> widgetsReceivedName;
    QList<QWidget*> widgetsPlacedName;
    QList<QWidget*> widgetsMissed;
    QList<QWidget*> widgetsBusy;
    QList<QWidget*> widgetsCancel;
    QList<QWidget*> widgetsReceived;
    QList<QWidget*> widgetsPlaced;

    QList<QLabel*> labelsAllName;
    QList<QLabel*> labelsMissedName;
    QList<QLabel*> labelsReceivedName;
    QList<QLabel*> labelsPlacedName;
    QList<QLabel*> labelsStatus;
    QList<QLabel*> notes;
    QList<QLabel*> notesMissed;
    QList<QLabel*> notesReceived;
    QList<QLabel*> notesPlaced;
};

#endif // CALLHISTORYDIALOG_H
