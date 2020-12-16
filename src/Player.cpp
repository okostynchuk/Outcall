#include "Player.h"
#include "ui_Player.h"

#include <QMouseEvent>
#include <QTime>

Player::Player(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->slider->installEventFilter(this);
    ui->volume->installEventFilter(this);

    player = new QMediaPlayer(this);
    player->setVolume(100);

    connect(player, &QMediaPlayer::durationChanged, ui->slider, &QSlider::setMaximum);
    connect(player, &QMediaPlayer::positionChanged, ui->slider, &QSlider::setValue);

    connect(player, &QMediaPlayer::volumeChanged,   ui->volume, &QSlider::setValue);

    connect(player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &Player::updateTime);
    connect(player, &QMediaPlayer::stateChanged,    this, &Player::stateChanged);

    connect(ui->slider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    connect(ui->volume, &QSlider::sliderMoved, player, &QMediaPlayer::setVolume);
}

Player::~Player()
{
    delete ui;
}

void Player::openMedia(QString recordpath)
{
    QString path = "ftp://support:Lvp^^ej0@192.168.0.8/ftp_media/usb/" + recordpath.remove(0, 16);
    player->setMedia(QUrl(path));
    player->play();
}

void Player::durationChanged(qint64 duration)
{
    this->duration = duration/1000;
}

void Player::updateTime()
{
    ui->duration->setText(QDateTime::fromTime_t(player->position()/1000).toUTC().toString("mm:ss") + " / " + QDateTime::fromTime_t(duration).toUTC().toString("mm:ss"));
}

void Player::stateChanged()
{
    if (player->state() == QMediaPlayer::StoppedState)
        ui->play->setIcon(QIcon(":/images/play.png"));
}

void Player::on_play_clicked()
{
    if (player->state() == QMediaPlayer::PlayingState)
    {
        player->pause();

        ui->play->setIcon(QIcon(":/images/play.png"));
    }
    else
    {
        player->play();
        ui->play->setIcon(QIcon(":/images/pause.png"));
    }
}

void Player::on_mute_clicked()
{
    if (player->isMuted())
    {
        player->setMuted(false);
        ui->mute->setIcon(QIcon(":/images/volume.png"));
    }
    else
    {
        player->setMuted(true);
        ui->mute->setIcon(QIcon(":/images/mute.png"));
    }
}

bool Player::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->slider && event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        player->setPosition(QStyle::sliderValueFromPosition(ui->slider->minimum(), ui->slider->maximum(), mouseEvent->x(), ui->slider->width()));
    }

    if (watched == ui->volume && event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        player->setVolume(QStyle::sliderValueFromPosition(ui->volume->minimum(), ui->volume->maximum(), mouseEvent->x(), ui->volume->width()));

        if (player->isMuted())
        {
            player->setMuted(false);
            ui->mute->setIcon(QIcon(":/images/volume.png"));
        }
    }

    return false;
}
