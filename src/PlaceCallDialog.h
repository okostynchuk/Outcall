#ifndef PLACECALLDIALOG_H
#define PLACECALLDIALOG_H

#include "ChooseNumber.h"
#include "SettingsDialog.h"

#include <QDialog>
#include <QSqlQueryModel>
#include <QValidator>
#include <QKeyEvent>
#include <QSqlDatabase>

class ChooseNumber;

namespace Ui {
class PlaceCallDialog;
}

class PlaceCallDialog : public QDialog
{
    Q_OBJECT

public slots:
    void receiveNumber(QString);

public:
    explicit PlaceCallDialog(QWidget* parent = 0);
    ~PlaceCallDialog();

private slots:
    void onCallButton();
    void onCancelButton();

    void onUpdate();
    void clearEditText();
    void onOrgChanged();
    void setModel(const QString& queryString);

    void showNumber(const QModelIndex& index);

    void on_lineEdit_returnPressed();
    void on_phoneLine_returnPressed();

    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PlaceCallDialog* ui;

    QSqlDatabase db;

    QStringList orgsId;
    QStringList orgsName;

    ChooseNumber* chooseNumber;

    QSqlQueryModel* queryModel;

    QValidator* validator;

    QString number;
    QString my_number;

    QByteArray geometry;
};

#endif // PLACECALLDIALOG_H
