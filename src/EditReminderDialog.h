#ifndef EDITREMINDERDIALOG_H
#define EDITREMINDERDIALOG_H

#include "AsteriskManager.h"
#include "Global.h"
#include "ChooseEmployee.h"

#include <QDialog>
#include <QDateTime>
#include <QPointer>
#include <QSqlDatabase>
#include <QTextCursor>

namespace Ui {
class EditReminderDialog;
}

class EditReminderDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool update);
    void getPos(qint32 x, qint32 y);

public slots:
    void receiveEmployee(const QStringList& employee);

public:
    explicit EditReminderDialog(QWidget* parent = 0);
    ~EditReminderDialog();

    void setValues(const QString& id, const QString& groupId, const QDateTime& dateTime, const QString& note);

private slots:
    void onChooseEmployee();
    void onSave();
    void onTextChanged();
    void onCursorPosChanged();

    void on_add5MinButton_clicked();
    void on_add10MinButton_clicked();
    void on_add30MinButton_clicked();
    void on_add60MinButton_clicked();

    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

    bool eventFilter(QObject*, QEvent* event);

private:
    Ui::EditReminderDialog* ui;

    QSqlDatabase m_db;

    QPointer<ChooseEmployee> m_chooseEmployee;

    QStringList m_employeeInitial;
    QStringList m_employee;

    QString my_number;
    QString m_id;
    QString m_groupId;
    QString m_oldNote;

    QDateTime m_oldDateTime;

    QTextCursor m_textCursor;
};

#endif // EDITREMINDERDIALOG_H
