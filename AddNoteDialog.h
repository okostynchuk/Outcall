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
    void setCallId(QString &uniqueid);
    ~AddNoteDialog();

protected:
    void onSave();

private:
    Ui::AddNoteDialog *ui;
    QString callId;
};

#endif // ADDNOTEDIALOG_H
