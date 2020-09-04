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

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->label_6->setText("1<span style=\"color: red;\">*</span>");
    ui->label_3->setText(tr("Имя:<span style=\"color: red;\">*</span>"));

    connect(ui->Comment, &QTextEdit::textChanged, this, &EditContactDialog::onTextChanged);
    connect(ui->backButton, &QAbstractButton::clicked, this, &EditContactDialog::onReturn);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &EditContactDialog::onSave);

    phonesList = { ui->FirstNumber, ui->SecondNumber, ui->ThirdNumber, ui->FourthNumber, ui->FifthNumber };
}

EditContactDialog::~EditContactDialog()
{
    delete ui;
}

void EditContactDialog::onReturn()
{    
    emit sendData(false, this->pos().x(), this->pos().y());

    close();
}

void EditContactDialog::setPos(int x, int y)
{
    int nDesktopHeight;
    int nDesktopWidth;
    int nWidgetHeight = QWidget::height();
    int nWidgetWidth = QWidget::width();

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

void EditContactDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);

    QString lastName = QString(ui->LastName->text());
    QString firstName = QString(ui->FirstName->text());
    QString patronymic = QString(ui->Patronymic->text());

    if (QString(ui->FirstName->text()).isEmpty())
    {
         ui->label_15->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

         ui->FirstName->setStyleSheet("border: 1px solid red");

         return;
    }
    else
    {
        ui->label_15->setText(tr(""));

        ui->FirstName->setStyleSheet("border: 1px solid grey");
    }

    if (QString(ui->FirstNumber->text()).isEmpty())
    {
        ui->label_14->show();
        ui->label_14->setText(tr("<span style=\"color: red;\">Заполните обязательное поле!</span>"));

        ui->FirstNumber->setStyleSheet("border: 1px solid red");

        return;
    }
    else
    {
        ui->label_14->setText(tr(""));

        ui->FirstNumber->setStyleSheet("border: 1px solid grey");
    }

    for (int i = 0; i < phonesList.length(); ++i)
    {
        if (!phonesList.at(i)->text().isEmpty())
        {
            QString phone = QString(phonesList.at(i)->text());

            if (isPhone(&phone) && !isInternalPhone(&phone))
                phonesList.at(i)->setStyleSheet("border: 1px solid grey");
            else
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");

                QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Номер не соответствует формату!"), QMessageBox::Ok);

                return;
            }
        }
    }

    for (int i = 0; i < phonesList.length(); ++i)
        phonesList.at(i)->setStyleSheet("border: 1px solid grey");

    bool same_phones = false;

    for (int i = 0; i < phonesList.length(); ++i)
        for (int j = 0; j < phonesList.length(); j++)
        {
            if (!phonesList.at(i)->text().isEmpty() && (QString(phonesList.at(i)->text().contains(QRegularExpression("^[\\+][3][8][0]")) ? phonesList.at(i)->text().remove(QRegularExpression("^[\\+][3][8]")) : phonesList.at(i)->text()) == phonesList.at(j)->text()
                || phonesList.at(i)->text() == phonesList.at(j)->text() || QString(phonesList.at(i)->text().contains(QRegularExpression("^[3][8][0]")) ? phonesList.at(i)->text().remove(QRegularExpression("^[3][8]")) : phonesList.at(i)->text()) == phonesList.at(j)->text()
                || QString(phonesList.at(i)->text().contains(QRegularExpression("^[\\+][3][8][0]")) ? phonesList.at(i)->text().remove(QRegularExpression("^[\\+]")) : phonesList.at(i)->text()) == phonesList.at(j)->text()) && i != j)
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");
                phonesList.at(j)->setStyleSheet("border: 1px solid red");

                same_phones = true;
            }
        }

    if (same_phones)
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Присутсвуют одинаковые номера!"), QMessageBox::Ok);

        return;
    }

    if (!QString(ui->FirstName->text()).isEmpty() && !QString(ui->FirstNumber->text()).isEmpty())
    {
        ui->label_15->setText(tr(""));
        ui->label_14->setText(tr(""));

        ui->FirstName->setStyleSheet("border: 1px solid grey");

        for(int i = 0; i < phonesList.length(); ++i)
            phonesList.at(i)->setStyleSheet("border: 1px solid grey");

    }

    for (int i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            query.prepare("SELECT EXISTS (SELECT entry_phone FROM entry_phone WHERE (entry_phone = '" + phonesList.at(i)->text() + "' OR entry_phone = '" + QString(phonesList.at(i)->text().contains(QRegularExpression("^[\\+][3][8][0]")) ? phonesList.at(i)->text().remove(QRegularExpression("^[\\+][3][8]")) : QString("+38" + phonesList.at(i)->text())) + "' OR entry_phone = '" + QString(phonesList.at(i)->text().contains(QRegularExpression("^[3][8][0]")) ? phonesList.at(i)->text().remove(QRegularExpression("^[3][8]")) :QString("38" + phonesList.at(i)->text())) + "' OR entry_phone = '" + QString(phonesList.at(i)->text().contains(QRegularExpression("^[\\+][3][8][0]")) ? phonesList.at(i)->text().remove(QRegularExpression("^[\\+]")) : phonesList.at(i)->text()) + "' OR entry_phone = '" + QString(phonesList.at(i)->text().contains(QRegularExpression("^[3][8][0]")) ? QString("+" + phonesList.at(i)->text()) : phonesList.at(i)->text()) + "') AND NOT entry_id = " + updateID + ")");
            query.exec();
            query.next();

            if (query.value(0) != 0)
            {
                phonesList.at(i)->setStyleSheet("border: 1px solid red");

                QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Введены существующие номера!"), QMessageBox::Ok);

                return;
            }
        }

    QString vyborId = QString(ui->VyborID->text());

    if (!vyborId.isEmpty())
    {
        if (isVyborID(&vyborId))
            ui->VyborID->setStyleSheet("border: 1px solid grey");
        else
        {
            ui->VyborID->setStyleSheet("border: 1px solid red");

            QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("VyborID не соответствует формату!"), QMessageBox::Ok);

            return;
        }
    }

    query.prepare("UPDATE entry SET entry_type = ?, entry_name = ?, entry_person_org_id = ?, entry_person_lname = ?, entry_person_fname = ?, entry_person_mname = ?, entry_city = ?, entry_address = ?, entry_email = ?, entry_vybor_id = ?, entry_comment = ? WHERE id = ?");
    query.addBindValue("person");

    if (ui->LastName->text().isEmpty())
        query.addBindValue(firstName + ' ' + patronymic);
    else
        query.addBindValue(lastName + ' ' + firstName + ' ' + patronymic);

    QString orgName = ui->label_org->text();

    if (orgName != tr("Нет"))
    {
        QSqlQuery queryOrg(db);

        QString sqlOrg = QString("SELECT id FROM entry WHERE entry_org_name = '%1'").arg(orgName);

        queryOrg.prepare(sqlOrg);
        queryOrg.exec();

        if (queryOrg.next())
            query.addBindValue(queryOrg.value(0).toInt());
        else
        {
            QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Организации не существует или она была изменена!"), QMessageBox::Ok);

            return;
        }
    }
    else
        query.addBindValue(QVariant(QVariant::Int));

    query.addBindValue(lastName);
    query.addBindValue(firstName);
    query.addBindValue(patronymic);
    query.addBindValue(ui->City->text());
    query.addBindValue(ui->Address->text());
    query.addBindValue(ui->Email->text());
    query.addBindValue(ui->VyborID->text());
    query.addBindValue(ui->Comment->toPlainText());
    query.addBindValue(updateID);
    query.exec();


    for (int i = 0; i < phonesList.length(); ++i)
        if (!phonesList.at(i)->text().isEmpty())
        {
            if(i >= oldPhonesList.length())
            {
                query.prepare("INSERT INTO fones (entry_id, fone)"
                               "VALUES(?, ?)");
                query.addBindValue(updateID);
                query.addBindValue(phonesList.at(i)->text());
                query.exec();
            }
            else
            {
                query.prepare("UPDATE fones SET fone = ? WHERE entry_id = ? AND fone = ?");
                query.addBindValue(phonesList.at(i)->text());
                query.addBindValue(updateID);
                query.addBindValue(oldPhonesList.at(i));
                query.exec();

            }
        }

    if (!addOrgToPerson.isNull())
        addOrgToPerson.data()->close();

    emit sendData(true, this->pos().x(), this->pos().y());

    close();

    QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Запись успешно изменена!"), QMessageBox::Ok);
}

bool EditContactDialog::isInternalPhone(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("^[0-9]{4}$"));
    QRegularExpressionValidator validator2(QRegularExpression("^[2][0-9]{2}$"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool EditContactDialog::isPhone(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("(^[\\+][3][8][0][0-9]{9}$|^[0][0-9]{5,}$|^[1-9]{1}[0-9]{9,}$)"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

bool EditContactDialog::isVyborID(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[0-9]*"));

    if (validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void EditContactDialog::setValuesContacts(QString i)
{
    updateID = i;

    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT entry_phone, (SELECT DISTINCT entry_name FROM entry_phone WHERE entry_id = "
                  "(SELECT DISTINCT entry_person_org_id FROM entry_phone WHERE entry_id = " + updateID + ")) "
                  "FROM entry_phone WHERE entry_id = " + updateID);
    query.exec();

    while (query.next())
        oldPhonesList.append(query.value(0).toString());

    for (int i = 0; i < oldPhonesList.length(); ++i)
        phonesList.at(i)->setText(oldPhonesList.at(i));

    query.first();

    QString orgName = query.value(1).toString();

    if (!orgName.isEmpty() && !orgName.isNull())
        ui->label_org->setText(orgName);
    else
        ui->label_org->setText(tr("Нет"));

    query.prepare("SELECT DISTINCT entry_person_fname, entry_person_mname, entry_person_lname, "
                  " entry_city, entry_address, entry_email, entry_vybor_id, entry_comment FROM entry WHERE id = " + updateID);
    query.exec();
    query.next();

    ui->FirstName->setText(query.value(0).toString());
    ui->Patronymic->setText(query.value(1).toString());
    ui->LastName->setText(query.value(2).toString());
    ui->City->setText(query.value(3).toString());
    ui->Address->setText(query.value(4).toString());
    ui->Email->setText(query.value(5).toString());
    ui->VyborID->setText(query.value(6).toString());
    ui->Comment->setText(query.value(7).toString());
}

void EditContactDialog::receiveOrgID(QString id)
{
    QSqlDatabase db;
    QSqlQuery query(db);

    query.prepare("SELECT entry_name FROM entry_phone WHERE entry_id = " + id);
    query.exec();
    query.first();

    if (!query.value(0).toString().isEmpty())
        ui->label_org->setText(query.value(0).toString());
    else
        ui->label_org->setText(tr("Нет"));
}

void EditContactDialog::on_addOrgButton_clicked()
{
    if (!addOrgToPerson.isNull())
        addOrgToPerson.data()->close();

    addOrgToPerson = new AddOrgToPerson;
    connect(addOrgToPerson.data(), &AddOrgToPerson::sendOrgID, this, &EditContactDialog::receiveOrgID);
    addOrgToPerson.data()->show();
    addOrgToPerson.data()->setAttribute(Qt::WA_DeleteOnClose);
}

void EditContactDialog::on_deleteOrgButton_clicked()
{
    ui->label_org->setText(tr("Нет"));
}

void EditContactDialog::onTextChanged()
{
    if (ui->Comment->toPlainText().trimmed().length() > 255)
        ui->Comment->textCursor().deletePreviousChar();
}

void EditContactDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        if (ui->Comment->hasFocus())
            return;
        else
            onSave();
    }
    else
        QDialog::keyPressEvent(event);
}

void EditContactDialog::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);

    if (!addOrgToPerson.isNull())
        addOrgToPerson.data()->close();
}
