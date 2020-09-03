#include "ChooseNumber.h"
#include "ui_ChooseNumber.h"

#include <QSqlDatabase>
#include <QSqlQuery>

ChooseNumber::ChooseNumber(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseNumber)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    my_number = global::getExtensionNumber("extensions");
    protocol = global::getSettingsValue(my_number, "extensions").toString();

    phonesList = { ui->FirstNumber, ui->SecondNumber, ui->ThirdNumber, ui->FourthNumber, ui->FifthNumber };

    for (int i = 0; i < phonesList.length(); ++i)
    {
        phonesList.at(i)->setVisible(false);
        phonesList.at(i)->installEventFilter(this);
    }
}

ChooseNumber::~ChooseNumber()
{
    delete ui;
}

void ChooseNumber::onCall(QString number)
{
    g_pAsteriskManager->originateCall(my_number, number, protocol, my_number);
}

void ChooseNumber::setValuesNumber(QString i)
{
    updateID = i;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT entry_phone, entry_name FROM entry_phone WHERE entry_id = " + updateID);
    query.exec();

    while (query.next())
         numbersList.append(query.value(0).toString());

    for (int i = 0; i < numbersList.length(); ++i)
        phonesList.at(i)->setText(numbersList.at(i));

    query.first();

    ui->label->show();
    ui->label->setText(tr("Номерa") + "\"" + query.value(1).toString() + "\"");
}

void ChooseNumber::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    int size = 31;

    for (int i = 0; i < numbersList.length(); ++i)
    {
        QWidget::setFixedHeight(size += 26);
        phonesList.at(i)->setVisible(true);
    }
}

bool ChooseNumber::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QLineEdit* line =  QDialog::findChild<QLineEdit*>(target->objectName());

        onCall(line->text());

        emit sendNumber(line->text());

        close();
    }

    return false;
}
