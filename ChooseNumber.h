#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include "Global.h"
#include "PlaceCallDialog.h"
#include "AsteriskManager.h"

#include <QDialog>
#include <QValidator>
#include <QEvent>

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

    bool eventFilter(QObject* target, QEvent* event);

private:
    Ui::ChooseNumber *ui;

    PlaceCallDialog* placeCallDialog;

    QValidator* validator;

    QString my_number;
    QString protocol;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
};

#endif // CHOOSENUMBER_H
