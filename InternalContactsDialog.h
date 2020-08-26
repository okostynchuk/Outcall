#ifndef INTERNALCONTACTSDIALOG_H
#define INTERNALCONTACTSDIALOG_H

#include "AsteriskManager.h"
#include "Global.h"
#include "AddReminderDialog.h"

#include <QDialog>
#include <QList>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>

class AddReminderDialog;

namespace Ui {
class InternalContactsDialog;
}

class InternalContactsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InternalContactsDialog(QWidget *parent = 0);
    ~InternalContactsDialog();

private slots:
    void onCallButtonClicked();
    void onAddReminder();
    void onSearch();
    void deleteObjects();
    void loadContacts();

    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_nextEndButton_clicked();
    void on_previousStartButton_clicked();
    void on_lineEdit_page_returnPressed();

    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    Ui::InternalContactsDialog *ui;

    AddReminderDialog* addReminderDialog;

    QValidator* validator;

    QString my_number;
    QString page;
    QString pages;
    QString go;

    int count;
    int remainder;
    int l_from;
    int l_to;

    QWidget* addWgt(QString);

    QListWidget* list;

    QList<QListWidgetItem*> itemsSearch;
    QList<QPushButton*> buttons;
    QList<QHBoxLayout*> layouts;
    QList<QWidget*> widgets;

    QStringList extensions;
    QStringList extensions_full;
};

#endif // INTERNALCONTACTSDIALOG_H
