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
#include "Player.h"

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
    void sendData(bool update);
    void getPos(qint32 x, qint32 y);

public slots:
    void receiveDataPerson(bool update);
    void receiveDataOrg(bool update, qint32 x, qint32 y);

public:
    explicit ViewOrgContactDialog(QWidget* parent = 0);
    ~ViewOrgContactDialog();

    void setValues(const QString& id);

private slots:
    void loadCalls();
    void loadEmployees();

    void setPage();
    void onAddReminder();
    void onOpenAccess();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onCall();
    void onEdit();
    void showCard(const QModelIndex& index);
    void viewNotes(const QModelIndex& index);
    void updateCount();
    void searchFunction();
    void tabSelected();
    void callTabSelected();
    void onUpdate();

    void deleteObjects();

    void getData(const QModelIndex& index);

    void on_lineEdit_returnPressed();
    void on_searchButton_clicked();
    void on_addPersonToOrg_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

private:
    Ui::ViewOrgContactDialog* ui;

    QSqlDatabase db;
    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<QSqlQueryModel> query_model;
    QPointer<QSqlQueryModel> queryModel;

    QPointer<AddPersonToOrg> addPersonToOrg;
    QPointer<Player> player;
    QPointer<ChooseNumber> chooseNumber;
    QPointer<AddReminderDialog> addReminderDialog;

    ViewContactDialog* viewContactDialog;
    NotesDialog* notesDialog;
    EditOrgContactDialog* editOrgContactDialog;

    QString recordpath;
    QString userId;
    QString contactId;
    QString my_number;
    QString go;
    QString page;

    qint32 countRecords;

    bool filter;

    QWidget* loadStatus(const QString& dialogStatus);
    QWidget* loadNote(const QString& uniqueid);
    QWidget* loadName(const QString& src, const QString& dst);

    QStringList numbersList;

    QModelIndexList selections;

    QList<QLineEdit*> phonesList;
    QList<QWidget*> widgets;

    QMap<QString, QLineEdit*> employeesPhonesList;
};

#endif // VIEWORGCONTACTDIALOG_H
