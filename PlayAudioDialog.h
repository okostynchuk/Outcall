#ifndef PLAYAUDIODIALOG_H
#define PLAYAUDIODIALOG_H

#include "ui_PlayAudioDialog.h"
#include "Global.h"

#include <QDialog>
#include <QMediaPlayer>

namespace Ui {
class PlayAudioDialog;
}

static const char geometryKey[] = "Geometry";

class PlayAudioDialog : public QDialog
{
    Q_OBJECT

public slots:
    void on_mediaPlayer_PlayStateChange(int newState);

public:
    explicit PlayAudioDialog(QWidget *parent = nullptr);
    ~PlayAudioDialog();

    void setValuesCallHistory(QString);
    void openMedia(const QString &mediaUrl);

private slots:
    void updateWindowTitle(const QString &state);

private:
    Ui::PlayAudioDialog *ui;

    QMediaPlayer* player;

    QString recordpath;
};

#endif // PLAYAUDIODIALOG_H


