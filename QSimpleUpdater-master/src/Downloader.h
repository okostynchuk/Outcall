#ifndef DOWNLOAD_DIALOG_H
#define DOWNLOAD_DIALOG_H

#include <QDir>
#include <QDialog>
#include <ui_Downloader.h>

namespace Ui {
class Downloader;
}

class QNetworkReply;
class QNetworkAccessManager;

class Downloader : public QWidget
{
    Q_OBJECT

signals:
    void downloadFinished (const QString& url, const QString& filepath);

public:
    explicit Downloader (QWidget* parent = 0);
    ~Downloader();

    bool useCustomInstallProcedures() const;

    QString downloadDir() const;
    void setDownloadDir (const QString& downloadDir);

public slots:
    void setUrlId (const QString& url);
    void startDownload (const QUrl& url);
    void setFileName (const QString& file);
    void setUserAgentString (const QString& agent);
    void setUseCustomInstallProcedures (const bool custom);
    void setMandatoryUpdate (const bool mandatory_update);

private slots:
    void finished();
    void openDownload();
    void installUpdate();
    void cancelDownload();
    void saveFile (qint64 received, qint64 total);
    void calculateSizes (qint64 received, qint64 total);
    void updateProgress (qint64 received, qint64 total);
    void calculateTimeRemaining (qint64 received, qint64 total);

private:
    qreal round (const qreal& input);

private:
    QString m_url;
    uint m_startTime;
    QDir m_downloadDir;
    QDir m_AppDir;
    QString m_fileName;
    QString m_AppName;
    Ui::Downloader* m_ui;
    QNetworkReply* m_reply;
    QString m_userAgentString;

    bool m_useCustomProcedures;
    bool m_mandatoryUpdate;

    QNetworkAccessManager* m_manager;
};

#endif
