#ifndef DATABASESCONNECTDIALOG_H
#define DATABASESCONNECTDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class DatabasesConnectDialog;
}

class DatabasesConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabasesConnectDialog(QWidget *parent = 0);
    ~DatabasesConnectDialog();

    void setDatabases(QSqlDatabase db, QSqlDatabase dbCalls, QString state);

private slots:
    void onSave();
    void onClose();
    void setSettingForFirstDb();
    void setSettingForSecondDb();
    void checkDb();
    void checkDbAsterisk();

private:
    Ui::DatabasesConnectDialog *ui;

    QString state_db;
    QSqlDatabase DB;
    QSqlDatabase DbAsterisk;
};

#endif // DATABASESCONNECTDIALOG_H
