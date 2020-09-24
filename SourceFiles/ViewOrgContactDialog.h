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

    void setValues(QString);

private slots:
    void loadCalls();

    void onAddReminder();
    void onOpenAccess();
    void onUpdateEmployees();
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
    void callTabSelected();
    void daysChanged();

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

    QSqlQueryModel* query_model;
    QSqlQueryModel* queryModel;

    QValidator* validator;

    QString recordpath;
    QString userId;
    QString contactId;
    QString my_number;
    QString page;
    QString pages;
    QString go;
    QString days;

    int count;

    bool filter;

    QWidget* loadStatus(QString);
    QWidget* loadNote(QString);
    QWidget* loadName(QString, QString);

    QStringList numbersList;

    QModelIndexList selections;

    QList<QLineEdit*> phonesList;

    QList<QSqlQueryModel*> queries;

    QList<QHBoxLayout*> layouts;

    QList<QWidget*> widgets;

    QList<QLabel*> labels;
};

#endif // VIEWORGCONTACTDIALOG_H
