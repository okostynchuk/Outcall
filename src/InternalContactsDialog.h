#ifndef INTERNALCONTACTSDIALOG_H
#define INTERNALCONTACTSDIALOG_H

#include "AsteriskManager.h"
#include "Global.h"
#include "AddReminderDialog.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QDebug>

class AddReminderDialog;

namespace Ui {
class InternalContactsDialog;
}

class InternalContactsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InternalContactsDialog(QWidget* parent = 0);
    ~InternalContactsDialog();

private slots:
    void loadContacts();
    void onExtenStatusChanged(const QString& exten, qint32& state);

    void on_callButton_clicked();
    void on_addReminderButton_clicked();
    void on_listWidget_clicked();
    void setIcon(qint32 index, qint32 state);
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::InternalContactsDialog* ui;

    AddReminderDialog* m_addReminderDialog;

    QStringList m_extensions;

    QByteArray m_geometry;

    QMap<QString, QString> m_states;
    QMap<QString, qint32> m_indexes;
};

#endif // INTERNALCONTACTSDIALOG_H
