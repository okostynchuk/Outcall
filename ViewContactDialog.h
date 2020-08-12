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

signals:
    void sendData(bool);
    void sendNumber(QString &);
    void getPos(int, int);

public slots:
    void receiveData(bool, int, int);
    void receiveNumber(QString &);
    void playerClosed(bool);

public:
    explicit ViewContactDialog(QWidget *parent = 0);
    ~ViewContactDialog();

    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);

private slots:
    void loadAllCalls();
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();

    void onAddReminder();
    void onOpenAccess();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onCall();
    void onEdit();
    void onUpdate();
    void updateCount();
    void tabSelected();
    void daysChanged();

    void deleteObjects();

    void viewNotes(const QModelIndex &index);

    void getData(const QModelIndex &index);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent *event);

private:
    Ui::ViewContactDialog *ui;

    AddReminderDialog *addReminderDialog;
    ChooseNumber *chooseNumber;
    EditContactDialog *editContactDialog;
    NotesDialog *notesDialog;
    PlayAudioDialog *playAudioDialog = nullptr;

    QSqlQueryModel *queryModel;

    QValidator *validator;

    QString recordpath;
    QString userID;
    QString updateID;
    QString uniqueid;
    QString number;
    QString my_number;
    QString page;
    QString pages;
    QString go;
    QString extfield;
    QString src;
    QString dst;
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

    QList<QSqlQueryModel*> queriesAll;
    QList<QSqlQueryModel*> queriesMissed;
    QList<QSqlQueryModel*> queriesReceived;
    QList<QSqlQueryModel*> queriesPlaced;

    QList<QHBoxLayout*> layoutsStatus;

    QList<QHBoxLayout*> layoutsAllName;
    QList<QHBoxLayout*> layoutsMissedName;
    QList<QHBoxLayout*> layoutsReceivedName;
    QList<QHBoxLayout*> layoutsPlacedName;

    QList<QHBoxLayout*> layoutsAllNotes;
    QList<QHBoxLayout*> layoutsMissedNotes;
    QList<QHBoxLayout*> layoutsReceivedNotes;
    QList<QHBoxLayout*> layoutsPlacedNotes;

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

#endif // VIEWCONTACTDIALOG_H
