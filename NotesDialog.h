#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include "SettingsDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <QLayout>
#include <QLabel>
#include <QKeyEvent>

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

    void receiveData(QString uniqueid, QString phone, QString loadState);
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

    void keyPressEvent(QKeyEvent* event);

private:
    Ui::NotesDialog *ui;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QValidator* validator;

    QSqlQueryModel* query;

    QWidget* addWidgetNote(int, QString);

    QList<QWidget*> widgets;
    QList<QHBoxLayout*> layouts;
    QList<QLabel*> labels;

    QStringList numbersList;
    QString callId;
    QString state;
    QString my_number;
    QString phoneNumber;
    QString loadState;
    QString page;
    QString pages;
    QString go;
    int count;
    int remainder;
};

#endif // NOTESDIALOG_H
