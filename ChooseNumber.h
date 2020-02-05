#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include <PlaceCallDialog.h>

#include <QDialog>
#include <QValidator>

class PlaceCallDialog;

namespace Ui {
class ChooseNumber;
}

class ChooseNumber : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseNumber(QWidget *parent = 0);
    void setValuesNumber(QString &);
    ~ChooseNumber();

public slots:
    void passNumber();
    void passNumber2();

private:
    Ui::ChooseNumber *ui;
    QValidator *validator;
    QString updateID;
    QString firstPassNumber;
    QString secondPassNumber;
    QString thirdPassNumber;
    QString fourthPassNumber;
    QString fifthPassNumber;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    PlaceCallDialog *placeCallDialog;
};

#endif // CHOOSENUMBER_H
