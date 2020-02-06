#include "ChooseNumber.h"
#include "ui_ChooseNumber.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QString>
#include <QMessageBox>

ChooseNumber::ChooseNumber(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseNumber)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->FirstNumber, SIGNAL(returnPressed()), this, SLOT(passNumber()));
    connect(ui->SecondNumber, SIGNAL(returnPressed()), this, SLOT(passNumber2()));
    //connect(ui->FirstNumber, SIGNAL(returnPressed()), this, SLOT(passNumber(const QString &)));
}

ChooseNumber::~ChooseNumber()
{
    delete ui;
}

void ChooseNumber::setValuesNumber(QString &i)
{
    updateID = i;
    QSqlDatabase db;
    QSqlQuery query(db);
    QString sql = QString("select entry_phone from entry_phone where entry_id = %1").arg(updateID);
    query.prepare(sql);
    query.exec();
    query.next();
    firstNumber = query.value(0).toString();
    query.next();
    secondNumber = query.value(0).toString();
    query.next();
    thirdNumber = query.value(0).toString();
    query.next();
    fourthNumber = query.value(0).toString();
    query.next();
    fifthNumber = query.value(0).toString();
    query.prepare("SELECT entry_person_org_id FROM entry WHERE id = " + updateID);
    query.exec();
    query.next();
    QString orgID = query.value(0).toString();
    query.prepare("SELECT entry_org_name FROM entry WHERE id = " + orgID);
    query.exec();
    query.next();

    ui->FirstNumber->setText(firstNumber);
    ui->SecondNumber->setText(secondNumber);
    ui->ThirdNumber->setText(thirdNumber);
    ui->FourthNumber->setText(fourthNumber);
    ui->FifthNumber->setText(fifthNumber);
}

void ChooseNumber::passNumber()
{
    firstPassNumber = ui->FirstNumber->text();
//    secondPassNumber = ui->SecondNumber->text();
//    thirdPassNumber = ui->ThirdNumber->text();
//    fourthPassNumber = ui->FourthNumber->text();
//    fifthPassNumber = ui->FifthNumber->text();

    placeCallDialog = new PlaceCallDialog;

    if(QString(ui->FirstNumber->text()).isEmpty() == false /*&& Qt::Key_Return || Qt::Key_Enter*/){

                                                             //&& (ui->FirstNumber->returnPressed())){
    close();

    placeCallDialog->getValuesNumber(firstPassNumber);
    placeCallDialog->exec();
    placeCallDialog->deleteLater();
    }

//    placeCallDialog->getValuesNumber(thirdPassNumber);
//    placeCallDialog->exec();
//    placeCallDialog->deleteLater();

//    placeCallDialog->getValuesNumber(fourthPassNumber);
//    placeCallDialog->exec();
//    placeCallDialog->deleteLater();

//    placeCallDialog->getValuesNumber(fifthPassNumber);
//    placeCallDialog->exec();
//    placeCallDialog->deleteLater();
    //close();
}

void ChooseNumber::passNumber2(){
    secondPassNumber = ui->SecondNumber->text();

    placeCallDialog = new PlaceCallDialog;

    if(QString(ui->SecondNumber->text()).isEmpty() == false){
    close();
    placeCallDialog->getValuesNumber(secondPassNumber);
    placeCallDialog->exec();
    placeCallDialog->deleteLater();
    }
}
