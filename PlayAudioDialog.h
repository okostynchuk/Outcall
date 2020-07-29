#ifndef PLAYAUDIODIALOG_H
#define PLAYAUDIODIALOG_H

#include <QDialog>
#include <QMediaPlayer>

#include "ui_PlayAudioDialog.h"

namespace Ui {
class PlayAudioDialog;
}

static const char geometryKey[] = "Geometry";

class PlayAudioDialog : public QDialog
{
    Q_OBJECT

signals:
    void isClosed(bool);

public:
    explicit PlayAudioDialog(QWidget *parent = nullptr);
    ~PlayAudioDialog();
    void setValuesCallHistory(QString &);
    void openMedia(const QString &mediaUrl);

protected slots:
    void playAudio(QString &);

public slots:
    void on_mediaPlayer_PlayStateChange(int newState);

private slots:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent* event);

private:
    Ui::PlayAudioDialog *ui;
    void updateWindowTitle(const QString &state);
    QMediaPlayer *player;
    QString recordpath;
};

#endif // PLAYAUDIODIALOG_H


