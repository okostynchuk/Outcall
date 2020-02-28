#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "AddExtensionDialog.h"
#include "Global.h"
#include "AsteriskManager.h"
#include "Notifier.h"

#include <QAbstractButton>
#include <QAbstractSocket>
#include <QSettings>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QTranslator>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_addExtensionDialog(nullptr)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Extensions
    connect(ui->addButton,    &QPushButton::clicked, this, &SettingsDialog::onAddButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &SettingsDialog::onRemoveButtonClicked);
    connect(ui->editButton,   &QPushButton::clicked, this, &SettingsDialog::onEditButtonClicked);
    ui->port->setValidator(new QIntValidator(0, 65535, this));

    // General
    userName = qgetenv("USERNAME");
    path = QString("C:\\Users\\%1\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup").arg(userName);

    ui->tabWidget->setCurrentIndex(0);

    loadLanguages();
    loadSettings();

    checkExten();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings()
{
    // General SettingsDialog
    global::setSettingsValue("auto_sign_in",  ui->autoSignIn->isChecked(),   "general");
    global::setSettingsValue("auto_startup",  ui->autoStartBox->isChecked(), "general");
    global::setSettingsValue("language", ui->languageList_2->currentText(), "settings");

    // Save Extension SettingsDialog
    global::setSettingsValue("servername", ui->serverName->text(), "settings");
    global::setSettingsValue("username",   ui->userName->text(),   "settings");
    QByteArray ba;
    ba.append(ui->password->text());
    global::setSettingsValue("password", ba.toBase64(),            "settings");
    global::setSettingsValue("port", ui->port->text(),    "settings");



    // Save extensions
    global::removeSettinsKey("extensions");
    int nRow = ui->treeWidget->topLevelItemCount();
    for (int i = 0; i < nRow; ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        QString extension = item->text(0);
        QString protocol = item->text(1);
        global::setSettingsValue(extension, protocol, "extensions");
    }

    // Save Databases SettingsDialog
        //Contact Base
    global::setSettingsValue("hostName_1", ui->hostName_1->text(), "settings");
    global::setSettingsValue("databaseName_1",   ui->databaseName_1->text(),   "settings");
    global::setSettingsValue("userName_1",   ui->userName_1->text(),   "settings");
    QByteArray ba1;
    ba1.append(ui->password_1->text());
    global::setSettingsValue("password_1", ba1.toBase64(),            "settings");
    global::setSettingsValue("port_1", ui->port_1->text(),    "settings");

        //Calls Base
    global::setSettingsValue("hostName_2", ui->hostName_2->text(), "settings");
    global::setSettingsValue("databaseName_2",   ui->databaseName_2->text(),   "settings");
    global::setSettingsValue("userName_2",   ui->userName_2->text(),   "settings");
    QByteArray ba2;
    ba2.append(ui->password_2->text());
    global::setSettingsValue("password_2", ba2.toBase64(),            "settings");
    global::setSettingsValue("port_2", ui->port_2->text(),    "settings");
}

void SettingsDialog::loadSettings()
{
    ui->serverName->setText(global::getSettingsValue("servername", "settings").toString());
    ui->userName->setText(global::getSettingsValue("username", "settings").toString());
    QByteArray password((global::getSettingsValue("password", "settings").toByteArray()));
    QString ba(QByteArray::fromBase64(password));
    ui->password->setText(ba);
    ui->port->setText(global::getSettingsValue("port", "settings", "5038").toString());

    // Load Databases
    ui->hostName_1->setText(global::getSettingsValue("hostName_1", "settings").toString());
    ui->databaseName_1->setText(global::getSettingsValue("databaseName_1", "settings").toString());
    ui->userName_1->setText(global::getSettingsValue("userName_1", "settings").toString());
    QByteArray password1((global::getSettingsValue("password_1", "settings").toByteArray()));
    QString ba1(QByteArray::fromBase64(password1));
    ui->password_1->setText(ba1);
    ui->port_1->setText(global::getSettingsValue("port_1", "settings").toString());

    ui->hostName_2->setText(global::getSettingsValue("hostName_2", "settings").toString());
    ui->databaseName_2->setText(global::getSettingsValue("databaseName_2", "settings").toString());
    ui->userName_2->setText(global::getSettingsValue("userName_2", "settings").toString());
    QByteArray password2((global::getSettingsValue("password_2", "settings").toByteArray()));
    QString ba2(QByteArray::fromBase64(password2));
    ui->password_2->setText(ba2);
    ui->port_2->setText(global::getSettingsValue("port_2", "settings").toString());

    // Load General SettingsDialog
    ui->autoStartBox->setChecked(global::getSettingsValue("auto_startup", "general", false).toBool());
    ui->languageList_2->setCurrentText(global::getSettingsValue("language", "settings").toString());
    bool autoSignIn = global::getSettingsValue("auto_sign_in",   "general", true).toBool();
    ui->autoSignIn->setChecked(autoSignIn);
    g_pAsteriskManager->setAutoSignIn(autoSignIn);

    // Load extensions
    QStringList extensions = global::getSettingKeys("extensions");
    int nRows              = extensions.size();
    for (int i = 0; i < nRows; ++i)
    {
        const QString extension = extensions.at(i);
        const QString protocol  = global::getSettingsValue(extension, "extensions").toString();

        QTreeWidgetItem *extensionItem = new QTreeWidgetItem(ui->treeWidget);
        extensionItem->setText(0, extension);
        extensionItem->setText(1, protocol);
    }
}

void SettingsDialog::show()
{
    ui->tabWidget->setCurrentIndex(0);
    QDialog::show();
}

void SettingsDialog::on_applyButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Применение настроек"));
    msgBox.setInformativeText(tr("Для применения изменений требуется перезапуск приложения. Подтвердить внесенные изменения?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msgBox.exec();
    switch(ret)
    {
        case QMessageBox::Yes:
            saveSettings();
            applySettings();
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
            break;
        case QMessageBox::No:
            msgBox.close();
            break;
        default:
            break;
    }
}

void SettingsDialog::on_cancelButton_clicked()
{
    QDialog::close();
}

void SettingsDialog::applySettings()
{
    if(ui->autoStartBox->isChecked())
       f.link(QApplication::applicationFilePath(), path.replace("/", "\\") + "/OutCALL.lnk");
    else
        f.remove("C:/Users/" + userName + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/OutCALL.lnk");

    g_pAsteriskManager->setAutoSignIn(global::getSettingsValue("auto_sign_in", "general", true).toBool());
    g_Notifier->emitSettingsChanged();
}

void SettingsDialog::loadLanguages()
{
        //ui->languageList_2->clear();

        ui->languageList_2->addItem(tr("Русский (default)"), "");
        ui->languageList_2->addItem(tr("Українська"), "");
        ui->languageList_2->addItem(tr("English"), "");

        QString lang = global::getSettingsValue("language", "settings").toString();
        if(lang == "")
            ui->languageList_2->setCurrentIndex(0);
        else
            ui->languageList_2->setCurrentIndex(ui->languageList_2->findData(lang, Qt::UserRole, Qt::MatchExactly));
}

QString SettingsDialog::getExtension()
{
    QStringList extensions = global::getSettingKeys("extensions");
    int nRows              = extensions.size();
    for (int i = 0; i < nRows; ++i)
    {
        const QString extension = extensions.at(i);
        return extension;
    }
    return NULL;
}

void SettingsDialog::onAddButtonClicked()
{
    m_addExtensionDialog = new AddExtensionDialog;
    m_addExtensionDialog->setWindowTitle(tr("Добавление"));
    if(m_addExtensionDialog->exec())
    {
        ui->addButton->setEnabled(false);
        QString extension = m_addExtensionDialog->getExtension();
        QString protocol = m_addExtensionDialog->getProtocol();

        QTreeWidgetItem *extensionItem = new QTreeWidgetItem();
        extensionItem->setText(0, extension);
        extensionItem->setText(1, protocol);
        extensionItem->setData(0, Qt::CheckStateRole, QVariant());

        ui->treeWidget->addTopLevelItem(extensionItem);
    }
}

void SettingsDialog::onRemoveButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();

    if (selectedItems.size() > 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr(APP_NAME),
                                      tr("Вы уверены, что хотите удалить выбранные элементы?"),
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::No)
            return;

        if (reply == QMessageBox::Yes)
            ui->addButton->setEnabled(true);

        for (int i = 0; i < selectedItems.size(); ++i)
        {
            int index = ui->treeWidget->indexOfTopLevelItem(selectedItems.at(i));
            ui->treeWidget->takeTopLevelItem(index);
        }
    }
}

void SettingsDialog::onEditButtonClicked()
{
    AddExtensionDialog editExtensionDialog;
    editExtensionDialog.setWindowTitle(tr("Редактирование"));
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();

    if (selectedItems.size())
    {
        QTreeWidgetItem *item = selectedItems.at(0);
        const QString extension = item->text(0);
        const QString protocol = item->text(1);

        editExtensionDialog.setExtension(extension);
        editExtensionDialog.setProtocol(protocol);

        if(editExtensionDialog.exec())
        {
            const QString newExtension = editExtensionDialog.getExtension();
            const QString newProtocol = editExtensionDialog.getProtocol();

            item->setText(0, newExtension);
            item->setText(1, newProtocol);
        }
    }
}

void SettingsDialog::checkExten()
{
    exten = getExtension();
    if(exten!=0)
        ui->addButton->setEnabled(false);
    else
    {
        ui->addButton->setEnabled(true);
    }
}
