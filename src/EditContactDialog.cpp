/*
 * Класс служит для редактирования физ. лица.
 */

#include "EditContactDialog.h"
#include "ui_EditContactDialog.h"

#include "ViewContactDialog.h"

#include <QMessageBox>
#include <QDesktopWidget>

EditContactDialog::EditContactDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::EditContactDialog)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->changeEntryTypeButton, &QAbstractButton::clicked, this, &EditContactDialog::changeEntryType);
    connect(ui->comment, &QTextEdit::cursorPositionChanged, this, &EditContactDialog::onCursorPosChanged);
    connect(ui->comment, &QTextEdit::textChanged, this, &EditContactDialog::onTextChanged);
    connect(ui->backButton, &QAbstractButton::clicked, this, &EditContactDialog::onReturn);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditContactDialog::onSave);

    m_phones = { ui->firstNumber, ui->secondNumber, ui->thirdNumber, ui->fourthNumber, ui->fifthNumber };
    m_phonesComments = { ui->firstNumberComment, ui->secondNumberComment, ui->thirdNumberComment, ui->fourthNumberComment, ui->fifthNumberComment };

    ui->region->addItems(g_regionsList);

    QSqlQuery query(m_db);

    query.prepare(QueryStringGetGroups());
    query.exec();

    while(query.next())
    {
        QLineEdit* line = new QLineEdit(this);
        QLabel* label = new QLabel(this);

        label->setMinimumHeight(20);
        line->setMinimumHeight(20);

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

EditContactDialog::~EditContactDialog()
{
    delete ui;
}

void EditContactDialog::on_phonesOrderButton_clicked()
{
    if (!m_chooseNumber.isNull())
        m_chooseNumber->close();

    m_chooseNumber = new ChooseNumber;
    connect(m_chooseNumber, &ChooseNumber::phonesOrderChanged, this, &EditContactDialog::updatePhonesOrder);
    m_chooseNumber->setValues(m_contactId, 1);
    m_chooseNumber->show();
    m_chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет сохранение позиции текстового курсора.
 */
void EditContactDialog::onCursorPosChanged()
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
bool EditContactDialog::eventFilter(QObject*, QEvent* event)
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
 * Выполняет изменение типа контакта на организацию.
 */
void EditContactDialog::changeEntryType()
{
    QSqlQuery query(m_db);

    QString lastName = ui->lastName->text();
    QString firstName = ui->firstName->text();
    QString patronymic = ui->patronymic->text();
    QString orgName = lastName + firstName + patronymic;

    qint32 msgBox = QMessageBox::information(this, tr("Предупреждение"), tr("Измененные данные не сохранятся!"
                                                                            " Вы уверены, что хотите продолжить?"), QMessageBox::Ok, QMessageBox::Cancel);

    switch (msgBox)
    {
    case QMessageBox::Ok:

        query.prepare("UPDATE entry SET entry_type = ?, entry_person_org_id = NULL, entry_org_name = ? WHERE id = ?");

        query.addBindValue("org");
        query.addBindValue(orgName);
        query.addBindValue(m_contactId);
        query.exec();

        emit sendData(true, this->pos().x(), this->pos().y());

        close();

        MsgBoxInformation(tr("Тип контакта успешно изменен!"));
        break;
    case QMessageBox::Cancel:
        return;
        break;
    default:
        break;
    }
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void EditContactDialog::onSave()
{
    QSqlQuery query(m_db);

    QString lastName = ui->lastName->text();
    QString firstName = ui->firstName->text();
    QString patronymic = ui->patronymic->text();

    QStringList actualPhonesList;
    QStringList actualPhonesCommentsList;

    for (qint32 i = 0; i < m_phones.length(); i++)
    {
        if (i < m_oldPhones.length() && m_phones.at(i)->text() == m_oldPhones.at(i))
        {
            actualPhonesList.append(m_phones.at(i)->text());
            actualPhonesCommentsList.append(m_phonesComments.at(i)->text());
        }
        else
        {
            m_phones.at(i)->setStyleSheet("border: 1px solid grey");

            actualPhonesList.append(m_phones.at(i)->text().remove(QRegularExpression("^[\\+]?[3]?[8]?")));
            actualPhonesCommentsList.append(m_phonesComments.at(i)->text());
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

    foreach (QString key, m_managers.keys())
        m_managers.value(key)->setStyleSheet("border: 1px solid grey");

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

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_person_org_id = ?, entry_person_lname = ?, "
                  "entry_person_fname = ?, entry_person_mname = ?, entry_region = ?, entry_city = ?, entry_address = ?, "
                  "entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
    query.addBindValue("person");

    if (ui->lastName->text().isEmpty())
        query.addBindValue(firstName + ' ' + patronymic);
    else
        query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic);

    if (!m_orgId.isNull())
        query.addBindValue(m_orgId);
    else
        query.addBindValue(QVariant(QVariant::Int));

    query.addBindValue(lastName);
    query.addBindValue(firstName);
    query.addBindValue(patronymic);
    query.addBindValue(ui->region->currentText());
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
                query.prepare("INSERT INTO fones (entry_id, fone, comment) VALUES(?, ?, ?)");
                query.addBindValue(m_contactId);
                query.addBindValue(actualPhonesList.at(i));
                query.addBindValue(actualPhonesCommentsList.at(i));
                query.exec();
            }
            else
            {
                query.prepare("UPDATE fones SET fone = ?, comment = ? WHERE entry_id = ? AND fone = ?");
                query.addBindValue(actualPhonesList.at(i));
                query.addBindValue(actualPhonesCommentsList.at(i));
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
                query.prepare("INSERT INTO managers (entry_id, group_number, manager_number)"
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
                query.prepare("UPDATE managers SET manager_number = ? WHERE entry_id = ? AND group_number = ?");
                query.addBindValue(m_managers.value(key)->text());
                query.addBindValue(m_contactId);
                query.addBindValue(key);
                query.exec();
            }
        }
    }

    if (!m_addOrgToPerson.isNull())
        m_addOrgToPerson->close();

    emit sendData(true, this->pos().x(), this->pos().y());

    MsgBoxInformation(tr("Запись успешно изменена!"));
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
void EditContactDialog::setValues(const QString& id)
{
    m_contactId = id;

    QSqlQuery query(m_db);

    query.prepare("SELECT DISTINCT entry_name FROM entry_phone WHERE entry_id = (SELECT DISTINCT entry_person_org_id FROM entry_phone WHERE entry_id = " + m_contactId + ")");
    query.exec();

    QString orgName;
    if (query.next())
         orgName = query.value("entry_name").toString();

    if (!orgName.isEmpty() && !orgName.isNull())
        ui->label_org->setText(orgName);
    else
        ui->label_org->setText(tr("Нет"));

    query.prepare("SELECT group_number, manager_number FROM managers WHERE entry_id = " + m_contactId);
    query.exec();

    while (query.next())
    {
        if (m_managers.keys().contains(query.value(0).toString()))
        {
            m_oldManagers.insert(query.value(0).toString(), query.value(1).toString());

            m_managers.value(query.value(0).toString())->setText(query.value(1).toString());
        }
    }

    updatePhonesOrder();

    if (m_oldPhones.length() > 1)
        ui->phonesOrderButton->setEnabled(true);

    query.prepare("SELECT DISTINCT entry_person_fname, entry_person_mname, entry_person_lname, "
                  " entry_region, entry_city, entry_address, entry_email, entry_vybor_id, entry_comment, entry_person_org_id FROM entry WHERE id = " + m_contactId);
    query.exec();
    query.next();

    ui->firstName->setText(query.value(0).toString());
    ui->patronymic->setText(query.value(1).toString());
    ui->lastName->setText(query.value(2).toString());
    ui->region->setCurrentText(query.value(3).toString());
    ui->city->setText(query.value(4).toString());
    ui->address->setText(query.value(5).toString());
    ui->email->setText(query.value(6).toString());
    ui->vyborId->setText(query.value(7).toString());
    ui->comment->setText(query.value(8).toString());

    m_orgId = query.value(9).toString();

}

void EditContactDialog::updatePhonesOrder()
{
    if (!m_oldPhones.isEmpty())
        m_oldPhones.clear();
    if (!m_oldComments.isEmpty())
        m_oldComments.clear();

    QSqlQuery query(m_db);
    query.prepare("SELECT fone, comment FROM fones WHERE entry_id = " + m_contactId + " ORDER BY priority");
    query.exec();

    while (query.next())
    {
        m_oldPhones.append(query.value(0).toString());
        m_oldComments.append(query.value(1).toString());
    }

    for (qint32 i = 0; i < m_oldPhones.length(); ++i)
        m_phones.at(i)->setText(m_oldPhones.at(i));
    for (qint32 i = 0; i < m_oldComments.length(); ++i)
        m_phonesComments.at(i)->setText(m_oldComments.at(i));
}

/**
 * Получает id и название организации из класса AddOrgToPerson
 * для последующей привязки к физ. лицу.
 */
void EditContactDialog::receiveOrg(const QString& id, const QString& name)
{
    if (!id.isNull())
    {
        ui->label_org->setText(name);

        m_orgId = id;
    }
    else
    {
        ui->label_org->setText(tr("Нет"));

        m_orgId = "0";
    }
}

/**
 * Выполняет открытие окна со списком организаций для выбора определенной на привязку.
 */
void EditContactDialog::on_addOrgButton_clicked()
{
    if (!m_addOrgToPerson.isNull())
        m_addOrgToPerson->close();

    m_addOrgToPerson = new AddOrgToPerson;
    connect(m_addOrgToPerson, &AddOrgToPerson::sendOrg, this, &EditContactDialog::receiveOrg);
    m_addOrgToPerson->show();
    m_addOrgToPerson->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет отвязку организации.
 */
void EditContactDialog::on_deleteOrgButton_clicked()
{
    ui->label_org->setText(tr("Нет"));

    m_orgId = "0";
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void EditContactDialog::onTextChanged()
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

    if (!m_addOrgToPerson.isNull())
        m_addOrgToPerson->close();
}
