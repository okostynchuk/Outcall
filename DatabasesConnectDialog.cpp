#include "DatabasesConnectDialog.h"
#include "ui_DatabasesConnectDialog.h"
#include "Global.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QAbstractButton>
#include <QAbstractSocket>
#include <QSettings>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDir>

DatabasesConnectDialog::DatabasesConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabasesConnectDialog)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QAbstractButton::clicked, this, &DatabasesConnectDialog::onSave);
}

DatabasesConnectDialog::~DatabasesConnectDialog()
{
    delete ui;
}

void DatabasesConnectDialog::onSave()
{
//    QSqlDatabase db;
//    QSqlDatabase dbAsterisk = QSqlDatabase::database("Second");

//    global::setSettingsValue("hostName_1", ui->hostName_1->text(), "settings");
//    global::setSettingsValue("databaseName_1",   ui->databaseName_1->text(),   "settings");
//    global::setSettingsValue("userName_1",   ui->userName_1->text(),   "settings");
//    QByteArray ba1;
//    ba1.append(ui->password_1->text());
//    global::setSettingsValue("password-1", ba1.toBase64(),            "settings");
//    global::setSettingsValue("port-1", ui->port_1->text().toUInt(),    "settings");

//    global::setSettingsValue("hostName_2", ui->hostName_2->text(), "settings");
//    global::setSettingsValue("databaseName_2",   ui->databaseName_2->text(),   "settings");
//    global::setSettingsValue("userName_2",   ui->userName_2->text(),   "settings");
//    QByteArray ba2;
//    ba2.append(ui->password_2->text());
//    global::setSettingsValue("password-2", ba2.toBase64(),            "settings");
//    global::setSettingsValue("port-2", ui->port_2->text().toUInt(),    "settings");

//    close();
}

