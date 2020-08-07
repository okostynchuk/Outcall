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
#include <QScrollBar>
#include <QTableView>
#include <QList>
#include <QWidget>

namespace Ui {
class ViewOrgContactDialog;
}

class ViewOrgContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);
    void sendNumber(QString &);
    void getPos(int, int);

public slots:
    void receiveDataPerson(bool);
    void receiveDataOrg(bool, int, int);
    void receiveNumber(QString &);
    void receivePersonID(QString &);
    void playerClosed(bool);

public:
    explicit ViewOrgContactDialog(QWidget *parent = 0);
    ~ViewOrgContactDialog();

    void setOrgValuesContacts(QString &);

protected slots:


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
    void onComboBoxSelected();
    void showCard(const QModelIndex &index);
    void viewAllNotes(const QModelIndex &index);
    void viewMissedNotes(const QModelIndex &index);
    void viewRecievedNotes(const QModelIndex &index);
    void viewPlacedNotes(const QModelIndex &index);
    void onSectionClicked (int logicalIndex);
    void updateCount();
    void searchFunction();
    void tabSelected();
    void daysChanged();

    void deleteNotesObjects();
    void deleteStatusObjects();
    void deleteNameObjects();

    void getDataAll(const QModelIndex &index);
    void getDataMissed();
    void getDataReceived(const QModelIndex &index);
    void getDataPlaced(const QModelIndex &index);

    void on_lineEdit_returnPressed();
    void on_pushButton_clicked();
    void on_addPersonToOrg_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

private:
    Ui::ViewOrgContactDialog *ui;

    ViewContactDialog *viewContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    ChooseNumber *chooseNumber;
    AddReminderDialog* addReminderDialog;
    NotesDialog *notesDialog;
    AddPersonToOrg *addPersonToOrg;
    PlayAudioDialog *playAudioDialog = nullptr;

    QSqlQueryModel *query_model;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;

    QValidator *validator;

    QHeaderView *m_horiz_header;

    QString recordpath;
    QString userID;
    QString updateID;
    QString number;
    QString my_number;
    QString uniqueid;
    int count2 = 1;
    QString contact_number;
    QString page;
    QString pages;
    QString go;
    QString extfield1;
    QString src;
    QString dialogStatus;
    QString update;
    QString id;
    QString days;
    int count;
    int countNumbers = 1;
    int remainder;
    bool filter;
    int valueV;
    int valueH;
    QString state_call;
    QString entry_name;
    QString entry_phone;
    QString entry_comment;
    QString sort;
    QStringList numbersList;

    QWidget* loadNote();
    QWidget* loadStatus();
    QWidget* loadName();

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
    QList<QWidget*> widgets;
    QList<QWidget*> widgetsMissed;
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

#endif // VIEWORGCONTACTDIALOG_H
