#include "OutCALL.h"
#include "Global.h"
#include "Notifier.h"

#include "Windows.h"

#include <QApplication>
#include <QLocalSocket>
#include <QDir>
#include <QTextCodec>
#include <QSqlQuery>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.0.30");
    db.setDatabaseName("test");
    db.setUserName("test");
    db.setPassword("testtest");
    db.setPort(3306);
    db.open();

    QSqlDatabase dbAsterisk = QSqlDatabase::addDatabase("QMYSQL","Second");
    dbAsterisk.setHostName("192.168.0.8");
    dbAsterisk.setDatabaseName("cdr");
    dbAsterisk.setUserName("SMDR");
    dbAsterisk.setPassword("20SMDR19p");
    dbAsterisk.setPort(3306);
    dbAsterisk.open();

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion("2.0");
    app.setOrganizationName(ORGANIZATION_NAME);

    bool bCallRequest = false;

    g_AppDirPath = QApplication::applicationDirPath();

    if (argc==2 && QString(argv[1]) == "installer")
    {
        // Setup log file paths
        QDir().mkpath(g_AppSettingsFolderPath);
    }

    if (argc == 2)
    {
        bCallRequest = QString(argv[1]).contains("Dial#####");
    }

    if (bCallRequest)
    {
        QStringList arguments = QString(argv[1]).split("#####");
        QString contactName = arguments[1];

        contactName.replace("&&&", " ");

        return 0;
    }

    Notifier notifier;

    QString username  = global::getSettingsValue("username", "settings").toString();
    QByteArray secret = global::getSettingsValue("password", "settings").toByteArray();
    AsteriskManager manager(username, QString(QByteArray::fromBase64(secret)));

    OutCall outcall;
    outcall.show();

    return app.exec();
}
