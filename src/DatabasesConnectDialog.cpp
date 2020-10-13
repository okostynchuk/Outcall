/*
 * Класс служит для подключения к базам данных.
 */

#include "DatabasesConnectDialog.h"
#include "ui_DatabasesConnectDialog.h"
#include "Global.h"
#include "OutCALL.h"

#include <QMessageBox>
#include <QSqlDatabase>

DatabasesConnectDialog::DatabasesConnectDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DatabasesConnectDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() ^ Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QString hostName_1 = global::getSettingsValue("hostName_1", "settings").toString();
    QString databaseName_1 = global::getSettingsValue("databaseName_1", "settings").toString();
    QString userName_1 = global::getSettingsValue("userName_1", "settings").toString();
    QByteArray password1 = global::getSettingsValue("password_1", "settings").toByteArray();
    QString password_1 = QString(QByteArray::fromBase64(password1));
    QString port_1 = global::getSettingsValue("port_1", "settings").toString();

    QString hostName_2 = global::getSettingsValue("hostName_2", "settings").toString();
    QString databaseName_2 = global::getSettingsValue("databaseName_2", "settings").toString();
    QString userName_2 = global::getSettingsValue("userName_2", "settings").toString();
    QByteArray password2 = global::getSettingsValue("password_2", "settings").toByteArray();
    QString password_2 = QString(QByteArray::fromBase64(password2));
    QString port_2 = global::getSettingsValue("port_1", "settings").toString();

    ui->hostName_1->setText(hostName_1);
    ui->databaseName_1->setText(databaseName_1);
    ui->userName_1->setText(userName_1);
    ui->password_1->setText(password_1);
    ui->port_1->setText(port_1);

    ui->hostName_2->setText(hostName_2);
    ui->databaseName_2->setText(databaseName_2);
    ui->userName_2->setText(userName_2);
    ui->password_2->setText(password_2);
    ui->port_2->setText(port_2);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &DatabasesConnectDialog::onSave);
    connect(ui->closeButton, &QAbstractButton::clicked, this, &DatabasesConnectDialog::close);
}

DatabasesConnectDialog::~DatabasesConnectDialog()
{
    delete ui;
}

/**
 * Выполняет проверку введенных данных и сохраняет их.
 */
void DatabasesConnectDialog::onSave()
{
    setSettingsDb();
    setSettingsDbCalls();

    if (state == "twoDbs")
    {
        openDb();
        openDbCalls();

        if (!db.isOpen() && !dbCalls.isOpen())
            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение не создано!"), QMessageBox::Ok);
        else if (!db.isOpen())
        {
            ui->tabWidget_2->setCurrentIndex(0);
            ui->tabWidget_2->setTabEnabled(1, false);

            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе контактов не создано!"), QMessageBox::Ok);

            setSettingsDbCalls();
        }
        else if (!dbCalls.isOpen())
        {
            ui->tabWidget_2->setCurrentIndex(1);
            ui->tabWidget_2->setTabEnabled(0, false);

            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе звонков не создано!"), QMessageBox::Ok);

            setSettingsDb();
        }
        else if (db.isOpen() && dbCalls.isOpen())
        {
            setSettingsDb();
            setSettingsDbCalls();

            QMessageBox::information(nullptr, tr("Уведомление"), tr("Подключение успешно создано!"), QMessageBox::Ok);

            close();
        }
    }
    else if (state == "db")
    {
        openDb();

        if (!db.isOpen())
            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе контактов не создано!"), QMessageBox::Ok);
        else
        {
            setSettingsDb();

            QMessageBox::information(nullptr, tr("Уведомление"), tr("Подключение успешно создано!"), QMessageBox::Ok);

            close();
        }
    }
    else if (state == "dbCalls")
    {
        openDbCalls();

        if (!dbCalls.isOpen())
            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе звонков не создано!"), QMessageBox::Ok);
        else
        {
            setSettingsDbCalls();

            QMessageBox::information(nullptr, tr("Уведомление"), tr("Подключение успешно создано!"), QMessageBox::Ok);

            close();
        }
    }
}

/**
 * Выполняет сохранение в реестр данных с полей окна подключения к базе контактов.
 */
void DatabasesConnectDialog::setSettingsDb()
{
    global::setSettingsValue("hostName_1", ui->hostName_1->text(), "settings");
    global::setSettingsValue("databaseName_1",   ui->databaseName_1->text(), "settings");
    global::setSettingsValue("userName_1",   ui->userName_1->text(), "settings");
    QByteArray password_1;
    password_1.append(ui->password_1->text());
    global::setSettingsValue("password_1", password_1.toBase64(), "settings");
    global::setSettingsValue("port_1", ui->port_1->text(), "settings");
}

/**
 * Выполняет сохранение в реестр данных с полей окна подключения к базе звонков.
 */
void DatabasesConnectDialog::setSettingsDbCalls()
{
    global::setSettingsValue("hostName_2", ui->hostName_2->text(), "settings");
    global::setSettingsValue("databaseName_2", ui->databaseName_2->text(), "settings");
    global::setSettingsValue("userName_2", ui->userName_2->text(), "settings");
    QByteArray password_2;
    password_2.append(ui->password_2->text());
    global::setSettingsValue("password_2", password_2.toBase64(), "settings");
    global::setSettingsValue("port_2", ui->port_2->text(), "settings");
}

/**
 * Получает параметры баз данных из main.
 */
void DatabasesConnectDialog::setDatabases(const QSqlDatabase& db, const QSqlDatabase& dbCalls, const QString& state)
{
    this->state = state;

    if (this->state == "db")
    {
        ui->tabWidget_2->setCurrentIndex(0);
        ui->tabWidget_2->setTabEnabled(1, false);
    }
    else if (this->state == "dbCalls")
    {
        ui->tabWidget_2->setCurrentIndex(1);
        ui->tabWidget_2->setTabEnabled(0, false);
    }

    this->db = db;
    this->dbCalls = dbCalls;
}

/**
 * Выполняет установку параметров и открытие соединения с базой контактов.
 */
void DatabasesConnectDialog::openDb()
{
    db.setHostName(ui->hostName_1->text());
    db.setDatabaseName(ui->databaseName_1->text());
    db.setUserName(ui->userName_1->text());
    db.setPassword(ui->password_1->text());
    db.setPort(ui->port_1->text().toUInt());
    db.open();
}

/**
 * Выполняет установку параметров и открытие соединения с базой звонков.
 */
void DatabasesConnectDialog::openDbCalls()
{
    dbCalls.setHostName(ui->hostName_2->text());
    dbCalls.setDatabaseName(ui->databaseName_2->text());
    dbCalls.setUserName(ui->userName_2->text());
    dbCalls.setPassword(ui->password_2->text());
    dbCalls.setPort(ui->port_2->text().toUInt());
    dbCalls.open();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Enter.
 */
void DatabasesConnectDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
        onSave();
    else
        QDialog::keyPressEvent(event);
}
