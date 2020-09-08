#ifndef VIEWORGCONTACTDIALOG_H
#define VIEWORGCONTACTDIALOG_H

#include "ViewContactDialog.h"
#include "EditOrgContactDialog.h"
#include "ChooseNumber.h"
#include "SettingsDialog.h"
#include "Global.h"
#include "AddPersonToOrg.h"
#include "NotesDialog.h"
#include "AddReminderDialog.h"
#include "PlayAudioDialog.h"

#include <QHeaderView>
#include <QSqlQueryModel>
#include <QWidget>
#include <QLineEdit>
#include <QPointer>
#include <QSqlDatabase>

namespace Ui {
class ViewOrgContactDialog;
}

class ViewOrgContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);
    void getPos(int, int);

public slots:
    void receiveDataPerson(bool);
    void receiveDataOrg(bool, int, int);

public:
    explicit ViewOrgContactDialog(QWidget *parent = 0);
    ~ViewOrgContactDialog();

    void setOrgValuesContacts(QString);

private slots:
    void loadAllCalls();
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();

    void onAddReminder();
    void onOpenAccess();
    void onUpdate();
    void onUpdateCalls();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onCall();
    void onEdit();
    void showCard(const QModelIndex &index);
    void viewNotes(const QModelIndex &index);
    void updateCount();
    void searchFunction();
    void tabSelected();
    void daysChanged();
    void setHeadersNonClickable();

    void deleteObjects();

    void getData(const QModelIndex &index);

    void on_lineEdit_returnPressed();
    void on_searchButton_clicked();
    void on_addPersonToOrg_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

private:
    Ui::ViewOrgContactDialog *ui;

    QSqlDatabase db;
    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<AddPersonToOrg> addPersonToOrg;
    QPointer<PlayAudioDialog> playAudioDialog;
    QPointer<ChooseNumber> chooseNumber;
    QPointer<AddReminderDialog> addReminderDialog;

    ViewContactDialog* viewContactDialog;
    NotesDialog* notesDialog;
    EditOrgContactDialog* editOrgContactDialog;

    QList<QLineEdit*> phonesList;

    QSqlQueryModel* query_model;
    QSqlQueryModel* queryModel;

    QValidator* validator;

    QString recordpath;
    QString userID;
    QString contactId;
    QString my_number;
    QString uniqueid;
    QString page;
    QString pages;
    QString go;
    QString extfield;
    QString src;
    QString dst;
    QString dialogStatus;
    QString update;
    QString days;
    int count;
    int remainder;
    bool filter;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    QString sort;
    QStringList numbersList;

    QWidget* loadNote();
    QWidget* loadStatus();
    QWidget* loadName();

    QList<QSqlQueryModel*> queriesAll;
    QList<QSqlQueryModel*> queriesMissed;
    QList<QSqlQueryModel*> queriesReceived;
    QList<QSqlQueryModel*> queriesPlaced;

    QList<QHBoxLayout*> layoutsAllNotes;
    QList<QHBoxLayout*> layoutsMissedNotes;
    QList<QHBoxLayout*> layoutsReceivedNotes;
    QList<QHBoxLayout*> layoutsPlacedNotes;

    QList<QHBoxLayout*> layoutsStatus;
    QList<QHBoxLayout*> layoutsAllName;
    QList<QHBoxLayout*> layoutsMissedName;
    QList<QHBoxLayout*> layoutsReceivedName;
    QList<QHBoxLayout*> layoutsPlacedName;

    QList<QWidget*> widgetsAllName;
    QList<QWidget*> widgetsMissedName;
    QList<QWidget*> widgetsReceivedName;
    QList<QWidget*> widgetsPlacedName;

    QList<QWidget*> widgetsStatus;

    QList<QWidget*> widgetsAllNotes;
    QList<QWidget*> widgetsMissedNotes;
    QList<QWidget*> widgetsReceivedNotes;
    QList<QWidget*> widgetsPlacedNotes;

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

#endif // VIEWORGCONTACTDIALOG_H
