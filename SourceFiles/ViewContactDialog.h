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
#include <QWidget>
#include <QLineEdit>
#include <QPointer>

namespace Ui {
class ViewContactDialog;
}

class ViewContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool);
    void getPos(int, int);

public slots:
    void receiveData(bool, int, int);

public:
    explicit ViewContactDialog(QWidget *parent = 0);
    ~ViewContactDialog();

    void setValues(QString);

private slots:
    void loadCalls();

    void onAddReminder();
    void onOpenAccess();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onCall();
    void onEdit();
    void updateCount();
    void tabSelected();
    void daysChanged();

    void setPage();

    void deleteObjects();

    void viewNotes(const QModelIndex &index);

    void getData(const QModelIndex &index);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent*);

private:
    Ui::ViewContactDialog *ui;

    QSqlDatabase db;
    QSqlDatabase dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<AddReminderDialog> addReminderDialog;
    QPointer<ChooseNumber> chooseNumber;
    QPointer<PlayAudioDialog> playAudioDialog;

    EditContactDialog* editContactDialog;
    NotesDialog* notesDialog;

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

    QWidget* loadStatus(QString);
    QWidget* loadNote(QString);
    QWidget* loadName(QString, QString);

    QModelIndexList selections;

    QStringList numbersList;

    QList<QLineEdit*> phonesList;

    QList<QSqlQueryModel*> queries;

    QList<QHBoxLayout*> layouts;

    QList<QWidget*> widgets;

    QList<QLabel*> labels;
};

#endif // VIEWCONTACTDIALOG_H
