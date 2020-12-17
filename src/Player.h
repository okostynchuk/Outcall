#ifndef PLAYER_H
#define PLAYER_H

#include <QDialog>
#include <QMediaPlayer>

namespace Ui {
class Player;
}

class Player : public QDialog
{
    Q_OBJECT

public:
    explicit Player(QWidget* parent = 0);
    ~Player();

public slots:
    void openMedia(QString recordpath);

private slots:
    void updateTime();

    void on_play_clicked();
    void on_mute_clicked();

    void stateChanged();
    void durationChanged(qint32 duration);

    bool eventFilter(QObject* watched, QEvent* event);

private:
    Ui::Player* ui;

    QMediaPlayer* player;

    qint32 duration;
};

#endif // PLAYER_H
