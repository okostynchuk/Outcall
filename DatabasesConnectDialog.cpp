#include "DatabasesConnectDialog.h"
#include "ui_DatabasesConnectDialog.h"

#include "Global.h"
#include "OutCALL.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSettings>
#include <QTabWidget>
#include <QProcess>

DatabasesConnectDialog::DatabasesConnectDialog(QWidget *parent) :
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

    connect(ui->saveButton, &QPushButton::clicked, this, &DatabasesConnectDialog::onSave);
    connect(ui->closeButton, &QPushButton::clicked, this, &DatabasesConnectDialog::onClose);
}

DatabasesConnectDialog::~DatabasesConnectDialog()
{
    delete ui;
}

void DatabasesConnectDialog::onSave()
{
    setSettingForFirstDb();
    setSettingForSecondDb();

    if (state_db == "twoDb")
    {
        checkDb();
        checkDbAsterisk();

        if (!DB.isOpen() && !DbAsterisk.isOpen())
            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение не создано!"), QMessageBox::Ok);
        else if (!DB.isOpen())
        {
            ui->tabWidget_2->setCurrentIndex(0);
            ui->tabWidget_2->setTabEnabled(1, false);

            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе контактов не создано!"), QMessageBox::Ok);

            setSettingForSecondDb();
        }
        else if (!DbAsterisk.isOpen())
        {
            ui->tabWidget_2->setCurrentIndex(1);
            ui->tabWidget_2->setTabEnabled(0, false);

            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе звонков не создано!"), QMessageBox::Ok);

            setSettingForFirstDb();
        }
        else if (DB.isOpen() && DbAsterisk.isOpen())
        {
            setSettingForFirstDb();
            setSettingForSecondDb();

            QMessageBox::information(nullptr, tr("Уведомление"), tr("Подключение успешно создано!"), QMessageBox::Ok);

            close();
        }
    }
    else if (state_db == "db")
    {
        checkDb();

        if (!DB.isOpen())
            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе контактов не создано!"), QMessageBox::Ok);
        else
        {
            setSettingForFirstDb();

            QMessageBox::information(nullptr, tr("Уведомление"), tr("Подключение успешно создано!"), QMessageBox::Ok);

            close();
        }
    }
    else if (state_db == "dbCalls")
    {
        checkDbAsterisk();

        if (!DbAsterisk.isOpen())
            QMessageBox::critical(nullptr, tr("Ошибка"), tr("Подключение к базе звонков не создано!"), QMessageBox::Ok);
        else
        {
            setSettingForSecondDb();

            QMessageBox::information(nullptr, tr("Уведомление"), tr("Подключение успешно создано!"), QMessageBox::Ok);

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

void DatabasesConnectDialog::setDatabases(QSqlDatabase db, QSqlDatabase dbCalls, QString state)
{
    state_db = state;

    if (state_db == "db")
    {
        ui->tabWidget_2->setCurrentIndex(0);
        ui->tabWidget_2->setTabEnabled(1, false);
    }
    else if (state_db == "dbCalls")
    {
        ui->tabWidget_2->setCurrentIndex(1);
        ui->tabWidget_2->setTabEnabled(0, false);
    }

    DB = db;
    DbAsterisk = dbCalls;
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
    DbAsterisk.setPassword( ui->password_2->text() );
    DbAsterisk.setPort( ui->port_2->text().toUInt() );
    DbAsterisk.open();
}

void DatabasesConnectDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
        onSave();
    else
        QDialog::keyPressEvent(event);
}
