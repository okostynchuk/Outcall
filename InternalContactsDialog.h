#ifndef INTERNALCONTACTSDIALOG_H
#define INTERNALCONTACTSDIALOG_H

#include "AsteriskManager.h"
#include "SettingsDialog.h"
#include "Global.h"
#include "AsteriskManager.h"
#include "AddReminderDialog.h"

#include <QDialog>
#include <QTreeWidget>
#include <QList>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>

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
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *);
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



private:
    Ui::InternalContactsDialog *ui;

    AddReminderDialog* addReminderDialog;

    QValidator *validator;

    QString page;
    QString pages;
    QString go;

    int count;
    int remainder;
    int l_from;
    int l_to;

    QWidget* addWgt(QString, bool);

    QListWidget *list;

    QList<QListWidgetItem *> items;

    QList<QListWidgetItem *> itemsSearch;

    QList<QPushButton*> buttons;
    QList<QPushButton*> buttonsSearch;

    QList<QHBoxLayout*> layouts;
    QList<QHBoxLayout*> layoutsSearch;

    QList<QWidget*> widgets;
    QList<QWidget*> widgetsSearch;

    QStringList extensions;
    QStringList extensions_full;

    QString my_number;
};

#endif // INTERNALCONTACTSDIALOG_H
