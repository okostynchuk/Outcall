#include "ChooseNumber.h"
#include "ui_ChooseNumber.h"

#include <QSqlDatabase>
#include <QTableView>
#include <QSqlQuery>
#include <QEvent>

ChooseNumber::ChooseNumber(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseNumber)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    this->setWindowTitle(tr("Выбор номера"));

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

    QString sql = QString("SELECT entry_phone FROM entry_phone WHERE entry_id = %1").arg(updateID);

    query.prepare(sql);
    query.exec();

    if (query.next())
        firstNumber = query.value(0).toString();
    if (query.next())
        secondNumber = query.value(0).toString();
    if (query.next())
        thirdNumber = query.value(0).toString();
    if (query.next())
        fourthNumber = query.value(0).toString();
    if (query.next())
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

    QString orgName = QString ("SELECT entry_name FROM entry_phone WHERE entry_id = %1").arg(updateID);

    query_org.prepare(orgName);
    query_org.exec();

    while (query_org.next())
        orgName = query_org.value(0).toString();

    if (orgID != NULL)
    {
        ui->label_5->show();
        ui->label_5->setText(tr("Номер(-a) \"") + orgName + tr("\""));
    }
}

bool ChooseNumber::eventFilter(QObject *target, QEvent *event)
{
    firstPassNumber = ui->FirstNumber->text();
    secondPassNumber = ui->SecondNumber->text();
    thirdPassNumber = ui->ThirdNumber->text();
    fourthPassNumber = ui->FourthNumber->text();
    fifthPassNumber = ui->FifthNumber->text();

    if (ui->SecondNumber->text().isEmpty())
    {
        ui->SecondNumber->hide(); ui->ThirdNumber->hide(); ui->FourthNumber->hide(); ui->FifthNumber->hide();
        ui->label_7->hide(); ui->label_8->hide(); ui->label_18->hide(); ui->label_19->hide();

        QWidget::setFixedHeight(70);
    }
    else if (ui->ThirdNumber->text().isEmpty())
    {
        ui->ThirdNumber->hide(); ui->FourthNumber->hide(); ui->FifthNumber->hide();
        ui->label_8->hide(); ui->label_18->hide(); ui->label_19->hide();

        QWidget::setFixedHeight(100);
    }
    else if (ui->FourthNumber->text().isEmpty())
    {
        ui->FourthNumber->hide(); ui->FifthNumber->hide();
        ui->label_18->hide(); ui->label_19->hide();

        QWidget::setFixedHeight(130);
    }
    else if (ui->FifthNumber->text().isEmpty())
    {
        ui->FifthNumber->hide();
        ui->label_19->hide();

        QWidget::setFixedHeight(160);
    }

    if (target == ui->FirstNumber && !ui->FirstNumber->text().isEmpty())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QString numb = ui->FirstNumber->text();

            emit sendNumber(numb);

            close();

            return true;
        }
        else
            return false;
    }

    if (target == ui->SecondNumber && !ui->SecondNumber->text().isEmpty())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QString numb = ui->SecondNumber->text();

            emit sendNumber(numb);

            close();

            return true;
        }
        else
            return false;
    }

    if (target == ui->ThirdNumber && !ui->ThirdNumber->text().isEmpty())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QString numb = ui->ThirdNumber->text();

            emit sendNumber(numb);

            close();

            return true;
        }
        else
            return false;
    }

    if (target == ui->FourthNumber && !ui->FourthNumber->text().isEmpty())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QString numb = ui->FourthNumber->text();

            emit sendNumber(numb);

            close();

            return true;
        }
        else
            return false;
    }

    if (target == ui->FifthNumber && !ui->FifthNumber->text().isEmpty())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QString numb = ui->FifthNumber->text();

            emit sendNumber(numb);

            close();

            return true;
        }
        else
            return false;
    }

    return QWidget::eventFilter(target, event);
}
