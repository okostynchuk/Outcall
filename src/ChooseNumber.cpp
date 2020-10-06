/*
 * Класс служит для выбора номера телефона определенного контакта для совершения звонка.
 */

#include "ChooseNumber.h"
#include "ui_ChooseNumber.h"

#include <QSqlQuery>

ChooseNumber::ChooseNumber(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ChooseNumber)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    my_number = global::getExtensionNumber("extensions");
    protocol = global::getSettingsValue(my_number, "extensions").toString();

    phonesList = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    for (qint32 i = 0; i < phonesList.length(); ++i)
    {
        phonesList.at(i)->setVisible(false);
        phonesList.at(i)->installEventFilter(this);
    }
}

ChooseNumber::~ChooseNumber()
{
    delete ui;
}

/**
 * Выполняет операцию для последующего совершения звонка.
 */
void ChooseNumber::onCall(const QString& number)
{
    g_pAsteriskManager->originateCall(my_number, number, protocol, my_number);
}

/**
 * Получает id контакта из классов PlaceCallDialog,
 * PopupReminder, ViewContactDialog, ViewOrgContactDialog.
 */
void ChooseNumber::setValues(const QString& id)
{
    contactId = id;

    QSqlQuery query(db);

    query.prepare("SELECT entry_phone, entry_name FROM entry_phone WHERE entry_id = " + contactId);
    query.exec();

    qint32 i = 0;

    while (query.next())
    {
        phonesList.at(i)->setText(query.value(0).toString());

        ++i;
    }

    query.first();

    ui->label->show();
    ui->label->setText(tr("Номерa") + " \"" + query.value(1).toString() + "\"");
}

/**
 * Выполняет обработку появления окна.
 */
void ChooseNumber::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    qint32 size = 31;

    for (qint32 i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            QWidget::setFixedHeight(size += 26);

            phonesList.at(i)->setVisible(true);
        }

    QWidget::setFixedHeight(size += 10);
}

/**
 * Выполняет обработку совершения операций с привязанным объектом.
 */
bool ChooseNumber::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QLineEdit* line = QDialog::findChild<QLineEdit*>(target->objectName());

        onCall(line->text());

        emit sendNumber(line->text());

        close();

        return true;
    }

    return false;
}
