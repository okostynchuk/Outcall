#ifndef DATABASESCONNECTDIALOG_H
#define DATABASESCONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class DatabasesConnectDialog;
}

class DatabasesConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabasesConnectDialog(QWidget *parent = 0);
    ~DatabasesConnectDialog();

protected:
    void onSave();

private:
    Ui::DatabasesConnectDialog *ui;
};

#endif // DATABASESCONNECTDIALOG_H
