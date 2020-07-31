#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include "Global.h"
#include "SettingsDialog.h"
#include "EditContactDialog.h"
#include "ChooseNumber.h"
#include "NotesDialog.h"
#include "AddReminderDialog.h"
#include "PlayAudioDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <QList>
#include <QWidget>

namespace Ui {
class ViewContactDialog;
}

class ViewContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewContactDialog(QWidget *parent = 0);
    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);
    ~ViewContactDialog();

signals:
    void sendData(bool);
    void sendNumber(QString &);

public slots:
    void receiveData(bool);
    void receiveNumber(QString &);
    void playerClosed(bool);

protected slots:
    void onAddReminder();
    void onOpenAccess();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onCall();
    void onEdit();
    void onUpdate();
    void loadAllCalls();
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void updateCount();
    void deleteNotesObjects();
    void deleteStatusObjects();
    void deleteNameObjects();
    void viewMissedNotes(const QModelIndex &index);
    void viewRecievedNotes(const QModelIndex &index);
    void viewPlacedNotes(const QModelIndex &index);
    void viewAllNotes(const QModelIndex &index);
    void getDataAll(const QModelIndex &index);
    void getDataMissed();
    void getDataReceived(const QModelIndex &index);
    void getDataPlaced(const QModelIndex &index);
    void tabSelected();
    void daysChanged();

private slots:
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

private:
    Ui::ViewContactDialog *ui;
    AddReminderDialog *addReminderDialog;
    ChooseNumber *chooseNumber;
    EditContactDialog *editContactDialog;
    NotesDialog *notesDialog;
    PlayAudioDialog *playAudioDialog = nullptr;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    QValidator *validator;
    QString recordpath;
    QString userID;
    QString updateID;
    QString uniqueid;
    QString number;
    QString my_number;
    QString my_group;
    QString page;
    QString pages;
    QString go;
    QString extfield1;
    QString src;
    QString dialogStatus;
    int count;
    int countNumbers = 1;
    int remainder;
    QString days;
    QString state_call;
    QString contact_number;
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

#endif // VIEWCONTACTDIALOG_H
