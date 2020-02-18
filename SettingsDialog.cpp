#include "SettingsDialog.h"
#include "ui_settingsdialog.h"
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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_addExtensionDialog(nullptr)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::handleButtonBox);

    // Extensions
    connect(ui->addButton,    &QPushButton::clicked, this, &SettingsDialog::onAddButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &SettingsDialog::onRemoveButtonClicked);
    connect(ui->editButton,   &QPushButton::clicked, this, &SettingsDialog::onEditButtonClicked);
    ui->port->setValidator(new QIntValidator(0, 65535, this));

    // General
    userName = qgetenv("USERNAME");
    path = QString("C:\\Users\\%1\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup").arg(userName);
    loadSettings();
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

    // Save Extension SettingsDialog
    global::setSettingsValue("servername", ui->serverName->text(), "settings");
    global::setSettingsValue("username",   ui->userName->text(),   "settings");
    QByteArray ba;
    ba.append(ui->password->text());
    global::setSettingsValue("password", ba.toBase64(),            "settings");
    global::setSettingsValue("port", ui->port->text().toUInt(),    "settings");

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
}

void SettingsDialog::loadSettings()
{
    ui->serverName->setText(global::getSettingsValue("servername", "settings").toString());
    ui->userName->setText(global::getSettingsValue("username", "settings").toString());
    QByteArray password((global::getSettingsValue("password", "settings").toByteArray()));
    QString ba(QByteArray::fromBase64(password));
    ui->password->setText(ba);
    ui->port->setText(global::getSettingsValue("port", "settings", "5038").toString());

    // Load General SettingsDialog
    ui->autoStartBox->setChecked(global::getSettingsValue("auto_startup", "general", false).toBool());
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

void SettingsDialog::handleButtonBox(QAbstractButton *button)
{
    if(ui->buttonBox->standardButton(button) == QDialogButtonBox::Ok)
    {
        okPressed();
    }
    else if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Apply)
    {
        applyPressed();
    }
}

void SettingsDialog::okPressed()
{
    saveSettings();
    applySettings();
    hide();
}

void SettingsDialog::applyPressed()
{
    saveSettings();
    applySettings();
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

/********************************************/
/****************General*********************/
/********************************************/

/********************************************/
/****************Extensions******************/
/********************************************/

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
    AddExtensionDialog addExtensionDialog;
    addExtensionDialog.setWindowTitle("Добавление");
    if(addExtensionDialog.exec())
    {
        QString extension = addExtensionDialog.getExtension();
        QString protocol = addExtensionDialog.getProtocol();

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
    editExtensionDialog.setWindowTitle("Редактирование");
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

//****************************************************//
//**********************Dialing rules*****************//
//****************************************************//
