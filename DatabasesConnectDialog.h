#ifndef DATABASESCONNECTDIALOG_H
#define DATABASESCONNECTDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QKeyEvent>

namespace Ui {
class DatabasesConnectDialog;
}

class DatabasesConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabasesConnectDialog(QWidget *parent = 0);
    ~DatabasesConnectDialog();

    void setDatabases(QSqlDatabase, QSqlDatabase, QString);

private slots:
    void onSave();
    void onClose();
    void setSettingsDb();
    void setSettingsDbCalls();
    void openDb();
    void openDbCalls();

    void keyPressEvent(QKeyEvent*);

private:
    Ui::DatabasesConnectDialog *ui;

    QSqlDatabase db;
    QSqlDatabase dbCalls;

    QString state;
};

#endif // DATABASESCONNECTDIALOG_H
