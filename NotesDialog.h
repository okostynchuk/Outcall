#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include "SettingsDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <QLayout>
#include <QLabel>

namespace Ui {
class NotesDialog;
}

class NotesDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendDataToAllCalls();
    void sendDataToMissed();
    void sendDataToReceived();
    void sendDataToPlaced();

public:
    explicit NotesDialog(QWidget *parent = 0);
    ~NotesDialog();

    void setCallId(QString uniqueid, QString state_call);

private slots:
    void onSave();
    void onTextChanged();
    void onUpdate();
    void loadNotes();
    void deleteObjects();

    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::NotesDialog *ui;

    QSqlQueryModel *query;

    QWidget* addWidgetNote(int);

    QList<QWidget*> widgets;
    QList<QHBoxLayout*> layouts;
    QList<QLabel*> labels;

    QString callId;
    QString state;
    QString my_number;
};

#endif // NOTESDIALOG_H
