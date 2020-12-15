#include "Player.h"
#include "ui_Player.h"

#include <QMouseEvent>

Player::Player(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    ui->slider->installEventFilter(this);

    player = new QMediaPlayer(this);
    player->setVolume(100);

    connect(player, &QMediaPlayer::durationChanged, ui->slider, &QSlider::setMaximum);
    connect(player, &QMediaPlayer::positionChanged, ui->slider, &QSlider::setValue);
    connect(ui->slider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
}

Player::~Player()
{
    delete player;

    delete ui;
}

void Player::openMedia(QString recordpath)
{
    QString path = "ftp://support:Lvp^^ej0@192.168.0.8/ftp_media/usb/" + recordpath.remove(0, 16);
    player->setMedia(QUrl(path));
    player->play();
}

void Player::on_play_clicked()
{
    if (play)
    {
        play = false;

        player->pause();
        ui->play->setIcon(QIcon(":/images/play.png"));
    }
    else
    {
        play = true;

        player->play();
        ui->play->setIcon(QIcon(":/images/pause.png"));
    }
}

void Player::on_volume_sliderMoved(int position)
{
    player->setVolume(position);
}

void Player::on_mute_clicked()
{
    if (player->isMuted())
    {
        player->setMuted(false);
        ui->mute->setIcon(QIcon(":/images/mute.png"));
    }
    else
    {
        player->setMuted(true);
        ui->mute->setIcon(QIcon(":/images/volume.png"));
    }
}

bool Player::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->slider && event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        player->setPosition(QStyle::sliderValueFromPosition(ui->slider->minimum(), ui->slider->maximum(), mouseEvent->x(), ui->slider->width()));
    }

    return false;
}
