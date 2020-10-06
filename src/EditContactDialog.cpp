/*
 * Класс служит для редактирования физ. лица.
 */

#include "EditContactDialog.h"
#include "ui_EditContactDialog.h"

#include "ViewContactDialog.h"

#include <QMessageBox>
#include <QDesktopWidget>

EditContactDialog::EditContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditContactDialog)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->comment, &QTextEdit::textChanged, this, &EditContactDialog::onTextChanged);
    connect(ui->comment, &QTextEdit::cursorPositionChanged, this, &EditContactDialog::onCursorPosChanged);
    connect(ui->backButton, &QAbstractButton::clicked, this, &EditContactDialog::onReturn);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditContactDialog::onSave);

    phonesList = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    QRegularExpression regExp("^[\\+]?[0-9]*$");
    phonesValidator = new QRegularExpressionValidator(regExp, this);

    for (qint32 i = 0; i < phonesList.length(); ++i)
        phonesList.at(i)->setValidator(phonesValidator);

    regExp.setPattern("^[0-9]*$");
    vyborIdValidator = new QRegularExpressionValidator(regExp, this);

    ui->vyborId->setValidator(vyborIdValidator);
}

EditContactDialog::~EditContactDialog()
{
    delete phonesValidator;
    delete vyborIdValidator;
    delete ui;
}

/**
 * Выполняет сохранение позиции текстового курсора.
 */
void EditContactDialog::onCursorPosChanged()
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
bool EditContactDialog::eventFilter(QObject*, QEvent* event)
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
 * Выполняет закрытие окна и отправку данных в класс ViewContactDialog.
 */
void EditContactDialog::onReturn()
{    
    emit sendData(false, this->pos().x(), this->pos().y());

    close();
}

/**
 * Выполняет установку позиции окна в зависимости от позиции окна-родителя ViewContactDialog.
 */
void EditContactDialog::setPos(qint32 x, qint32 y)
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
void EditContactDialog::onSave()
{
    QSqlQuery query(db);

    QString lastName = ui->lastName->text();
    QString firstName = ui->firstName->text();
    QString patronymic = ui->patronymic->text();

    QStringList actualPhonesList;

    for (qint32 i = 0; i < phonesList.length(); i++)
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

    if (ui->firstName->text().isEmpty())
    {
         ui->label_15->setText("<span style=\"color: red;\">" + tr("Заполните обязательное поле!") + "</span>");

         ui->firstName->setStyleSheet("border: 1px solid red");

         empty_field = true;
    }
    else
    {
        ui->label_15->setText("");

        ui->firstName->setStyleSheet("border: 1px solid grey");
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

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_person_org_id = ?, entry_person_lname = ?, entry_person_fname = ?, entry_person_mname = ?, entry_city = ?, entry_address = ?, entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
    query.addBindValue("person");

    if (ui->lastName->text().isEmpty())
        query.addBindValue(firstName + ' ' + patronymic);
    else
        query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic);

    if (!orgId.isNull())
        query.addBindValue(orgId);
    else
        query.addBindValue(QVariant(QVariant::Int));

    query.addBindValue(lastName);
    query.addBindValue(firstName);
    query.addBindValue(patronymic);
    query.addBindValue(ui->city->text());
    query.addBindValue(ui->address->text());
    query.addBindValue(ui->email->text());
    query.addBindValue(ui->vyborId->text());
    query.addBindValue(ui->comment->toPlainText().trimmed());
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

    if (!addOrgToPerson.isNull())
        addOrgToPerson.data()->close();

    emit sendData(true, this->pos().x(), this->pos().y());

    close();

    QMessageBox::information(this, tr("Уведомление"), tr("Запись успешно изменена!"), QMessageBox::Ok);
}

/**
 * Выполняет проверку на соответсвие номера шаблону.
 */
bool EditContactDialog::isPhone(QString* str)
{
    qint32 pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("(^[\\+][3][8][0][0-9]{9}$|^[3][8][0][0-9]{9}$|^[0][0-9]{9}$)"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Получает и заполняет поля окна необходимыми данными.
 * Получает id контакта из классов CallHistoryDialog, ViewContactDialog.
 */
void EditContactDialog::setValues(QString id)
{
    contactId = id;

    QSqlQuery query(db);

    query.prepare("SELECT entry_phone, (SELECT DISTINCT entry_name FROM entry_phone WHERE entry_id = "
                  "(SELECT DISTINCT entry_person_org_id FROM entry_phone WHERE entry_id = " + contactId + ")) "
                  "FROM entry_phone WHERE entry_id = " + contactId);
    query.exec();

    while (query.next())
        oldPhonesList.append(query.value(0).toString());

    for (qint32 i = 0; i < oldPhonesList.length(); ++i)
        phonesList.at(i)->setText(oldPhonesList.at(i));

    query.first();

    QString orgName = query.value(1).toString();

    if (!orgName.isEmpty() && !orgName.isNull())
        ui->label_org->setText(orgName);
    else
        ui->label_org->setText(tr("Нет"));

    query.prepare("SELECT DISTINCT entry_person_fname, entry_person_mname, entry_person_lname, "
                  " entry_city, entry_address, entry_email, entry_vybor_id, entry_comment, entry_person_org_id FROM entry WHERE id = " + contactId);
    query.exec();
    query.next();

    ui->firstName->setText(query.value(0).toString());
    ui->patronymic->setText(query.value(1).toString());
    ui->lastName->setText(query.value(2).toString());
    ui->city->setText(query.value(3).toString());
    ui->address->setText(query.value(4).toString());
    ui->email->setText(query.value(5).toString());
    ui->vyborId->setText(query.value(6).toString());
    ui->comment->setText(query.value(7).toString());

    orgId = query.value(8).toString();
}

/**
 * Получает id и название организации из класса AddOrgToPerson
 * для последующей привязки к физ. лицу.
 */
void EditContactDialog::receiveOrgName(QString id, QString name)
{
    if (!id.isNull())
    {
        ui->label_org->setText(name);

        orgId = id;
    }
    else
    {
        ui->label_org->setText(tr("Нет"));

        orgId = "0";
    }
}

/**
 * Выполняет открытие окна со списком организаций для выбора определенной на привязку.
 */
void EditContactDialog::on_addOrgButton_clicked()
{
    if (!addOrgToPerson.isNull())
        addOrgToPerson.data()->close();

    addOrgToPerson = new AddOrgToPerson;
    connect(addOrgToPerson.data(), &AddOrgToPerson::sendOrgName, this, &EditContactDialog::receiveOrgName);
    addOrgToPerson.data()->show();
    addOrgToPerson.data()->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет отвязку организации.
 */
void EditContactDialog::on_deleteOrgButton_clicked()
{
    ui->label_org->setText(tr("Нет"));

    orgId = "0";
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void EditContactDialog::onTextChanged()
{
    if (ui->comment->toPlainText().trimmed().length() > 255)
        ui->comment->textCursor().deletePreviousChar();
}

/**
 * Выполняет скрытие кнопки возврата к карточке физ. лица.
 */
void EditContactDialog::hideBackButton()
{
    ui->backButton->hide();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиш Esc и Enter.
 */
void EditContactDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else if (event->key() == Qt::Key_Return)
    {
        if (ui->comment->hasFocus())
            return;
        else
            onSave();
    }
    else
        QDialog::keyPressEvent(event);
}

/**
 * Выполняет обработку закрытия окна.
 */
void EditContactDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    if (!addOrgToPerson.isNull())
        addOrgToPerson.data()->close();
}
