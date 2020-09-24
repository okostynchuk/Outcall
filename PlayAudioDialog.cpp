/*
 * Класс служит для воспроизведения записи разговора в проигрывателе.
 */

#include "PlayAudioDialog.h"

#include <QDebug>

PlayAudioDialog::PlayAudioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayAudioDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

PlayAudioDialog::~PlayAudioDialog()
{
    delete ui;
}

/**
 * Выполняет воспроизведение аудио записи из заданного источника.
 */
void PlayAudioDialog::openMedia(QString recordpath)
{
    if (!recordpath.isEmpty())
        ui->mediaPlayer->dynamicCall("URL", "ftp://support:Lvp^^ej0@192.168.0.8/ftp_media/usb/" + recordpath.remove(0, 16));
}
