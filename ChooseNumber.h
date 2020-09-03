#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include "Global.h"
#include "PlaceCallDialog.h"
#include "AsteriskManager.h"

#include <QDialog>
#include <QValidator>
#include <QEvent>
#include <QLineEdit>

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

    void setValuesNumber(QString);

private slots:
    void onCall(QString);

    void showEvent(QShowEvent* event);
    bool eventFilter(QObject* target, QEvent* event);

private:
    Ui::ChooseNumber *ui;

    PlaceCallDialog* placeCallDialog;

    QValidator* validator;

    QList<QLineEdit*> phonesList;

    QStringList numbersList;

    QString my_number;
    QString protocol;
    QString updateID;
};

#endif // CHOOSENUMBER_H
