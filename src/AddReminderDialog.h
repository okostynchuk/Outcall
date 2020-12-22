#ifndef ADDREMINDERDIALOG_H
#define ADDREMINDERDIALOG_H

#include "Global.h"
#include "AsteriskManager.h"
#include "ChooseEmployee.h"

#include <QDialog>
#include <QKeyEvent>
#include <QPointer>
#include <QSqlQuery>

namespace Ui {
class AddReminderDialog;
}

class AddReminderDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool update);
    void getPos(qint32 x, qint32 y);

public slots:
    void receiveEmployee(const QStringList& employee);

public:
    explicit AddReminderDialog(QWidget* parent = nullptr);
    ~AddReminderDialog();

    void setCallId(const QString& callId);

private slots:
    void onChooseEmployee();
    void onSave();
    void onTextChanged();

    void on_add10MinButton_clicked();
    void on_add30MinButton_clicked();
    void on_add5MinButton_clicked();
    void on_add60MinButton_clicked();

    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    Ui::AddReminderDialog* ui;

    QSqlDatabase m_db;

    QPointer<ChooseEmployee> m_chooseEmployee;

    QStringList m_employee;

    QString my_number;
    QString m_callId;
};

#endif // ADDREMINDERDIALOG_H
