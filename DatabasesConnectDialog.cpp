#include "DatabasesConnectDialog.h"
#include "ui_DatabasesConnectDialog.h"
#include "Global.h"
#include "OutCALL.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QAbstractButton>
#include <QSettings>
#include <QKeyEvent>
#include <QTabWidget>
#include <QProcess>

DatabasesConnectDialog::DatabasesConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabasesConnectDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() ^ Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &DatabasesConnectDialog::onSave);
    connect(ui->closeButton, &QAbstractButton::clicked, this, &DatabasesConnectDialog::onClose);
}

DatabasesConnectDialog::~DatabasesConnectDialog()
{
    delete ui;
}

void DatabasesConnectDialog::onSave()
{
    if(state_db == "twoDb")
    {
        checkDb();
        checkDbAsterisk();

        if(!DB.open() && !DbAsterisk.open())
            QMessageBox::critical(nullptr, "Ошибка", "Подключение не создано!", QMessageBox::Ok);
        else if(!DB.open())
        {
            ui->tabWidget_2->setCurrentIndex(0);
            ui->tabWidget_2->setTabEnabled(1, false);
            QMessageBox::critical(nullptr, "Ошибка", "Подключение к базе контактов не создано!", QMessageBox::Ok);
            setSettingForSecondDb();
        }
        else if(!DbAsterisk.open())
        {
            ui->tabWidget_2->setCurrentIndex(1);
            ui->tabWidget_2->setTabEnabled(0, false);
            QMessageBox::critical(nullptr, "Ошибка", "Подключение к базе звонков не создано!", QMessageBox::Ok);
            setSettingForFirstDb();
        }
        else if(DB.open() && DbAsterisk.open())
        {
            setSettingForFirstDb();
            setSettingForSecondDb();
            QMessageBox::information(nullptr, "Уведомление", "Подключение успешно создано!", QMessageBox::Ok);
            close();
        }
    }
    else if(state_db == "db")
    {
        checkDb();

        if(!DB.open())
            QMessageBox::critical(nullptr, "Ошибка", "Подключение к базе контактов не создано!", QMessageBox::Ok);
        else
        {
            setSettingForFirstDb();
            QMessageBox::information(nullptr, "Уведомление", "Подключение успешно создано!", QMessageBox::Ok);
            close();
        }
    }
    else if(state_db == "dbAsterisk")
    {
        checkDbAsterisk();

        if(!DbAsterisk.open())
            QMessageBox::critical(nullptr, "Ошибка", "Подключение к базе звонков не создано!", QMessageBox::Ok);
        else
        {
            setSettingForSecondDb();
            QMessageBox::information(nullptr, "Уведомление", "Подключение успешно создано!", QMessageBox::Ok);
            close();
        }
    }
}

void DatabasesConnectDialog::setSettingForFirstDb()
{
    global::setSettingsValue("hostName_1", ui->hostName_1->text(), "settings");
    global::setSettingsValue("databaseName_1",   ui->databaseName_1->text(),   "settings");
    global::setSettingsValue("userName_1",   ui->userName_1->text(),   "settings");
    QByteArray ba1;
    ba1.append(ui->password_1->text());
    global::setSettingsValue("password_1", ba1.toBase64(),    "settings");
    global::setSettingsValue("port_1", ui->port_1->text(),    "settings");
}

void DatabasesConnectDialog::setSettingForSecondDb()
{
    global::setSettingsValue("hostName_2", ui->hostName_2->text(), "settings");
    global::setSettingsValue("databaseName_2",   ui->databaseName_2->text(),   "settings");
    global::setSettingsValue("userName_2",   ui->userName_2->text(),   "settings");
    QByteArray ba2;
    ba2.append(ui->password_2->text());
    global::setSettingsValue("password_2", ba2.toBase64(),    "settings");
    global::setSettingsValue("port_2", ui->port_2->text(),    "settings");
}

void DatabasesConnectDialog::onClose()
{
     g_pAsteriskManager->signOut();
     qApp->quit();
}

void DatabasesConnectDialog::setDatabases(QSqlDatabase db, QSqlDatabase dbAsterisk, QString state)
{
    state_db = state;
    if(state_db == "db")
    {
        ui->tabWidget_2->setCurrentIndex(0);
        ui->tabWidget_2->setTabEnabled(1, false);
    }
    else if(state_db == "dbAsterisk")
    {
        ui->tabWidget_2->setCurrentIndex(1);
        ui->tabWidget_2->setTabEnabled(0, false);
    }
    DB = db;
    DbAsterisk = dbAsterisk;
}

void DatabasesConnectDialog::checkDb()
{
    DB.setHostName(ui->hostName_1->text());
    DB.setDatabaseName(ui->databaseName_1->text());
    DB.setUserName(ui->userName_1->text());
    DB.setPassword(ui->password_1->text());
    DB.setPort(ui->port_1->text().toUInt());
    DB.open();
}

void DatabasesConnectDialog::checkDbAsterisk()
{
    DbAsterisk.setHostName(ui->hostName_2->text());
    DbAsterisk.setDatabaseName(ui->databaseName_2->text());
    DbAsterisk.setUserName(ui->userName_2->text());
    DbAsterisk.setPassword(ui->password_2->text());
    DbAsterisk.setPort(ui->port_2->text().toUInt());
    DbAsterisk.open();
}
