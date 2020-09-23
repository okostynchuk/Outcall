#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include "Global.h"
#include "PlaceCallDialog.h"
#include "AsteriskManager.h"

#include <QDialog>
#include <QValidator>
#include <QEvent>
#include <QLineEdit>
#include <QSqlDatabase>

class PlaceCallDialog;

namespace Ui {
class ChooseNumber;
}

class ChooseNumber : public QDialog
{
    Q_OBJECT

signals:
    void sendNumber(QString);

public:
    explicit ChooseNumber(QWidget *parent = 0);
    ~ChooseNumber();

    void setValues(QString);

private slots:
    void onCall(QString);

    void showEvent(QShowEvent*);
    bool eventFilter(QObject*, QEvent*);

private:
    Ui::ChooseNumber *ui;

    QSqlDatabase db;

    PlaceCallDialog* placeCallDialog;

    QValidator* validator;

    QList<QLineEdit*> phonesList;

    QString my_number;
    QString protocol;
    QString contactId;
};

#endif // CHOOSENUMBER_H
