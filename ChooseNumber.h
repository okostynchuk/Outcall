#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include <PlaceCallDialog.h>

#include <QDialog>
#include <QValidator>

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

private:
    Ui::ChooseNumber *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
};

#endif // CHOOSENUMBER_H
