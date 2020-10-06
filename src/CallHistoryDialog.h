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

public:
    explicit CallHistoryDialog(QWidget *parent = 0);
    ~CallHistoryDialog();

private slots:
    void loadCalls();
    void setPage();

    void onAddPhoneNumberToContact();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onAddContact();
    void onAddOrgContact();
    void onCallClicked();
    void onUpdate();
    void updateDefault();

    void updateCount();
    void tabSelected();
    void clearSelections();
    void disableButtons();

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

    QString recordpath;
    QString go;
    QString number;
    QString my_number;
    QString my_group;
    QString page;

    qint32 countRecords;

    QWidget* loadStatus(QString);
    QWidget* loadNote(QString);
    QWidget* loadName(QString, QString);

    QModelIndexList selections;

    QList<QSqlQueryModel*> queries;

    QList<QHBoxLayout*> layouts;

    QList<QWidget*> widgets;

    QList<QLabel*> labels;
};

#endif // CALLHISTORYDIALOG_H
