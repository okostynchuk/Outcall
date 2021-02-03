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
    m_phonesComments = { ui->firstNumberComment, ui->secondNumberComment, ui->thirdNumberComment, ui->fourthNumberComment, ui->fifthNumberComment };  

    for (qint32 i = 0; i < m_phones.length(); ++i)
    {
        m_phones.at(i)->setVisible(false);
        m_phonesComments.at(i)->setVisible(false);
    }

    ui->saveButton->setVisible(false);
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
void ChooseNumber::setValues(const QString id, qint32 status)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT fone, comment FROM fones WHERE entry_id = " + id + " ORDER BY priority");
    query.exec();

    qint32 phonesCount = 0;
    while (query.next())
    {
        m_phones.at(phonesCount)->setText(query.value(0).toString());
        m_phonesComments.at(phonesCount)->setText(query.value(1).toString());
        phonesCount++;
    }

    switch (status) {
    case call:
        for (qint32 i = 0; i < m_phones.length(); ++i)
            m_phones.at(i)->installEventFilter(this);
        break;
    case orderChange:
        ui->saveButton->setVisible(true);

        for (qint32 i = 0; i < m_phones.length(); ++i)
            if (!m_phones.at(i)->text().isEmpty())
            {
                QAction* action_1 = m_phones.at(i)->addAction(QIcon(":/images/arrowDown.png"), QLineEdit::TrailingPosition);
                QAction* action_2 = m_phones.at(i)->addAction(QIcon(":/images/arrowUp"), QLineEdit::TrailingPosition);

                if (i == 0)
                    action_2->setDisabled(true);
                else if (i == phonesCount - 1)
                    action_1->setDisabled(true);

                action_1->setProperty("position", QVariant::fromValue(i));
                action_2->setProperty("position", QVariant::fromValue(i));

                action_1->setProperty("name", "down");
                action_2->setProperty("name", "up");

                connect(action_1, &QAction::triggered, this, &ChooseNumber::phonePriorityChanged);
                connect(action_2, &QAction::triggered, this, &ChooseNumber::phonePriorityChanged);
            }
        break;
    }
}

void ChooseNumber::phonePriorityChanged()
{
    qint32 position = sender()->property("position").value<qint32>();
    QString name = sender()->property("name").value<QString>();

    qint32 next_position = 0;
    if (name == "up")
        next_position = position - 1;
    else if (name == "down")
        next_position = position + 1;

    QString curr_phone = m_phones.at(position)->text();
    QString neigh_phone = m_phones.at(next_position)->text();

    QString curr_phoneComment = m_phonesComments.at(position)->text();
    QString neigh_phoneComment = m_phonesComments.at(next_position)->text();

    m_phones.at(next_position)->setText(curr_phone);
    m_phones.at(position)->setText(neigh_phone);
    m_phonesComments.at(next_position)->setText(curr_phoneComment);
    m_phonesComments.at(position)->setText(neigh_phoneComment);
}

/**
 * Выполняет обработку появления окна.
 */
void ChooseNumber::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    qint32 size = 57;

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            QWidget::setFixedHeight(size += 26);

            m_phones.at(i)->setVisible(true);
            m_phonesComments.at(i)->setVisible(true);
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

void ChooseNumber::on_saveButton_clicked()
{
    QSqlQuery query(m_db);

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            query.prepare("UPDATE fones SET priority = ? WHERE fone = ?");
            query.addBindValue(i);
            query.addBindValue(m_phones.at(i)->text());
            query.exec();
        }

    close();
    emit phonesOrderChanged();
}
