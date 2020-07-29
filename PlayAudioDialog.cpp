#include "PlayAudioDialog.h"
#include "ui_PlayAudioDialog.h"

#include <QBuffer>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QDebug>
#include <QMediaPlayer>
#include <QOperatingSystemVersion>
#include <QtWin>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QVariant>
#include <QSettings>
#include <QFileDialog>
#include <QCommandLineParser>
#include <QKeyEvent>


PlayAudioDialog::PlayAudioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayAudioDialog)
{
    ui->setupUi(this);
}

PlayAudioDialog::~PlayAudioDialog()
{
    delete ui;
}

void PlayAudioDialog::setValuesCallHistory(QString &recordpath)
{
    playAudio(recordpath);
}

void PlayAudioDialog::playAudio(QString &recordpath)
{
    openMedia("ftp://support:Lvp^^ej0@192.168.0.8/ftp_media/usb/"+recordpath);
}

void PlayAudioDialog::on_mediaPlayer_PlayStateChange(int newState)
{
    static const QHash<int, const char *> stateMapping {
        {1,  "Stopped"},
        {2,  "Paused"},
        {3,  "Playing"},
        {4,  "Scanning Forwards"},
        {5,  "Scanning Backwards"},
        {6,  "Buffering"},
        {7,  "Waiting"},
        {8,  "Media Ended"},
        {9,  "Transitioning"},
        {10, "Ready"},
        {11, "Reconnecting"},
    };
    const char *stateStr = stateMapping.value(newState, "");
    updateWindowTitle(tr(stateStr));
}

void PlayAudioDialog::openMedia(const QString &mediaUrl)
{
    if (!mediaUrl.isEmpty())
        ui->mediaPlayer->dynamicCall("URL", mediaUrl);
}

void PlayAudioDialog::updateWindowTitle(const QString &state)
{
    QString appName = QCoreApplication::applicationName();
    QString title = state.isEmpty() ? appName :
                    QString("%1 (%2)").arg(appName, state);
    setWindowTitle(title);
}

void PlayAudioDialog::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);

    emit isClosed(false);
}

void PlayAudioDialog::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Escape)
    {
        emit isClosed(false);

        QDialog::close();
    }
    else
        QWidget::keyPressEvent(event);
}
