#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include "Global.h"
#include "SettingsDialog.h"
#include "EditContactDialog.h"
#include "ChooseNumber.h"
#include "NotesDialog.h"
#include "AddReminderDialog.h"

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

protected slots:
    void onAddReminder();
    void onOpenAccess();
    void onCall();
    void onEdit();
    void loadAllCalls();
    void loadMissedCalls();
    void loadReceivedCalls();
    void loadPlacedCalls();
    void updateCount();
    void deleteObjects();
    void updateCalls();
    void viewMissedNotes(const QModelIndex &index);
    void viewRecievedNotes(const QModelIndex &index);
    void viewPlacedNotes(const QModelIndex &index);

private:
    Ui::ViewContactDialog *ui;
    AddReminderDialog *addReminderDialog;
    ChooseNumber *chooseNumber;
    EditContactDialog *editContactDialog;
    NotesDialog *notesDialog;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QSqlQueryModel *query3;
    QSqlQueryModel *query4;
    QValidator *validator;
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
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
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
    QList<QLabel*> labelsAllName;
    QList<QLabel*> labelsMissedName;
    QList<QLabel*> labelsReceivedName;
    QList<QLabel*> labelsPlacedName;
    QList<QLabel*> notes;
    QList<QLabel*> labelsStatus;
    QList<QSqlQueryModel*> queries;
};

#endif // VIEWCONTACTDIALOG_H
