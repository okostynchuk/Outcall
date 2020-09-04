#ifndef _QSIMPLEUPDATER_MAIN_H
#define _QSIMPLEUPDATER_MAIN_H

#include <QUrl>
#include <QList>
#include <QObject>

#if defined (QSU_SHARED)
    #define QSU_DECL Q_DECL_EXPORT
#elif defined (QSU_IMPORT)
    #define QSU_DECL Q_DECL_IMPORT
#else
    #define QSU_DECL
#endif

class Updater;

class QSU_DECL QSimpleUpdater : public QObject
{
    Q_OBJECT

signals:
    void checkingFinished(const QString& url);
    void appcastDownloaded(const QString& url, const QByteArray& data);
    void downloadFinished(const QString& url, const QString& filepath);

public:
    static QSimpleUpdater* getInstance();

    bool usesCustomAppcast(const QString& url) const;
    bool getNotifyOnUpdate(const QString& url) const;
    bool getNotifyOnFinish(const QString& url) const;
    bool getUpdateAvailable(const QString& url) const;
    bool getDownloaderEnabled(const QString& url) const;
    bool usesCustomInstallProcedures(const QString& url) const;

    QString getOpenUrl(const QString& url) const;
    QString getChangelog(const QString& url) const;
    QString getModuleName(const QString& url) const;
    QString getDownloadUrl(const QString& url) const;
    QString getPlatformKey(const QString& url) const;
    QString getLatestVersion(const QString& url) const;
    QString getModuleVersion(const QString& url) const;
    QString getUserAgentString(const QString& url) const;

public slots:
    void checkForUpdates(const QString& url);
    void setModuleName(const QString& url, const QString& name);
    void setNotifyOnUpdate(const QString& url, const bool notify);
    void setNotifyOnFinish(const QString& url, const bool notify);
    void setPlatformKey(const QString& url, const QString& platform);
    void setModuleVersion(const QString& url, const QString& version);
    void setDownloaderEnabled(const QString& url, const bool enabled);
    void setUserAgentString(const QString& url, const QString& agent);
    void setUseCustomAppcast(const QString& url, const bool customAppcast);
    void setUseCustomInstallProcedures(const QString& url, const bool custom);
    void setMandatoryUpdate(const QString& url, const bool mandatory_update);

protected:
    ~QSimpleUpdater();

private:
    Updater* getUpdater(const QString& url) const;
};

#endif
