#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>

namespace Ui {
class NotesDialog;
}

class NotesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotesDialog(QWidget *parent = 0);
     void setCallId(QString &uniqueid, QString &state_call);
    ~NotesDialog();


protected:
    void onSave();
    void setNote();
    void loadNotes();

private:
    Ui::NotesDialog *ui;
    QString callId;
    QString state;
    QSqlQueryModel *query;

signals:
    void sendDataToAllCalls();
    void sendDataToMissed();
    void sendDataToReceived();
    void sendDataToPlaced();
};

#endif // NOTESDIALOG_H
