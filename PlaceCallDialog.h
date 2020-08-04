#ifndef PLACECALLDIALOG_H
#define PLACECALLDIALOG_H

#include "ChooseNumber.h"
#include "SettingsDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QTableView>
#include <QValidator>

class Contact;
class QTreeWidgetItem;
class ChooseNumber;

namespace Ui {
class PlaceCallDialog;
}

class PlaceCallDialog : public QDialog
{
    Q_OBJECT

public slots:
    void receiveNumber(QString &number);

public:
    explicit PlaceCallDialog(QWidget *parent = 0);
    ~PlaceCallDialog();

    void show();

private slots:
    void onCallButton();
    void onCancelButton();
    void onUpdate();
    void modelNull();
    void onComboBoxSelected();
    void clearEditText();
    void showNumber(const QModelIndex &);

    void on_lineEdit_returnPressed();

private:
    Ui::PlaceCallDialog *ui;

    ChooseNumber *chooseNumber;
    PlaceCallDialog *placeCallDialog;

    QSqlQueryModel *query1;
    QSqlQueryModel *query2;

    QValidator *validator;

    QString update;
    QString number;
    QString my_number;
};

#endif // PLACECALLDIALOG_H
