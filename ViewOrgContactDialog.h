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
#include <QLineEdit>

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
    void viewNotes(const QModelIndex &index);
    void onSectionClicked (int logicalIndex);
    void updateCount();
    void searchFunction();
    void tabSelected();
    void daysChanged();

    void deleteObjects();

    void getData(const QModelIndex &index);


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

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    ViewContactDialog *viewContactDialog;
    EditOrgContactDialog *editOrgContactDialog;
    ChooseNumber *chooseNumber;
    AddReminderDialog* addReminderDialog;
    NotesDialog *notesDialog;
    AddPersonToOrg *addPersonToOrg;
    PlayAudioDialog *playAudioDialog = nullptr;

    QList <QLineEdit *> phonesList;

    QSqlQueryModel *query_model;
    QSqlQueryModel *queryModel;

    QValidator *validator;

    QHeaderView *m_horiz_header;

    QString recordpath;
    QString userID;
    QString contactId;
    QString number;
    QString my_number;
    QString uniqueid;
    int count2 = 1;
    QString contact_number;
    QString page;
    QString pages;
    QString go;
    QString extfield;
    QString src;
    QString dst;
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
