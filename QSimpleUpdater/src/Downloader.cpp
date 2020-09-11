#include <QDir>
#include <QFile>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QNetworkAccessManager>

#include <math.h>

#include "Downloader.h"
#include "Global.h"

static const QString PARTIAL_DOWN(".part");

Downloader::Downloader(QWidget* parent) : QWidget(parent)
{
    m_ui = new Ui::Downloader;
    m_ui->setupUi(this);

    /* Initialize private members */
    m_manager = new QNetworkAccessManager();

    /* Initialize internal values */
    m_url = "";
    m_fileName = "";
    m_startTime = 0;
    m_useCustomProcedures = false;
    m_mandatoryUpdate = false;

    /* Set download directory */
    m_downloadDir.setPath(QDir::homePath() + "/Downloads/");

    m_AppDir.setPath("C:\\");

    /* Make the window look like a modal dialog */
    setWindowIcon(QIcon());
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    /* Configure the appearance and behavior of the buttons */
    m_ui->stopButton->hide();
    connect(m_ui->stopButton, &QAbstractButton::clicked, this, &Downloader::cancelDownload);
    //connect(m_ui->openButton, &QAbstractButton::clicked, this, &Downloader::installUpdate);

    /* Resize to fit */
    setFixedSize(minimumSizeHint());
}

Downloader::~Downloader()
{
    delete m_ui;
    delete m_reply;
    delete m_manager;
}

/**
 * Returns \c true if the updater shall not intervene when the download has
 * finished (you can use the \c QSimpleUpdater signals to know when the
 * download is completed).
 */
bool Downloader::useCustomInstallProcedures() const
{
    return m_useCustomProcedures;
}

/**
 * Changes the URL, which is used to indentify the downloader dialog
 * with an \c Updater instance
 *
 * \note the \a url parameter is not the download URL, it is the URL of
 *       the AppCast file
 */
void Downloader::setUrlId(const QString& url)
{
    m_url = url;
}

/**
 * Begins downloading the file at the given \a url
 */
void Downloader::startDownload(const QUrl& url)
{
    /* Reset UI */
    m_ui->progressBar->setValue(0);
    m_ui->stopButton->setText(tr("Отмена"));
    m_ui->downloadLabel->setText(tr("Скачивание обновления"));
    m_ui->timeLabel->setText(tr("Времени осталось") + ": " + tr("неизвестно"));

    /* Configure the network request */
    QNetworkRequest request(url);
    if (!m_userAgentString.isEmpty())
        request.setRawHeader("", m_userAgentString.toUtf8());

    /* Start download */
    m_reply = m_manager->get(request);
    m_startTime = QDateTime::currentDateTime().toTime_t();

    /* Ensure that downloads directory exists */
    if (!m_downloadDir.exists())
        m_downloadDir.mkpath(".");

    /* Remove old downloads */
    QFile::remove(m_downloadDir.filePath(m_fileName));
    QFile::remove(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));

    /* Update UI when download progress changes or download finishes */
    connect(m_reply, &QNetworkReply::downloadProgress, this, &Downloader::updateProgress);
    connect(m_reply, &QNetworkReply::finished, this, &Downloader::finished);
    //connect(m_reply, &QNetworkReply::redirected, this, &Downloader::startDownload);

    showNormal();
}

/**
 * Changes the name of the downloaded file
 */
void Downloader::setFileName(const QString& file)
{
    m_fileName = file;

    if (m_fileName.isEmpty())
        m_fileName = "QSU_Update.bin";
}

/**
 * Changes the user-agent string used to communicate with the remote HTTP server
 */
void Downloader::setUserAgentString(const QString& agent)
{
    m_userAgentString = agent;
}

void Downloader::finished()
{
    /* Rename file */
    QFile::rename(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN),
                  m_downloadDir.filePath (m_fileName));

    /* Notify application */
    emit downloadFinished(m_url, m_downloadDir.filePath(m_fileName));

    /* Install the update */
    m_reply->close();
    installUpdate();
    setVisible(false);
}

/**
 * Opens the downloaded file.
 * \note If the downloaded file is not found, then the function will alert the
 *       user about the error.
 */
void Downloader::openDownload()
{
    if (!m_fileName.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(m_downloadDir.filePath(m_fileName)));
        qApp->closeAllWindows();
        qApp->quit();
    }

    else
    {
        QMessageBox::critical(this,
                               tr("Ошибка"),
                               tr("Файл обновления не найден!"),
                               QMessageBox::Close);
    }

    QDir dir("C:\\OutCALL");
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    int fileAmount = dir.count();

    QStringList namesOfDirectories;
    namesOfDirectories = dir.entryList();

    for (int i = 0; i < fileAmount; ++i)
        QFile::rename("C:\\OutCALL\\" + namesOfDirectories.at(i), "C:\\OutCALL\\" + PARTIAL_DOWN + namesOfDirectories.at(i));
}

/**
 * Instructs the OS to open the downloaded file.
 *
 * \note If \c useCustomInstallProcedures() returns \c true, the function will
 *       not instruct the OS to open the downloaded file. You can use the
 *       signals fired by the \c QSimpleUpdater to install the update with your
 *       own implementations/code.
 */
void Downloader::installUpdate()
{
    if (useCustomInstallProcedures())
        return;

    /* Update labels */
    m_ui->stopButton->setText(tr("Закрыть"));
    m_ui->downloadLabel->setText(tr("Загрузка завершена!"));
    m_ui->timeLabel->setText(tr("Установщик откроется отдельно"));

    /* Ask the user to install the download */
    QMessageBox box;
    box.setIcon(QMessageBox::Question);
    box.setDefaultButton(QMessageBox::Ok);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    box.setButtonText(QMessageBox::Yes, tr("ОК"));
    box.setButtonText(QMessageBox::Cancel, tr("Отмена"));
    box.setInformativeText(tr("Нажмите \"OK\" для начала установки обновления"));

    QString text = tr("Для установки обновления необходимо закрыть приложение");

    if (m_mandatoryUpdate)
        text = tr("Для установки обновления необходимо закрыть приложение. Это обязательное обновление, программа будет закрыта!");

    box.setText("<h3>" + text + "</h3>");

    /* User wants to install the download */
    if (box.exec() == QMessageBox::Ok)
    {
        if (!useCustomInstallProcedures())
        {
            openDownload();
        }

    }
    /* Wait */
    else
    {
        if (m_mandatoryUpdate)
            QApplication::quit();

        m_ui->timeLabel->setText(tr("Нажмите кнопку \"Открыть\" для применения обновлений"));
    }
}

/**
 * Prompts the user if he/she wants to cancel the download and cancels the
 * download if the user agrees to do that.
 */
void Downloader::cancelDownload()
{
    if (!m_reply->isFinished())
    {
        QMessageBox box;
        box.setWindowTitle(tr ("Окно обновлений"));
        box.setIcon(QMessageBox::Question);
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setButtonText(QMessageBox::Yes, tr("Да"));
        box.setButtonText(QMessageBox::No, tr("Нет"));

        QString text = tr("Вы уверены, что хотите отменить установку?");
        if (m_mandatoryUpdate)
        {
            text = tr("Вы уверены, что хотите отменить установку? Это обязательное обновление!");
        }
        box.setText(text);

        if (box.exec() == QMessageBox::Yes)
        {
            hide();
            m_reply->abort();
            close();
            if (m_mandatoryUpdate)
                QApplication::quit();
        }
    }
    else
    {
        if (m_mandatoryUpdate)
            QApplication::quit();

        hide();
    }
}

/**
 * Writes the downloaded data to the disk
 */
void Downloader::saveFile(qint64 received, qint64 total)
{
    Q_UNUSED (received);
    Q_UNUSED (total);

    /* Check if we need to redirect */
    QUrl url = m_reply->attribute(
                   QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty())
    {
        startDownload(url);
        return;
    }

    /* Save downloaded data to disk */
    QFile file(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));

    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        file.write(m_reply->readAll());
        file.close();
    }
}


/**
 * Calculates the appropiate size units (bytes, KB or MB) for the received
 * data and the total download size. Then, this function proceeds to update the
 * dialog controls/UI.
 */
void Downloader::calculateSizes(qint64 received, qint64 total)
{
    QString totalSize;
    QString receivedSize;

    if (total < 1024)
        totalSize = tr("%1 байтов").arg(total);

    else if (total < 1048576)
        totalSize = tr("%1 KB").arg(round(total / 1024));

    else
        totalSize = tr("%1 MB").arg(round(total / 1048576));

    if (received < 1024)
        receivedSize = tr("%1 байтов").arg(received);

    else if (received < 1048576)
        receivedSize = tr("%1 KB").arg(received / 1024);

    else
        receivedSize = tr("%1 MB").arg(received / 1048576);

    m_ui->downloadLabel->setText(tr("Скачивание обновлений")
                                  + " (" + receivedSize + " " + tr("из")
                                  + " " + totalSize + ")");
}

/**
 * Uses the \a received and \a total parameters to get the download progress
 * and update the progressbar value on the dialog.
 */
void Downloader::updateProgress(qint64 received, qint64 total)
{
    if (total > 0)
    {
        m_ui->progressBar->setMinimum(0);
        m_ui->progressBar->setMaximum(100);
        m_ui->progressBar->setValue((received * 100) / total);

        calculateSizes(received, total);
        calculateTimeRemaining(received, total);
        saveFile(received, total);
    }

    else
    {
        m_ui->progressBar->setMinimum(0);
        m_ui->progressBar->setMaximum(0);
        m_ui->progressBar->setValue(-1);
        m_ui->downloadLabel->setText(tr("Скачивание обновлений") + "...");
        m_ui->timeLabel->setText(QString("%1: %2")
                                  .arg(tr("Времени осталось"))
                                  .arg(tr("Неизвестно")));
    }
}

/**
 * Uses two time samples (from the current time and a previous sample) to
 * calculate how many bytes have been downloaded.
 *
 * Then, this function proceeds to calculate the appropiate units of time
 * (hours, minutes or seconds) and constructs a user-friendly string, which
 * is displayed in the dialog.
 */
void Downloader::calculateTimeRemaining(qint64 received, qint64 total)
{
    uint difference = QDateTime::currentDateTime().toTime_t() - m_startTime;

    if (difference > 0)
    {
        QString timeString;
        qreal timeRemaining = (total - received) / (received / difference);

        if (timeRemaining > 7200)
        {
            timeRemaining /= 3600;
            int hours = int(timeRemaining + 0.5);

            if (hours > 1)
                timeString = tr("примерно %1 часа(-ов)").arg(hours);
            else
                timeString = tr("примерно 1 час");
        }

        else if (timeRemaining > 60)
        {
            timeRemaining /= 60;
            int minutes = int(timeRemaining + 0.5);

            if (minutes > 1)
                timeString = tr("%1 минут(-ы)").arg(minutes);
            else
                timeString = tr("1 минута");
        }

        else if (timeRemaining <= 60)
        {
            int seconds = int(timeRemaining + 0.5);

            if (seconds > 1)
                timeString = tr("%1 секунд(-ы)").arg(seconds);
            else
                timeString = tr("1 секунда");
        }

        m_ui->timeLabel->setText(tr("Времени осталось") + ": " + timeString);
    }
}

/**
 * Rounds the given \a input to two decimal places
 */
qreal Downloader::round(const qreal& input)
{
    return static_cast<qreal>(roundf(static_cast<float>(input) * 100) / 100);
}

QString Downloader::downloadDir() const
{
    return m_downloadDir.absolutePath();
}

void Downloader::setDownloadDir(const QString& downloadDir)
{
    if (m_downloadDir.absolutePath() != downloadDir)
        m_downloadDir.setPath(downloadDir);
}

/**
 * If the \a mandatory_update is set to \c true, the \c Downloader has to download and install the
 * update. If the user cancels or exits, the application will close
 */
void Downloader::setMandatoryUpdate(const bool mandatory_update)
{
    m_mandatoryUpdate = mandatory_update;
}

/**
 * If the \a custom parameter is set to \c true, then the \c Downloader will not
 * attempt to open the downloaded file.
 *
 * Use the signals fired by the \c QSimpleUpdater to implement your own install
 * procedures.
 */
void Downloader::setUseCustomInstallProcedures(const bool custom)
{
    m_useCustomProcedures = custom;
}
