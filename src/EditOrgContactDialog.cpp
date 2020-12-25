/*
 * Класс служит для редактирования организации.
 */

#include "EditOrgContactDialog.h"
#include "ui_EditOrgContactDialog.h"

#include "Global.h"
#include "AsteriskManager.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QDesktopWidget>

EditOrgContactDialog::EditOrgContactDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditOrgContactDialog)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->comment, &QTextEdit::textChanged, this, &EditOrgContactDialog::onTextChanged);
    connect(ui->comment, &QTextEdit::cursorPositionChanged, this, &EditOrgContactDialog::onCursorPosChanged);
    connect(ui->backButton, &QAbstractButton::clicked, this, &EditOrgContactDialog::onReturn);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditOrgContactDialog::onSave);

    m_phones = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    m_managers.insert("6203", ui->group_6203);
    m_managers.insert("6204", ui->group_6204);
    m_managers.insert("6207", ui->group_6207);

    QRegularExpression regExp("^[\\+]?[0-9]*$");
    for (qint32 i = 0; i < m_phones.length(); ++i)
        m_phones.at(i)->setValidator(new QRegularExpressionValidator(regExp, this));

    regExp.setPattern("^[0-9]*$");
    ui->vyborId->setValidator(new QRegularExpressionValidator(regExp, this));

    regExp.setPattern("^[0-9]{3}$");
    foreach (QString key, m_managers.keys())
        m_managers.value(key)->setValidator(new QRegularExpressionValidator(regExp, this));

    g_asteriskManager->m_groupNumbers.removeDuplicates();
}

EditOrgContactDialog::~EditOrgContactDialog()
{
    delete ui;
}

/**
 * Выполняет сохранение позиции текстового курсора.
 */
void EditOrgContactDialog::onCursorPosChanged()
{
    if (m_textCursor.isNull())
    {
        m_textCursor = ui->comment->textCursor();
        m_textCursor.movePosition(QTextCursor::End);
    }
    else
        m_textCursor = ui->comment->textCursor();
}

/**
 * Выполняет обработку совершения операций с привязанным объектом.
 */
bool EditOrgContactDialog::eventFilter(QObject*, QEvent* event)
{
    if (event && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);

        if (keyEvent && (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab))
        {
            if (ui->comment->hasFocus())
                ui->comment->setTextCursor(m_textCursor);

            return true;
        }
    }

    return false;
}

/**
 * Выполняет закрытие окна и отправку данных в класс ViewOrgContactDialog.
 */
void EditOrgContactDialog::onReturn()
{
    emit sendData(false, this->pos().x(), this->pos().y());

    close();
}

/**
 * Выполняет установку позиции окна в зависимости от позиции окна-родителя ViewOrgContactDialog.
 */
void EditOrgContactDialog::setPos(qint32 x, qint32 y)
{
    qint32 desktopHeight;
    qint32 desktopWidth;
    qint32 widgetHeight = QWidget::height();
    qint32 widgetWidth = QWidget::width();

    QDesktopWidget desktopWidget;
    QRect desktop = desktopWidget.availableGeometry(this);

    desktopWidth = desktop.width();
    desktopHeight = desktop.height();

    if (x < 0 && (desktopHeight - y) > widgetHeight)
    {
        x = 0;
        this->move(x, y);
    }
    else if (x < 0 && ((desktopHeight - y) < widgetHeight))
    {
        x = 0;
        y = widgetHeight;
        this->move(x, y);
    }
    else if ((desktopWidth - x) < widgetWidth && (desktopHeight - y) > widgetHeight)
    {
        x = widgetWidth;
        this->move(x, y);
    }
    else if ((desktopWidth - x) < widgetWidth && ((desktopHeight - y) < widgetHeight))
    {
        x = widgetWidth;
        y = widgetHeight;
        this->move(x, y);
    }
    else if (x > 0 && ((desktopHeight - y) < widgetHeight))
    {
        y = widgetHeight;
        this->move(x, y);
    }
    else
    {
        this->move(x, y);
    }
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void EditOrgContactDialog::onSave()
{
    QSqlQuery query(m_db);

    QString orgName = ui->orgName->text();

    QStringList actualPhonesList;

    for (qint32 i = 0; i < m_phones.length(); ++i)
    {
        if (i < m_oldPhones.length() && m_phones.at(i)->text() == m_oldPhones.at(i))
            actualPhonesList.append(m_phones.at(i)->text());
        else
        {
            m_phones.at(i)->setStyleSheet("border: 1px solid grey");

            actualPhonesList.append(m_phones.at(i)->text().remove(QRegularExpression("^[\\+]?[3]?[8]?")));
        }
    }

    bool empty_field = false;

    if (ui->orgName->text().isEmpty())
    {
         ui->label_15->setText("<span style=\"color: red;\">" + tr("Заполните обязательное поле!") + "</span>");

         ui->orgName->setStyleSheet("border: 1px solid red");

         empty_field = true;
    }
    else
    {
        ui->label_15->setText("");

        ui->orgName->setStyleSheet("border: 1px solid grey");
    }

    if (ui->firstNumber->text().isEmpty())
    {
        ui->label_14->show();
        ui->label_14->setText("<span style=\"color: red;\">" + tr("Заполните обязательное поле!") + "</span>");

        ui->firstNumber->setStyleSheet("border: 1px solid red");

        empty_field = true;
    }
    else
    {
        ui->label_14->setText("");

        ui->firstNumber->setStyleSheet("border: 1px solid grey");
    }

    if (empty_field)
        return;

    bool invalid_phones = false;

    for (qint32 i = 0; i < m_phones.length(); ++i)
    {
        if (!m_phones.at(i)->text().isEmpty())
        {
            bool old_phone = false;

            for (qint32 j = 0; j < m_oldPhones.length(); ++j)
                if (m_phones.at(i)->text() == m_oldPhones.at(j))
                    old_phone = true;

            if (!old_phone)
            {
                QString phone = m_phones.at(i)->text();

                if (isPhone(&phone))
                    m_phones.at(i)->setStyleSheet("border: 1px solid grey");
                else
                {
                    m_phones.at(i)->setStyleSheet("border: 1px solid red");

                    invalid_phones = true;
                }
            }
        }
    }

    if (invalid_phones)
    {
        MsgBoxError(tr("Номер не соответствует формату!"));

        return;
    }

    bool same_phones = false;

    for (qint32 i = 0; i < m_phones.length(); ++i)
        for (qint32 j = 0; j < m_phones.length(); ++j)
        {
            if (!m_phones.at(i)->text().isEmpty() && actualPhonesList.at(i) == actualPhonesList.at(j) && i != j)
            {
                m_phones.at(i)->setStyleSheet("border: 1px solid red");
                m_phones.at(j)->setStyleSheet("border: 1px solid red");

                same_phones = true;
            }
        }

    if (same_phones)
    {
        MsgBoxError(tr("Присутсвуют одинаковые номера!"));

        return;
    }

    bool existing_phones = false;

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + actualPhonesList.at(i) + "' AND entry_id <> " + m_contactId + ")");
            query.exec();
            query.next();

            if (query.value(0) != 0)
            {
                m_phones.at(i)->setStyleSheet("border: 1px solid red");

                existing_phones = true;
            }
        }

    if (existing_phones)
    {
        MsgBoxError(tr("Введены существующие номера!"));

        return;
    }

//    bool invalid_employee = false;

//    QString employee = ui->employee->text();

//    if (!ui->employee->text().isEmpty())
//    {
//        if (g_asteriskManager->m_extensionNumbers.contains(employee) || g_asteriskManager->m_groupNumbers.contains(employee))
//            ui->employee->setStyleSheet("border: 1px solid grey");
//        else
//        {
//            ui->employee->setStyleSheet("border: 1px solid red");

//            invalid_employee = true;
//        }
//    }

//    if (invalid_employee)
//    {
//        MsgBoxError(tr("Указанный номер не зарегистрирован!"));

//        return;
//    }

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_org_name = ?, entry_city = ?, entry_address = ?, "
                  "entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
    query.addBindValue("org");
    query.addBindValue(orgName);
    query.addBindValue(orgName);
    query.addBindValue(ui->city->text());
    query.addBindValue(ui->address->text());
    query.addBindValue(ui->email->text());
    query.addBindValue(ui->vyborId->text());
    query.addBindValue(ui->comment->toPlainText().trimmed());
    query.addBindValue(m_contactId);
    query.exec();

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            if (i >= m_oldPhones.length())
            {
                query.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query.addBindValue(m_contactId);
                query.addBindValue(actualPhonesList.at(i));
                query.exec();
            }
            else
            {
                query.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                query.addBindValue(actualPhonesList.at(i));
                query.addBindValue(m_contactId);
                query.addBindValue(m_oldPhones.at(i));
                query.exec();
            }
        }

    foreach (QString key, m_managers.keys())
    {
        if (!m_oldManagers.keys().contains(key))
        {
            if (!m_managers.value(key)->text().isEmpty())
            {
                query.prepare("INSERT INTO managers (id_client, group_number, manager_number)"
                              "VALUES(?, ?, ?)");
                query.addBindValue(m_contactId);
                query.addBindValue(key);
                query.addBindValue(m_managers.value(key)->text());
                query.exec();
            }
        }
        else
        {
            if (m_managers.value(key)->text() != m_oldManagers.value(key))
            {
                if (m_managers.value(key)->text().isEmpty())
                {
                    query.prepare("DELETE FROM managers WHERE id_client = ? AND group_number = ?");
                    query.addBindValue(m_contactId);
                    query.addBindValue(key);
                    query.exec();
                }
                else
                {
                    query.prepare("UPDATE managers SET manager_number = ? WHERE id_client = ? AND group_number = ?");
                    query.addBindValue(m_managers.value(key)->text());
                    query.addBindValue(m_contactId);
                    query.addBindValue(key);
                    query.exec();
                }
            }
        }
    }

    emit sendData(true, this->pos().x(), this->pos().y());

    close();

    MsgBoxInformation(tr("Запись успешно изменена!"));
}

/**
 * Выполняет проверку на соответсвие номера шаблону.
 */
bool EditOrgContactDialog::isPhone(QString* str)
{
    qint32 pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("(^[\\+][3][8][0][0-9]{9}$|^[3][8][0][0-9]{9}$|^[0][0-9]{9}$)"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Получает и заполняет поля окна необходимыми данными.
 * Получает id контакта из классов CallHistoryDialog, ViewOrgContactDialog.
 */
void EditOrgContactDialog::setValues(const QString& id)
{
    m_contactId = id;

    QSqlQuery query(m_db);

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + m_contactId);
    query.exec();

    while (query.next())
        m_oldPhones.append(query.value(0).toString());

    for (qint32 i = 0; i < m_oldPhones.length(); ++i)
        m_phones.at(i)->setText(m_oldPhones.at(i));

    query.prepare("SELECT group_number, manager_number FROM managers WHERE id_client = " + m_contactId);
    query.exec();

    while (query.next())
    {
        m_oldManagers.insert(query.value(0).toString(), query.value(1).toString());

        m_managers.value(query.value(0).toString())->setText(query.value(1).toString());
    }

    query.prepare("SELECT DISTINCT entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, "
                  "entry_comment FROM entry WHERE id = " + m_contactId);
    query.exec();
    query.next();

    ui->orgName->setText(query.value(0).toString());
    ui->city->setText(query.value(1).toString());
    ui->address->setText(query.value(2).toString());
    ui->email->setText(query.value(3).toString());
    ui->vyborId->setText(query.value(4).toString());
    ui->comment->setText(query.value(5).toString());
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void EditOrgContactDialog::onTextChanged()
{
    qint32 maxTextLength = 255;

    if (ui->comment->toPlainText().length() > maxTextLength)
    {
        qint32 diff = ui->comment->toPlainText().length() - maxTextLength;

        QString newStr = ui->comment->toPlainText();
        newStr.chop(diff);

        ui->comment->setText(newStr);

        QTextCursor cursor(ui->comment->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

        ui->comment->setTextCursor(cursor);
    }
}

/**
 * Выполняет скрытие кнопки возврата к карточке организации.
 */
void EditOrgContactDialog::hideBackButton()
{
    ui->backButton->hide();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Enter.
 */
void EditOrgContactDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        if (ui->comment->hasFocus())
            return;
        else
            onSave();
    }
    else
        QDialog::keyPressEvent(event);
}
