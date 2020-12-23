#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include "Global.h"
#include "SettingsDialog.h"
#include "EditContactDialog.h"
#include "ChooseNumber.h"
#include "NotesDialog.h"
#include "AddReminderDialog.h"
#include "Player.h"

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
    void sendData(bool update);
    void getPos(qint32 x, qint32 y);

public slots:
    void receiveData(bool update, qint32 x, qint32 y);

public:
    explicit ViewContactDialog(QWidget* parent = 0);
    ~ViewContactDialog();

    void setValues(const QString& id);

private slots:
    void loadCalls();

    void onAddReminder();
    void onOpenAccess();
    void onPlayAudio();
    void onPlayAudioPhone();
    void onCall();
    void onEdit();
    void updateCount();
    void onUpdate();
    void tabSelected();

    void setPage();

    void deleteObjects();

    void viewNotes(const QModelIndex& index);
    void getData(const QModelIndex& index);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent* event);

private:
    Ui::ViewContactDialog* ui;

    QSqlDatabase m_db;
    QSqlDatabase m_dbCalls = QSqlDatabase::database("Calls");

    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<AddReminderDialog> m_addReminderDialog;
    QPointer<ChooseNumber> m_chooseNumber;
    QPointer<Player> m_player;

    EditContactDialog* m_editContactDialog;
    NotesDialog* m_notesDialog;

    QPointer<QSqlQueryModel> m_queryModel;

    QString m_recordpath;
    QString m_contactId;
    QString m_go;
    QString m_page;

    qint32 m_countRecords;

    QWidget* loadStatus(const QString& dialogStatus);
    QWidget* loadNote(const QString& uniqueid);
    QWidget* loadName(const QString& src, const QString& dst);

    QStringList m_numbers;

    QMap<QString, QLineEdit*> m_managers;

    QList<QLineEdit*> m_phones;

    QList<QWidget*> m_widgets;
};

#endif // VIEWCONTACTDIALOG_H
