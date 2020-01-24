#ifndef DEBUGINFODIALOG_H
#define DEBUGINFODIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include <QSqlQueryModel>

namespace Ui {
class DebugInfoDialog;
}

class DebugInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugInfoDialog(QWidget *parent = 0);
    ~DebugInfoDialog();

    void updateDebug(const QString &info);

protected:
    void onClear() const;
    void onExit();

private:
    Ui::DebugInfoDialog *ui;
    QSqlQuery *query;
};

#endif // DEBUGINFODIALOG_H
