#include "PlayAudioDialog.h"
#include "ui_PlayAudioDialog.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QVariant>
#include <QSettings>
#include <QFileDialog>
#include <QCommandLineParser>

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
    QString languages = global::getSettingsValue("language", "settings").toString();

    if (languages == "Русский (по умолчанию)")
    {
        static const QHash<int, const char*> stateMapping
        {
            {1,  "Остановленно"},
            {2,  "Пауза"},
            {3,  "Воспроизведение"},
            {4,  "Перемотка вперед"},
            {5,  "Перемотка назад"},
            {6,  "Буферизация"},
            {7,  "Ожидание"},
            {8,  "Конец файла"},
            {9,  "Переход"},
            {10, "Готов"},
            {11, "Переподключение"},
        };

        const char* stateStr = stateMapping.value(newState, "");

        updateWindowTitle(stateStr);
    }
    else if (languages == "English")
    {
        static const QHash<int, const char*> stateMapping
        {
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

        const char* stateStr = stateMapping.value(newState, "");

        updateWindowTitle(stateStr);
    }
    else if (languages == "Українська")
    {
        static const QHash<int, const char*> stateMapping
        {
            {1,  "Перепідключення"},
            {2,  "Пауза"},
            {3,  "Відтворення"},
            {4,  "Промотування вперед"},
            {5,  "Промотування назад"},
            {6,  "Буферизація"},
            {7,  "Очікування"},
            {8,  "Кінець файлу"},
            {9,  "Перехід"},
            {10, "Готовий"},
            {11, "Перепідключення"},
        };

        const char* stateStr = stateMapping.value(newState, "");

        updateWindowTitle(stateStr);
    }
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
