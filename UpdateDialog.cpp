#include "UpdateDialog.h"
#include "ui_UpdateDialog.h"

#include <QSimpleUpdater.h>

#include <QDebug>

static const QString DEFS_URL = "http://192.168.0.30/definitions/updates.json";

UpdateDialog::UpdateDialog (QWidget* parent) :
    QDialog (parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi (this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QWidget::resize(470, 230);

    setWindowTitle (qApp->applicationName());

    /* QSimpleUpdater is single-instance */
    m_updater = QSimpleUpdater::getInstance();

    /* Check for updates when the "Check For Updates" button is clicked */
    connect (m_updater, SIGNAL (checkingFinished(QString)), this, SLOT (updateChangelog(QString)));
    connect (m_updater, SIGNAL (appcastDownloaded(QString, QByteArray)), this, SLOT (displayAppcast(QString, QByteArray)));

    /* React to button clicks */
    connect (ui->resetButton, SIGNAL (clicked()), this, SLOT (resetFields()));
    connect (ui->closeButton, SIGNAL (clicked()), this, SLOT (close()));
    connect (ui->checkButton, SIGNAL (clicked()), this, SLOT (checkForUpdates()));

    ui->installedVersion->hide();
    ui->showAllNotifcations->hide();
    ui->showUpdateNotifications->hide();
    ui->enableDownloader->hide();
    ui->customAppcast->hide();
    ui->mandatoryUpdate->hide();
    ui->resetButton->hide();
    ui->groupBox->hide();

    /* Reset the UI state */
    resetFields();
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::resetFields()
{
    ui->installedVersion->setText("3.0.2");
    ui->customAppcast->setChecked(false);
    ui->enableDownloader->setChecked(true);
    ui->showAllNotifcations->setChecked(true);
    ui->showUpdateNotifications->setChecked(true);
    ui->mandatoryUpdate->setChecked(false);
}

void UpdateDialog::checkForUpdates()
{
    /* Get settings from the UI */
    QString version = ui->installedVersion->text();
    bool customAppcast = ui->customAppcast->isChecked();
    bool downloaderEnabled = ui->enableDownloader->isChecked();
    bool notifyOnFinish = ui->showAllNotifcations->isChecked();
    bool notifyOnUpdate = ui->showUpdateNotifications->isChecked();
    bool mandatoryUpdate = ui->mandatoryUpdate->isChecked();

    /* Apply the settings */
    m_updater->setModuleVersion (DEFS_URL, version);
    m_updater->setNotifyOnFinish (DEFS_URL, notifyOnFinish);
    m_updater->setNotifyOnUpdate (DEFS_URL, notifyOnUpdate);
    m_updater->setUseCustomAppcast (DEFS_URL, customAppcast);
    m_updater->setDownloaderEnabled (DEFS_URL, downloaderEnabled);
    m_updater->setMandatoryUpdate (DEFS_URL, mandatoryUpdate);

    /* Check for updates */
    m_updater->checkForUpdates (DEFS_URL);
}

void UpdateDialog::updateChangelog (const QString& url)
{
    if (url == DEFS_URL)
        ui->changelogText->setText (m_updater->getChangelog (url));
}

void UpdateDialog::displayAppcast (const QString& url, const QByteArray& reply)
{
    if (url == DEFS_URL) {
        QString text = "" + QString::fromUtf8 (reply) + "";

        ui->changelogText->setText (text);
    }
}
