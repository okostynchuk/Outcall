/*
 * Класс служит для редактирования организации.
 */

#include "EditOrgContactDialog.h"
#include "ui_EditOrgContactDialog.h"
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

    phonesList = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    QRegularExpression regExp("^[\\+]?[0-9]*$");
    Validator = new QRegularExpressionValidator(regExp, this);

    for (qint32 i = 0; i < phonesList.length(); ++i)
        phonesList.at(i)->setValidator(Validator);

    regExp.setPattern("^[0-9]*$");
    Validator = new QRegularExpressionValidator(regExp, this);

    ui->vyborId->setValidator(Validator);

    regExp.setPattern("^[0-9]{3,4}$");
    Validator = new QRegularExpressionValidator(regExp, this);
    ui->employee->setValidator(Validator);

    g_pAsteriskManager->groupNumbers.removeDuplicates();
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
    if (textCursor.isNull())
    {
        textCursor = ui->comment->textCursor();
        textCursor.movePosition(QTextCursor::End);
    }
    else
        textCursor = ui->comment->textCursor();
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
                ui->comment->setTextCursor(textCursor);

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
    qint32 nDesktopHeight;
    qint32 nDesktopWidth;
    qint32 nWidgetHeight = QWidget::height();
    qint32 nWidgetWidth = QWidget::width();

    QDesktopWidget desktop;
    QRect rcDesktop = desktop.availableGeometry(this);

    nDesktopWidth = rcDesktop.width();
    nDesktopHeight = rcDesktop.height();

    if (x < 0 && (nDesktopHeight - y) > nWidgetHeight)
    {
        x = 0;
        this->move(x, y);
    }
    else if (x < 0 && ((nDesktopHeight - y) < nWidgetHeight))
    {
        x = 0;
        y = nWidgetHeight;
        this->move(x, y);
    }
    else if ((nDesktopWidth - x) < nWidgetWidth && (nDesktopHeight - y) > nWidgetHeight)
    {
        x = nWidgetWidth;
        this->move(x, y);
    }
    else if ((nDesktopWidth - x) < nWidgetWidth && ((nDesktopHeight - y) < nWidgetHeight))
    {
        x = nWidgetWidth;
        y = nWidgetHeight;
        this->move(x, y);
    }
    else if (x > 0 && ((nDesktopHeight - y) < nWidgetHeight))
    {
        y = nWidgetHeight;
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
    QSqlQuery query(db);

    QString orgName = ui->orgName->text();

    QStringList actualPhonesList;

    for (qint32 i = 0; i < phonesList.length(); ++i)
    {
        if (i < oldPhonesList.length() && phonesList.at(i)->text() == oldPhonesList.at(i))
            actualPhonesList.append(phonesList.at(i)->text());
        else
        {
            phonesList.at(i)->setStyleSheet("border: 1px solid grey");

            actualPhonesList.append(phonesList.at(i)->text().remove(QRegularExpression("^[\\+]?[3]?[8]?")));
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

    for (qint32 i = 0; i < phonesList.length(); ++i)
    {
        if (!phonesList.at(i)->text().isEmpty())
        {
            bool old_phone = false;

            for (qint32 j = 0; j < oldPhonesList.length(); ++j)
                if (phonesList.at(i)->text() == oldPhonesList.at(j))
                    old_phone = true;

            if (!old_phone)
            {
                QString phone = phonesList.at(i)->text();

                if (isPhone(&phone))
                    phonesList.at(i)->setStyleSheet("border: 1px solid grey");
                else
                {
                    phonesList.at(i)->setStyleSheet("border: 1px solid red");

                    invalid_phones = true;
                }
            }
        }
    }

    if (invalid_phones)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Номер не соответствует формату!"), QMessageBox::Ok);

        return;
    }

    bool same_phones = false;

    for (qint32 i = 0; i < phonesList.length(); ++i)
        for (qint32 j = 0; j < phonesList.length(); ++j)
        {
            if (!phonesList.at(i)->text().isEmpty() && actualPhonesList.at(i) == actualPhonesList.at(j) && i != j)
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");
                phonesList.at(j)->setStyleSheet("border: 1px solid red");

                same_phones = true;
            }
        }

    if (same_phones)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Присутсвуют одинаковые номера!"), QMessageBox::Ok);

        return;
    }

    bool existing_phones = false;

    for (qint32 i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + actualPhonesList.at(i) + "' AND entry_id <> " + contactId + ")");
            query.exec();
            query.next();

            if (query.value(0) != 0)
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");

                existing_phones = true;
            }
        }

    if (existing_phones)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Введены существующие номера!"), QMessageBox::Ok);

        return;
    }

    bool invalid_employee = false;

    QString employee = ui->employee->text();

    if (!ui->employee->text().isEmpty())
    {
        if (g_pAsteriskManager->extensionNumbers.contains(employee) || g_pAsteriskManager->groupNumbers.contains(employee))
            ui->employee->setStyleSheet("border: 1px solid grey");
        else
        {
            ui->employee->setStyleSheet("border: 1px solid red");

            invalid_employee = true;
        }
    }

    if (invalid_employee)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Указанный номер не зарегистрирован!"), QMessageBox::Ok);

        return;
    }

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_org_name = ?, entry_city = ?, entry_address = ?, "
                  "entry_email = ?, entry_vybor_id = ?, entry_comment = ?, entry_employee = ? WHERE id = ?");
    query.addBindValue("org");
    query.addBindValue(orgName);
    query.addBindValue(orgName);
    query.addBindValue(ui->city->text());
    query.addBindValue(ui->address->text());
    query.addBindValue(ui->email->text());
    query.addBindValue(ui->vyborId->text());
    query.addBindValue(ui->comment->toPlainText().trimmed());
    query.addBindValue(ui->employee->text());
    query.addBindValue(contactId);
    query.exec();

    for (qint32 i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            if (i >= oldPhonesList.length())
            {
                query.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query.addBindValue(contactId);
                query.addBindValue(actualPhonesList.at(i));
                query.exec();
            }
            else
            {
                query.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                query.addBindValue(actualPhonesList.at(i));
                query.addBindValue(contactId);
                query.addBindValue(oldPhonesList.at(i));
                query.exec();
            }
        }

    emit sendData(true, this->pos().x(), this->pos().y());

    close();

    QMessageBox::information(this, tr("Уведомление"), tr("Запись успешно изменена!"), QMessageBox::Ok);
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
    contactId = id;

    QSqlQuery query(db);

    query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = " + contactId);
    query.exec();

    while (query.next())
        oldPhonesList.append(query.value(0).toString());

    for (qint32 i = 0; i < oldPhonesList.length(); ++i)
        phonesList.at(i)->setText(oldPhonesList.at(i));

    query.prepare("SELECT DISTINCT entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, "
                  "entry_comment, entry_employee FROM entry WHERE id = " + contactId);
    query.exec();
    query.next();

    ui->orgName->setText(query.value(0).toString());
    ui->city->setText(query.value(1).toString());
    ui->address->setText(query.value(2).toString());
    ui->email->setText(query.value(3).toString());
    ui->vyborId->setText(query.value(4).toString());
    ui->comment->setText(query.value(5).toString());
    ui->employee->setText(query.value(6).toString());
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void EditOrgContactDialog::onTextChanged()
{
    if (ui->comment->toPlainText().trimmed().length() > 255)
        ui->comment->textCursor().deletePreviousChar();
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
