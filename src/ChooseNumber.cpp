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

    m_phones = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    for (qint32 i = 0; i < m_phones.length(); ++i)
    {
        m_phones.at(i)->setVisible(false);
        m_phones.at(i)->installEventFilter(this);
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
    QString protocol = global::getSettingsValue(g_personalNumber, "extensions").toString();

    g_asteriskManager->originateCall(g_personalNumber, number, protocol, g_personalNumber);
}

/**
 * Получает id контакта из классов PlaceCallDialog,
 * PopupReminder, ViewContactDialog, ViewOrgContactDialog.
 */
void ChooseNumber::setValues(const QStringList& numbers)
{
    for(qint32 i = 0; i < numbers.count(); i++)
        m_phones.at(i)->setText(numbers.at(i));
}

/**
 * Выполняет обработку появления окна.
 */
void ChooseNumber::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    qint32 size = 31;

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            QWidget::setFixedHeight(size += 26);

            m_phones.at(i)->setVisible(true);
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
