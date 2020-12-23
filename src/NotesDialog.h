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
#include <QPointer>

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

    QSqlDatabase m_db;

    QRegularExpression m_hrefRegExp = QRegularExpression("(https?:\\/\\/\\S+)");

    QPointer<QSqlQueryModel> m_queryModel;

    QWidget* addWidgetNote(qint32 row_index, bool url);

    QList<QWidget*> m_widgets;

    QStringList m_numbers;

    QString m_callId;
    QString m_phone;
    QString m_page;
    QString m_go;
};

#endif // NOTESDIALOG_H
