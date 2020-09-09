#include "PlayAudioDialog.h"
#include "ui_PlayAudioDialog.h"

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

void PlayAudioDialog::setValuesCallHistory(QString recordpath)
{
    recordpath.remove(0, 16);

    openMedia("ftp://support:Lvp^^ej0@192.168.0.8/ftp_media/usb/" + recordpath);
}

void PlayAudioDialog::on_mediaPlayer_PlayStateChange(int newState)
{
    static const QHash<int, const char*> stateMapping
    {
        {1,  QT_TR_NOOP("Остановлено")},
        {2,  QT_TR_NOOP("Пауза")},
        {3,  QT_TR_NOOP("Воспроизведение")},
        {4,  QT_TR_NOOP("Перемотка вперед")},
        {5,  QT_TR_NOOP("Перемотка назад")},
        {6,  QT_TR_NOOP("Буферизация")},
        {7,  QT_TR_NOOP("Ожидание")},
        {8,  QT_TR_NOOP("Конец файла")},
        {9,  QT_TR_NOOP("Переход")},
        {10, QT_TR_NOOP("Готово")},
        {11, QT_TR_NOOP("Переподключение")},
    };

    const char* stateStr = stateMapping.value(newState, "");

    updateWindowTitle(stateStr);
}

void PlayAudioDialog::openMedia(const QString &mediaUrl)
{
    if (!mediaUrl.isEmpty())
        ui->mediaPlayer->dynamicCall("URL", mediaUrl);
}

void PlayAudioDialog::updateWindowTitle(const QString &state)
{
    QString title = state.isEmpty() ? QString(APP_NAME) :
                    QString("%1 (%2)").arg(QString(APP_NAME), state);

    setWindowTitle(title);
}
