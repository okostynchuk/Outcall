/*
 * Класс служит для добавления организации.
 */

#include "AddOrgContactDialog.h"
#include "ui_AddOrgContactDialog.h"

#include <QMessageBox>

AddOrgContactDialog::AddOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOrgContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddOrgContactDialog::onSave);

    phonesList = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    QRegularExpression regExp("^[\\+]?[0-9]*$");
    phonesValidator = new QRegularExpressionValidator(regExp, this);

    for (int i = 0; i < phonesList.length(); ++i)
        phonesList.at(i)->setValidator(phonesValidator);

    regExp.setPattern("^[0-9]*$");
    vyborIdValidator = new QRegularExpressionValidator(regExp, this);

    ui->vyborId->setValidator(vyborIdValidator);
}

AddOrgContactDialog::~AddOrgContactDialog()
{
    delete phonesValidator;
    delete vyborIdValidator;
    delete ui;
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void AddOrgContactDialog::onSave()
{
    QSqlQuery query(db);

    QString orgName = ui->orgName->text();

    QStringList phonesListRegExp;

    for (int i = 0; i < phonesList.length(); ++i)
    {
        phonesList.at(i)->setStyleSheet("border: 1px solid grey");

        phonesListRegExp.append(phonesList.at(i)->text().remove(QRegularExpression("^[\\+]?[3]?[8]?")));
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

    for (int i = 0; i < phonesList.length(); ++i)
    {
        if (!phonesList.at(i)->text().isEmpty())
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

    if (invalid_phones)
    {
        QMessageBox::critical(this, tr("Ошибка"), tr("Номер не соответствует формату!"), QMessageBox::Ok);

        return;
    }

    bool same_phones = false;

    for (int i = 0; i < phonesList.length(); ++i)
        for (int j = 0; j < phonesList.length(); ++j)
        {
            if (!phonesList.at(i)->text().isEmpty() && phonesListRegExp.at(i) == phonesListRegExp.at(j) && i != j)
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

    for (int i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + phonesListRegExp.at(i) + "')");
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

    query.prepare("INSERT INTO entry (entry_type, entry_name, entry_org_name, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment)"
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue("org");
    query.addBindValue(orgName);
    query.addBindValue(orgName);
    query.addBindValue(ui->city->text());
    query.addBindValue(ui->address->text());
    query.addBindValue(ui->email->text());
    query.addBindValue(ui->vyborId->text());
    query.addBindValue(ui->comment->toPlainText().trimmed());
    query.exec();

    int id = query.lastInsertId().toInt();

    for (int i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            query.prepare("INSERT INTO fones (entry_id, fone)"
                           "VALUES(?, ?)");
            query.addBindValue(id);
            query.addBindValue(phonesListRegExp.at(i));
            query.exec();
        }

    emit sendData(true);

    close();

    QMessageBox::information(this, tr("Уведомление"), tr("Запись успешно добавлена!"), QMessageBox::Ok);
}

/**
 * Выполняет проверку на соответсвие номера шаблону.
 */
bool AddOrgContactDialog::isPhone(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("(^[\\+][3][8][0][0-9]{9}$|^[3][8][0][0-9]{9}$|^[0][0-9]{9}$)"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Получает номер телефона неизвестной организации из классов
 * CallHistoryDialog и PopupWindow.
 */
void AddOrgContactDialog::setValues(QString number)
{
    ui->firstNumber->setText(number);
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void AddOrgContactDialog::onTextChanged()
{
    if (ui->comment->toPlainText().trimmed().length() > 255)
        ui->comment->textCursor().deletePreviousChar();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Enter.
 */
void AddOrgContactDialog::keyPressEvent(QKeyEvent* event)
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
