#ifndef CALLHISTORYDIALOG_H
#define CALLHISTORYDIALOG_H

#include "AsteriskManager.h"
#include "AddContactDialog.h"
#include "AddOrgContactDialog.h"
#include "EditContactDialog.h"
#include "EditOrgContactDialog.h"
#include "AddPhoneNumberToContactDialog.h"
#include "SettingsDialog.h"
#include "Player.h"
#include "NotesDialog.h"
#include "Outcall.h"
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
    void receiveData(bool update);

public:
    explicit CallHistoryDialog(QWidget* parent = 0);
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

    void editContact(const QString& number);
    void editOrgContact(const QString& number);

    QString getUpdateId(const QString& number);

    void addNote(const QModelIndex& index);
    void getData(const QModelIndex& index);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    bool isInternalPhone(QString* str);
    bool checkNumber(const QString& number);

    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::CallHistoryDialog* ui;

    QSqlDatabase m_db;
    QSqlDatabase m_dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<QSqlQueryModel> m_queryModel;
    QPointer<Player> m_player;

    AddContactDialog* m_addContactDialog;
    AddOrgContactDialog* m_addOrgContactDialog;
    EditContactDialog* m_editContactDialog;
    EditOrgContactDialog* m_editOrgContactDialog;
    AddPhoneNumberToContactDialog* m_addPhoneNumberToContactDialog;
    NotesDialog* m_notesDialog;

    QString m_recordpath;
    QString m_go;
    QString m_number;
    QString my_number;
    QString my_group;
    QString m_page;

    qint32 m_countRecords;

    QWidget* loadStatus(const QString& dialogStatus);
    QWidget* loadNote(const QString& uniqueid);
    QWidget* loadName(const QString& src, const QString& dst);

    QModelIndexList m_selections;

    QList<QWidget*> m_widgets;

    QByteArray m_geometry;
};

#endif // CALLHISTORYDIALOG_H
