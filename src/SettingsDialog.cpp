/*
 * Класс служит для настройки приложения,
 * а также сохранения настроек в реестре.
 */

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include "AddExtensionDialog.h"
#include "Global.h"
#include "Outcall.h"

#include <QSettings>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProcess>
#include <QDesktopWidget>

static const QString DEFS_URL = "http://192.168.0.30/definitions/updates.json";

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    m_geometry = saveGeometry();

    ui->autoSignIn->hide();
    ui->autoStartBox->hide();

    ui->port->setValidator(new QIntValidator(0, 65535, this));

    connect(g_asteriskManager,  &AsteriskManager::stateChanged, this, &SettingsDialog::checkAsteriskState);
    connect(ui->addButton,      &QAbstractButton::clicked,      this, &SettingsDialog::onAddButtonClicked);
    connect(ui->addButton_2,    &QAbstractButton::clicked,      this, &SettingsDialog::onAddGroupButtonClicked);
    connect(ui->editButton,     &QAbstractButton::clicked,      this, &SettingsDialog::onEditButtonClicked);
    connect(ui->editButton_2,   &QAbstractButton::clicked,      this, &SettingsDialog::onEditGroupButtonClicked);
    connect(ui->removeButton,   &QAbstractButton::clicked,      this, &SettingsDialog::onRemoveButtonClicked);
    connect(ui->removeButton_2, &QAbstractButton::clicked,      this, &SettingsDialog::onRemoveGroupButtonClicked);
    connect(ui->updateButton,   &QAbstractButton::clicked,      this, &SettingsDialog::checkForUpdates);

    loadLanguages();
    loadSettings();

    m_updater = QSimpleUpdater::getInstance();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/**
 * Выполняет проверку на наличие доступного обновления.
 */
void SettingsDialog::checkForUpdates()
{
    m_updater->setModuleVersion(DEFS_URL, qApp->applicationVersion());
    m_updater->setNotifyOnFinish(DEFS_URL, true);
    m_updater->setNotifyOnUpdate(DEFS_URL, true);
    m_updater->setUseCustomAppcast(DEFS_URL, false);
    m_updater->setDownloaderEnabled(DEFS_URL, true);
    m_updater->setMandatoryUpdate(DEFS_URL, false);
    m_updater->checkForUpdates(DEFS_URL);
}

/**
 * Выполняет проверку соединения с Asterisk
 * для того, чтобы управлять доступом к функционалу.
 */
void SettingsDialog::checkAsteriskState(const AsteriskManager::AsteriskState& state)
{
    if (state == AsteriskManager::CONNECTED)
    {
        ui->tabWidget->setTabEnabled(3, true);
        ui->updateButton->setDisabled(false);
    }
    else
    {
        ui->tabWidget->setTabEnabled(3, false);
        ui->updateButton->setDisabled(true);
    }
}

/**
 * Выполняет обработку закрытия окна.
 */
void SettingsDialog::closeEvent(QCloseEvent*)
{
    hide();

    QDialog::clearFocus();

    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget_2->setCurrentIndex(0);
    ui->tabWidget_3->setCurrentIndex(0);

    ui->languageList->setCurrentText(global::getSettingsValue("language", "settings").toString());

    restoreGeometry(m_geometry);

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);
    move(screen.center() - rect().center());
}

/**
 * Выполняет сохранение настроек приложения в реестре.
 */
void SettingsDialog::saveSettings()
{
    // General
    //global::setSettingsValue("auto_sign_in",  ui->autoSignIn->isChecked(), "general");
    //global::setSettingsValue("auto_startup",  ui->autoStartBox->isChecked(), "general");
    global::setSettingsValue("language", ui->languageList->currentText(), "settings");

    // Server
    global::setSettingsValue("servername", ui->serverName->text(), "settings");
    global::setSettingsValue("username",   ui->userName->text(), "settings");
    QByteArray ba;
    ba.append(ui->password->text().toLatin1());
    global::setSettingsValue("password", ba.toBase64(), "settings");
    global::setSettingsValue("port", ui->port->text(), "settings");

    // Databases
        // Contact Base
    global::setSettingsValue("hostName_1", ui->hostName_1->text(),         "settings");
    global::setSettingsValue("databaseName_1", ui->databaseName_1->text(), "settings");
    global::setSettingsValue("userName_1", ui->userName_1->text(),         "settings");
    QByteArray ba1;
    ba1.append(ui->password_1->text().toLatin1());
    global::setSettingsValue("password_1", ba1.toBase64(), "settings");
    global::setSettingsValue("port_1", ui->port_1->text(), "settings");

        // Call Base
    global::setSettingsValue("hostName_2", ui->hostName_2->text(),         "settings");
    global::setSettingsValue("databaseName_2", ui->databaseName_2->text(), "settings");
    global::setSettingsValue("userName_2", ui->userName_2->text(),         "settings");
    QByteArray ba2;
    ba2.append(ui->password_2->text().toLatin1());
    global::setSettingsValue("password_2", ba2.toBase64(), "settings");
    global::setSettingsValue("port_2", ui->port_2->text(), "settings");

        // Client Base
    global::setSettingsValue("hostName_3", ui->hostName_3->text(), "settings");
    global::setSettingsValue("databaseName_3", ui->databaseName_3->text(), "settings");
    global::setSettingsValue("userName_3", ui->userName_3->text(), "settings");
    QByteArray ba3;
    ba3.append(ui->password_3->text().toLatin1());
    global::setSettingsValue("password_3", ba3.toBase64(), "settings");
    global::setSettingsValue("port_3", ui->port_3->text(), "settings");
    global::setSettingsValue("user_login", ui->user_login->text(), "settings");

    // Personal number
    global::removeSettingsKey("extensions");

    qint32 rows = ui->treeWidget->topLevelItemCount();

    if (rows == 0)
        global::removeSettingsKey("extensions_name");

    for (qint32 i = 0; i < rows; ++i)
    {
        QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
        QString extension = item->text(0);
        QString protocol = item->text(1);

        global::setSettingsValue(extension, protocol, "extensions");

        if (g_asteriskManager->isSignedIn())
        {
            global::removeSettingsKey("extensions_name");
            global::setSettingsValue(extension, g_asteriskManager->m_extensionNumbers.value(extension), "extensions_name");
        }
    }

    // Group's number
    global::removeSettingsKey("group_extensions");

    rows = ui->treeWidget_2->topLevelItemCount();

    for (qint32 i = 0; i < rows; ++i)
    {
        QTreeWidgetItem* group_item = ui->treeWidget_2->topLevelItem(i);
        QString group_extension = group_item->text(0);
        QString group_protocol = group_item->text(1);

        global::setSettingsValue(group_extension, group_protocol, "group_extensions");
    }
}

/**
 * Выполняет получение настроек приложения из реестра и заполнение необходимых полей ввода
 * (параметров подключения к базам данных, язык интерфейса и т.д.).
 */
void SettingsDialog::loadSettings()
{
    // General
    ui->autoStartBox->setChecked(global::getSettingsValue("auto_startup", "general", true).toBool());
    ui->languageList->setCurrentText(global::getSettingsValue("language", "settings").toString());
    bool autoSignIn = global::getSettingsValue("auto_sign_in", "general", true).toBool();
    ui->autoSignIn->setChecked(autoSignIn);
    g_asteriskManager->setAutoSignIn(autoSignIn);

    // Server
    ui->serverName->setText(global::getSettingsValue("servername", "settings").toString());
    ui->userName->setText(global::getSettingsValue("username", "settings").toString());
    QByteArray password((global::getSettingsValue("password", "settings").toByteArray()));
    QString ba(QByteArray::fromBase64(password));
    ui->password->setText(ba);
    ui->port->setText(global::getSettingsValue("port", "settings").toString());

    // Databases
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

    ui->hostName_3->setText(global::getSettingsValue("hostName_3", "settings").toString());
    ui->databaseName_3->setText(global::getSettingsValue("databaseName_3", "settings").toString());
    ui->userName_3->setText(global::getSettingsValue("userName_3", "settings").toString());
    QByteArray password3((global::getSettingsValue("password_3", "settings").toByteArray()));
    QString ba3(QByteArray::fromBase64(password3));
    ui->password_3->setText(ba3);
    ui->user_login->setText(global::getSettingsValue("user_login", "settings").toString());
    ui->port_3->setText(global::getSettingsValue("port_3", "settings").toString());

    // Personal number
    QStringList extensions = global::getSettingKeys("extensions");

    if (extensions.size() == 0)
        ui->addButton->setEnabled(true);
    else
    {
        ui->addButton->setEnabled(false);

        for (qint32 i = 0; i < extensions.size(); ++i)
        {
            const QString extension = extensions.at(i);
            const QString protocol  = global::getSettingsValue(extension, "extensions").toString();

            QTreeWidgetItem* extensionItem = new QTreeWidgetItem(ui->treeWidget);
            extensionItem->setText(0, extension);
            extensionItem->setText(1, protocol);
        }
    }

    // Group's number
    QStringList group_extensions = global::getSettingKeys("group_extensions");

    if (group_extensions.size() == 0)
        ui->addButton_2->setEnabled(true);
    else
    {
        ui->addButton_2->setEnabled(false);

        for (qint32 i = 0; i < group_extensions.size(); ++i)
        {
            const QString group_extension = group_extensions.at(i);
            const QString group_protocol  = global::getSettingsValue(group_extension, "group_extensions").toString();

            QTreeWidgetItem* group_extensionItem = new QTreeWidgetItem(ui->treeWidget_2);
            group_extensionItem->setText(0, group_extension);
            group_extensionItem->setText(1, group_protocol);
        }
    }
}

/**
 * Выполняет операции для последующего применения настроек
 * или же отмены их применения.
 */
void SettingsDialog::on_applyButton_clicked()
{
    QMessageBox msgBox;    
    msgBox.setText(tr("Применение настроек"));
    msgBox.setInformativeText(tr("Для применения изменений требуется перезапуск приложения. Подтвердить внесенные изменения?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, tr("Да"));
    msgBox.setButtonText(QMessageBox::No, tr("Нет"));
    qint32 reply = msgBox.exec();

    switch (reply)
    {
    case QMessageBox::Yes:
        saveSettings();
        applySettings();

        emit restart(true);

        qApp->closeAllWindows();
        qApp->quit();

        QProcess::startDetached(qApp->arguments()[0], QStringList() << "restart");
        break;
    case QMessageBox::No:
        msgBox.close();
        break;
    default:
        break;
    }
}

/**
 * Выполняет закрытие окна при нажатии кнопки "Отменить".
 */
void SettingsDialog::on_cancelButton_clicked()
{
    QDialog::close();
}

/**
 * Выполняет применение настроек приложения
 * (включение / отключение автозагрузки и автоподключения).
 */
void SettingsDialog::applySettings()
{
//    QFile file;
//    QSettings settings("Microsoft\\Windows\\CurrentVersion", "Explorer");
//    settings.beginGroup("Shell Folders");
//    QFile path = settings.value("Startup").toString();
//    if (ui->autoStartBox->isChecked())
//        file.link(QApplication::applicationFilePath(), path.replace("/", "\\") + "/" + QString(APP_NAME) + ".lnk");
//    else
//        file.remove(path.replace("/", "\\") + "/" + QString(APP_NAME) + ".lnk");

//    g_asteriskManager->setAutoSignIn(global::getSettingsValue("auto_sign_in", "general", true).toBool());
}

/**
 * Выполняет загрузку языка интерфейса приложения.
 */
void SettingsDialog::loadLanguages()
{
    QString ruPath(":/images/ru.png");
    QIcon ruIcon = (QIcon(ruPath));
    QString ruLabel = (tr("Русский"));

    QString uaPath(":/images/ua.png");
    QIcon uaIcon = (QIcon(uaPath));
    QString uaLabel = (tr("Українська"));

    QString ukPath(":/images/uk.png");
    QIcon ukIcon = (QIcon(ukPath));
    QString ukLabel = (tr("English"));

    ui->languageList->addItem(ruIcon, ruLabel);
    ui->languageList->addItem(uaIcon, uaLabel);
    ui->languageList->addItem(ukIcon, ukLabel);

    QString lang = global::getSettingsValue("language", "settings").toString();

    if (lang == "")
        ui->languageList->setCurrentIndex(0);
    else
        ui->languageList->setCurrentIndex(ui->languageList->findData(lang, Qt::UserRole, Qt::MatchExactly));
}

/**
 * Выполняет добавление личного номера.
 */
void SettingsDialog::onAddButtonClicked()
{
    AddExtensionDialog* addExtensionDialog = new AddExtensionDialog;
    addExtensionDialog->setWindowTitle(tr("Добавление"));

    if (addExtensionDialog->exec())
    {
        ui->addButton->setEnabled(false);

        QString extension = addExtensionDialog->getExtension();
        QString protocol = addExtensionDialog->getProtocol();

        QTreeWidgetItem* extensionItem = new QTreeWidgetItem();
        extensionItem->setText(0, extension);
        extensionItem->setText(1, protocol);
        extensionItem->setData(0, Qt::CheckStateRole, QVariant());

        ui->treeWidget->addTopLevelItem(extensionItem);
    }

    addExtensionDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет добавление номера группы.
 */
void SettingsDialog::onAddGroupButtonClicked()
{
    AddExtensionDialog* addExtensionDialog = new AddExtensionDialog;
    addExtensionDialog->setWindowTitle(tr("Добавление"));

    if (addExtensionDialog->exec())
    {
        ui->addButton_2->setEnabled(false);

        QString group_extension = addExtensionDialog->getExtension();
        QString group_protocol = addExtensionDialog->getProtocol();

        QTreeWidgetItem *group_extensionItem = new QTreeWidgetItem();
        group_extensionItem->setText(0, group_extension);
        group_extensionItem->setText(1, group_protocol);
        group_extensionItem->setData(0, Qt::CheckStateRole, QVariant());

        ui->treeWidget_2->addTopLevelItem(group_extensionItem);
    }

    addExtensionDialog->setAttribute(Qt::WA_DeleteOnClose);
}

/**
 * Выполняет удаление личного номера.
 */
void SettingsDialog::onRemoveButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();

    if (selectedItems.size() > 0)
    {
        QMessageBox msgBox;       
        msgBox.setText(tr("Удаление номера"));
        msgBox.setInformativeText(tr("Вы уверены, что хотите удалить выбранный номер?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, tr("Да"));
        msgBox.setButtonText(QMessageBox::No, tr("Нет")); 
        qint32 reply = msgBox.exec();

        switch (reply)
        {
        case QMessageBox::Yes:
            ui->addButton->setEnabled(true);
            break;
        case QMessageBox::No:
            msgBox.close();
            return;
        default:
            break;
        }

        for (qint32 i = 0; i < selectedItems.size(); ++i)
        {
            qint32 index = ui->treeWidget->indexOfTopLevelItem(selectedItems.at(i));
            ui->treeWidget->takeTopLevelItem(index);
        }
    }
}

/**
 * Выполняет удаление номера группы.
 */
void SettingsDialog::onRemoveGroupButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_2->selectedItems();

    if (selectedItems.size() > 0)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Удаление номера"));
        msgBox.setInformativeText(tr("Вы уверены, что хотите удалить выбранный номер?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setButtonText(QMessageBox::Yes, tr("Да"));
        msgBox.setButtonText(QMessageBox::No, tr("Нет"));
        qint32 reply = msgBox.exec();

        switch (reply)
        {
        case QMessageBox::Yes:
            ui->addButton_2->setEnabled(true);
            break;
        case QMessageBox::No:
            msgBox.close();
            return;
        default:
            break;
        }

        for (qint32 i = 0; i < selectedItems.size(); ++i)
        {
            qint32 index = ui->treeWidget_2->indexOfTopLevelItem(selectedItems.at(i));
            ui->treeWidget_2->takeTopLevelItem(index);
        }
    }
}

/**
 * Выполняет редактирование личного номера.
 */
void SettingsDialog::onEditButtonClicked()
{
    AddExtensionDialog editExtensionDialog;
    editExtensionDialog.setWindowTitle(tr("Редактирование"));

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget->selectedItems();

    if (selectedItems.size())
    {
        QTreeWidgetItem* item = selectedItems.at(0);
        const QString extension = item->text(0);
        const QString protocol = item->text(1);

        editExtensionDialog.setExtension(extension);
        editExtensionDialog.setProtocol(protocol);

        if (editExtensionDialog.exec())
        {
            const QString newExtension = editExtensionDialog.getExtension();
            const QString newProtocol = editExtensionDialog.getProtocol();

            item->setText(0, newExtension);
            item->setText(1, newProtocol);
        }
    }
}

/**
 * Выполняет редактирование номера группы.
 */
void SettingsDialog::onEditGroupButtonClicked()
{
    AddExtensionDialog editExtensionDialog;
    editExtensionDialog.setWindowTitle(tr("Редактирование"));

    QList<QTreeWidgetItem*> selectedItems = ui->treeWidget_2->selectedItems();

    if (selectedItems.size())
    {
        QTreeWidgetItem* item = selectedItems.at(0);
        const QString group_extension = item->text(0);
        const QString group_protocol = item->text(1);

        editExtensionDialog.setExtension(group_extension);
        editExtensionDialog.setProtocol(group_protocol);

        if (editExtensionDialog.exec())
        {
            const QString newExtension = editExtensionDialog.getExtension();
            const QString newProtocol = editExtensionDialog.getProtocol();

            item->setText(0, newExtension);
            item->setText(1, newProtocol);
        }
    }
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиш Esc и Enter.
 */
void SettingsDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else if (event->key() == Qt::Key_Return)
        ui->applyButton->click();
    else
        QDialog::keyPressEvent(event);
}
