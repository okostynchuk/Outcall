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

    QSqlDatabase m_db;
    QSqlDatabase m_dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<QSqlQueryModel> query_model;
    QPointer<QSqlQueryModel> m_queryModel;

    QPointer<AddPersonToOrg> addPersonToOrg;
    QPointer<Player> m_player;
    QPointer<ChooseNumber> m_chooseNumber;
    QPointer<AddReminderDialog> m_addReminderDialog;

    ViewContactDialog* m_viewContactDialog;
    NotesDialog* m_notesDialog;
    EditOrgContactDialog* m_editOrgContactDialog;

    QString m_recordpath;
    QString m_contactId;
    QString m_go;
    QString m_page;

    qint32 m_countRecords;

    bool m_filter;

    QWidget* loadStatus(const QString& dialogStatus);
    QWidget* loadNote(const QString& uniqueid);
    QWidget* loadName(const QString& src, const QString& dst);

    QStringList m_numbers;
    QStringList m_comments;

    QMap<QString, QLineEdit*> m_managers;

    QList<QLineEdit*> m_phones;
    QList<QLineEdit*> m_phonesComments;

    QList<QWidget*> m_widgets;
};

#endif // VIEWORGCONTACTDIALOG_H
