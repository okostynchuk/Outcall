#include "ChooseNumber.h"
#include "ui_ChooseNumber.h"

#include <QVariantList>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QPlainTextEdit>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QMessageBox>
#include <QEvent>

ChooseNumber::ChooseNumber(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseNumber)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle("Выбор номера");


    ui->FirstNumber->installEventFilter(this);
    ui->SecondNumber->installEventFilter(this);
    ui->ThirdNumber->installEventFilter(this);
    ui->FourthNumber->installEventFilter(this);
    ui->FifthNumber->installEventFilter(this);
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

    QSqlDatabase db1;
    QSqlQuery query_org(db1);
    QString orgName = QString ("select entry_name from entry_phone where entry_id = %1").arg(updateID);
    query_org.prepare(orgName);
    query_org.exec();

    while (query_org.next())
    {
        orgName = query_org.value(0).toString();
    }
    if (orgID != NULL)
    {
        ui->label_5->show();
        ui->label_5->setText("Номер(-a) \"" + orgName + "\"");
    }
}

bool ChooseNumber::eventFilter(QObject *target, QEvent *event)
{
    firstPassNumber = ui->FirstNumber->text();
    secondPassNumber = ui->SecondNumber->text();
    thirdPassNumber = ui->ThirdNumber->text();
    fourthPassNumber = ui->FourthNumber->text();
    fifthPassNumber = ui->FifthNumber->text();

    if(target == ui->FirstNumber && !ui->FirstNumber->text().isEmpty())
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            close();
            placeCallDialog = new PlaceCallDialog;
            placeCallDialog->getValuesNumber(firstPassNumber);
            placeCallDialog->exec();
            placeCallDialog->deleteLater();
            return true;
        } else { return false;}
    }
    else if (ui->SecondNumber->text().isEmpty() && ui->ThirdNumber->text().isEmpty() && ui->FourthNumber->text().isEmpty() && ui->FifthNumber->text().isEmpty())
        {
        ui->SecondNumber->hide(); ui->ThirdNumber->hide(); ui->FourthNumber->hide(); ui->FifthNumber->hide();
        ui->label_7->hide(); ui->label_8->hide(); ui->label_18->hide(); ui->label_19->hide();
        QWidget::setFixedHeight(70);
        }

    if(target == ui->SecondNumber && !ui->SecondNumber->text().isEmpty())
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            close();
            placeCallDialog = new PlaceCallDialog;
            placeCallDialog->getValuesNumber(secondPassNumber);
            placeCallDialog->exec();
            placeCallDialog->deleteLater();
            return true;
        } else { return false;}
    }

    if(target == ui->ThirdNumber && !ui->ThirdNumber->text().isEmpty())
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            close();
            placeCallDialog = new PlaceCallDialog;
            placeCallDialog->getValuesNumber(thirdPassNumber);
            placeCallDialog->exec();
            placeCallDialog->deleteLater();
            return true;
        } else { return false;}
    }

    if(target == ui->FourthNumber && !ui->FourthNumber->text().isEmpty())
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            close();
            placeCallDialog = new PlaceCallDialog;
            placeCallDialog->getValuesNumber(fourthPassNumber);
            placeCallDialog->exec();
            placeCallDialog->deleteLater();
            return true;
        } else { return false;}
    }

    if(target == ui->FifthNumber && !ui->FifthNumber->text().isEmpty())
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            close();
            placeCallDialog = new PlaceCallDialog;
            placeCallDialog->getValuesNumber(fifthPassNumber);
            connect(placeCallDialog, SIGNAL(sendData(QString)), this, SLOT(recieveData(QString)));
            placeCallDialog->exec();
            placeCallDialog->deleteLater();
            return true;
        } else{ return false;}
    }
    return QWidget::eventFilter(target, event);
}
