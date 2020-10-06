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
    explicit NotesDialog(QWidget* parent = 0);
    ~NotesDialog();

    void setValues(const QString& uniqueid, const QString& phone);
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
    Ui::NotesDialog* ui;

    QSqlDatabase db;

    QRegularExpression hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QValidator* validator;

    QSqlQueryModel* query;

    QWidget* addWidgetNote(qint32 row_index, bool url);

    QList<QWidget*> widgets;
    QList<QHBoxLayout*> layouts;
    QList<QLabel*> labels;

    qint32 count;
    qint32 remainder;

    QStringList numbersList;

    QString callId;
    QString state;
    QString my_number;
    QString phone;
    QString page;
    QString go;
};

#endif // NOTESDIALOG_H
