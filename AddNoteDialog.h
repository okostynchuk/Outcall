#ifndef ADDNOTEDIALOG_H
#define ADDNOTEDIALOG_H

#include <QDialog>

namespace Ui {
class AddNoteDialog;
}

class AddNoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNoteDialog(QWidget *parent = 0);
    void setCallId(QString &uniqueid, QString &state_call);
    ~AddNoteDialog();

protected:
    void onSave();
    void setNote();

private:
    Ui::AddNoteDialog *ui;
    QString callId;
    QString state;

signals:
    void sendDataToAllCalls();
    void sendDataToMissed();
    void sendDataToReceived();
    void sendDataToPlaced();
};

#endif // ADDNOTEDIALOG_H
