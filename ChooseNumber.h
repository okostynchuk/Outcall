#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include "PlaceCallDialog.h"

#include <QDialog>
#include <QString>
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
    void sendNumber(QString &);

public:
    explicit ChooseNumber(QWidget *parent = 0);
    ~ChooseNumber();

    void setValuesNumber(QString &);

private slots:
    bool eventFilter(QObject*, QEvent *event);

private:
    Ui::ChooseNumber *ui;

    PlaceCallDialog *placeCallDialog;

    QValidator *validator;

    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
};

#endif // CHOOSENUMBER_H
