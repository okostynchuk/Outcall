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
    if(state_db == "db")
        setSettingForFirstDb();
    else if(state_db == "dbAsterisk")
        setSettingForSecondDb();

    close();
}

void DatabasesConnectDialog::setState(QString &state)
{
    if(state == "db")
    {
        state_db = state;
        ui->tabWidget_2->setCurrentIndex(0);
        ui->tabWidget_2->setTabEnabled(1, false);
    }
    else if(state == "dbAsterisk")
    {
        state_db = state;
        ui->tabWidget_2->setCurrentIndex(1);
        ui->tabWidget_2->setTabEnabled(0, false);
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
