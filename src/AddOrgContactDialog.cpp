/*
 * Класс служит для добавления организации.
 */

#include "AddOrgContactDialog.h"
#include "ui_AddOrgContactDialog.h"

#include "Global.h"
#include "AsteriskManager.h"

#include <QMessageBox>

AddOrgContactDialog::AddOrgContactDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AddOrgContactDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &AddOrgContactDialog::onSave);

    m_phones = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };

    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM groups");
    query.exec();

    while(query.next())
    {
        QLineEdit* line = new QLineEdit;
        QLabel* label = new QLabel;

        m_managers.insert(query.value(0).toString(), line);
        label->setText(query.value(1).toString() + " (" + query.value(0).toString() + "):");

        ui->gridLayout->addWidget(label);
        ui->gridLayout->addWidget(line);
    }

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

AddOrgContactDialog::~AddOrgContactDialog()
{
    delete ui;
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void AddOrgContactDialog::onSave()
{
    QSqlQuery query(m_db);

    QString orgName = ui->orgName->text();

    QStringList phonesListRegExp;

    for (qint32 i = 0; i < m_phones.length(); ++i)
    {
        m_phones.at(i)->setStyleSheet("border: 1px solid grey");

        phonesListRegExp.append(m_phones.at(i)->text().remove(QRegularExpression("^[\\+]?[3]?[8]?")));
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

    if (invalid_phones)
    {
        MsgBoxError(tr("Номер не соответствует формату!"));

        return;
    }

    bool same_phones = false;

    for (qint32 i = 0; i < m_phones.length(); ++i)
        for (qint32 j = 0; j < m_phones.length(); ++j)
            if (!m_phones.at(i)->text().isEmpty() && phonesListRegExp.at(i) == phonesListRegExp.at(j) && i != j)
            {
                m_phones.at(i)->setStyleSheet("border: 1px solid red");
                m_phones.at(j)->setStyleSheet("border: 1px solid red");

                same_phones = true;
            }

    if (same_phones)
    {
        MsgBoxError(tr("Присутсвуют одинаковые номера!"));

        return;
    }

    foreach (QString key, m_managers.keys())
        m_managers.value(key)->setStyleSheet("border: 1px solid grey");

    bool existing_phones = false;

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE entry_phone = '" + phonesListRegExp.at(i) + "')");
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

    bool existing_managers = false;

    foreach (QString key, m_managers.keys())
        if (!m_managers.value(key)->text().isEmpty())
            if (!g_asteriskManager->m_extensionNumbers.keys().contains(m_managers.value(key)->text()))
            {
               m_managers.value(key)->setStyleSheet("border: 1px solid red");

                existing_managers = true;
            }

    if (existing_managers)
    {
        MsgBoxError(tr("Указанный номер менеджера не зарегистрирован!"));

        return;
    }

    bool same_managers = false;

    foreach (QString key_i, m_managers.keys())
        foreach (QString key_j, m_managers.keys())
            if (!m_managers.value(key_i)->text().isEmpty() &&  m_managers.value(key_i)->text() == m_managers.value(key_j)->text() && key_i != key_j)
            {
                m_managers.value(key_i)->setStyleSheet("border: 1px solid red");
                m_managers.value(key_j)->setStyleSheet("border: 1px solid red");

                same_managers = true;
            }

    if (same_managers)
    {
        MsgBoxError(tr("Присутсвуют одинаковые номера менеджеров!"));

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

    qint32 id = query.lastInsertId().toInt();

    for (qint32 i = 0; i < m_phones.length(); ++i)
        if (!m_phones.at(i)->text().isEmpty())
        {
            query.prepare("INSERT INTO fones (entry_id, fone)"
                           "VALUES(?, ?)");
            query.addBindValue(id);
            query.addBindValue(phonesListRegExp.at(i));
            query.exec();
        }

    foreach (QString key, m_managers.keys())
    {
        if (!m_managers.value(key)->text().isEmpty())
        {
            query.prepare("INSERT INTO managers (entry_id, group_number, manager_number)"
                           "VALUES(?, ?, ?)");
            query.addBindValue(id);
            query.addBindValue(key);
            query.addBindValue(m_managers.value(key)->text());
            query.exec();
        }
    }

    emit sendData(true);

    close();

    MsgBoxInformation(tr("Запись успешно добавлена!"));
}

/**
 * Выполняет проверку на соответсвие номера шаблону.
 */
bool AddOrgContactDialog::isPhone(QString* str)
{
    qint32 pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("(^[\\+][3][8][0][0-9]{9}$|^[3][8][0][0-9]{9}$|^[0][0-9]{9}$)"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Получает номер телефона неизвестной организации из классов
 * CallHistoryDialog и PopupWindow.
 */
void AddOrgContactDialog::setValues(const QString& number)
{
    ui->firstNumber->setText(number);
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void AddOrgContactDialog::onTextChanged()
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
