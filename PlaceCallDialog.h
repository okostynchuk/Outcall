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

public:
    explicit PlaceCallDialog(QWidget *parent = 0);
    void show();
    ~PlaceCallDialog();

protected:
     void clearCallTree();

public slots:
    void receiveNumber( QString &number);

protected slots:
    void onCallButton();
    void onCancelButton();
    void onUpdate();
    void modelNull();
    void onComboBoxSelected();
    void clearEditText();
    void showNumber(const QModelIndex &);


private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::PlaceCallDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QValidator *validator;
    QString update;
    QString number;
    ChooseNumber *chooseNumber;
    PlaceCallDialog *placeCallDialog;
    SettingsDialog *settingsDialog;
    QString my_number;
};

#endif // PLACECALLDIALOG_H
