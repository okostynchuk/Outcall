#include "Player.h"
#include "ui_Player.h"

#include <QMouseEvent>
#include <QTime>
#include <QStyle>

Player::Player(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->slider->installEventFilter(this);
    ui->volume->installEventFilter(this);

    m_mediaPlayer = new QMediaPlayer(this);
    m_mediaPlayer->setVolume(80);

    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, ui->slider, &QSlider::setMaximum);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, ui->slider, &QSlider::setValue);

    connect(m_mediaPlayer, &QMediaPlayer::volumeChanged,   ui->volume, &QSlider::setValue);

    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &Player::updateTime);
    connect(m_mediaPlayer, &QMediaPlayer::stateChanged,    this, &Player::stateChanged);

    connect(ui->slider, &QSlider::sliderMoved, m_mediaPlayer, &QMediaPlayer::setPosition);
    connect(ui->volume, &QSlider::sliderMoved, m_mediaPlayer, &QMediaPlayer::setVolume);
}

Player::~Player()
{
    delete ui;
}

void Player::openMedia(QString recordpath)
{
    QString path = "ftp://support:Lvp^^ej0@192.168.0.8/ftp_media/usb/" + recordpath.remove(0, 16);

    m_mediaPlayer->setMedia(QUrl(path));
    m_mediaPlayer->play();
}

void Player::durationChanged(const qint32 duration)
{
    m_duration = duration / 1000;
}

void Player::updateTime()
{
    ui->duration->setText(QDateTime::fromTime_t(m_mediaPlayer->position() / 1000).toUTC().toString("mm:ss") + " / " + QDateTime::fromTime_t(m_duration).toUTC().toString("mm:ss"));
}

void Player::stateChanged()
{
    if (m_mediaPlayer->state() == QMediaPlayer::StoppedState)
        ui->play->setIcon(QIcon(":/images/play.png"));
}

void Player::on_play_clicked()
{
    if (m_mediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        m_mediaPlayer->pause();

        ui->play->setIcon(QIcon(":/images/play.png"));
    }
    else
    {
        m_mediaPlayer->play();

        ui->play->setIcon(QIcon(":/images/pause.png"));
    }
}

void Player::on_mute_clicked()
{
    if (m_mediaPlayer->isMuted())
    {
        m_mediaPlayer->setMuted(false);

        ui->mute->setIcon(QIcon(":/images/volume.png"));
    }
    else
    {
        m_mediaPlayer->setMuted(true);

        ui->mute->setIcon(QIcon(":/images/mute.png"));
    }
}

bool Player::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->slider && event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        m_mediaPlayer->setPosition(QStyle::sliderValueFromPosition(ui->slider->minimum(), ui->slider->maximum(), mouseEvent->x(), ui->slider->width()));
    }

    if (watched == ui->volume && event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        m_mediaPlayer->setVolume(QStyle::sliderValueFromPosition(ui->volume->minimum(), ui->volume->maximum(), mouseEvent->x(), ui->volume->width()));

        if (m_mediaPlayer->isMuted())
        {
            m_mediaPlayer->setMuted(false);

            ui->mute->setIcon(QIcon(":/images/volume.png"));
        }
    }

    return false;
}
