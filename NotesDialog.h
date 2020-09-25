#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include "SettingsDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QRegularExpression>
#include <QValidator>
#include <QSqlDatabase>

namespace Ui {
class NotesDialog;
}

class NotesDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData();

public:
    explicit NotesDialog(QWidget *parent = 0);
    ~NotesDialog();

    void setValues(QString uniqueid, QString phone);
    void hideAddNote();

private slots:
    void onSave();
    void onTextChanged();
    void onUpdate();
    void loadNotes();
    void deleteObjects();

    bool isInternalPhone(QString* str);

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void keyPressEvent(QKeyEvent*);

private:
    Ui::NotesDialog *ui;

    QSqlDatabase db;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QValidator* validator;

    QSqlQueryModel* query;

    QWidget* addWidgetNote(int, bool);

    QList<QWidget*> widgets;
    QList<QHBoxLayout*> layouts;
    QList<QLabel*> labels;

    QStringList numbersList;
    QString callId;
    QString state;
    QString my_number;
    QString phone;
    QString loadState;
    QString page;
    QString pages;
    QString go;
    int count;
    int remainder;
};

#endif // NOTESDIALOG_H
