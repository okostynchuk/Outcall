#ifndef PLAYAUDIODIALOG_H
#define PLAYAUDIODIALOG_H

#include "ui_PlayAudioDialog.h"
#include "Global.h"

#include <QDialog>

namespace Ui {
class PlayAudioDialog;
}

class PlayAudioDialog : public QDialog
{
    Q_OBJECT

public slots:
    void openMedia(QString);

public:
    explicit PlayAudioDialog(QWidget *parent = nullptr);
    ~PlayAudioDialog();

private:
    Ui::PlayAudioDialog *ui;
};

#endif // PLAYAUDIODIALOG_H


