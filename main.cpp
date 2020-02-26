#include "OutCALL.h"
#include "Global.h"
#include "Notifier.h"
#include "Windows.h"
#include "DatabasesConnectDialog.h"

#include <QApplication>
#include <QProcess>
#include <QLocalSocket>
#include <QDir>
#include <QTextCodec>
#include <QSqlQuery>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication app(argc, argv);
    g_LanguagesPath = QApplication::applicationDirPath() + "/translations";
    g_AppSettingsFolderPath = QDir::homePath() + "/OutCALL";
    g_AppDirPath = QApplication::applicationDirPath();

    QString hostName_1 = global::getSettingsValue("hostName_1", "settings").toString();
    QString databaseName_1 = global::getSettingsValue("databaseName_1", "settings").toString();
    QString userName_1 = global::getSettingsValue("userName_1", "settings").toString();
    QByteArray password1 = global::getSettingsValue("password_1", "settings").toByteArray();
    QString password_1 = QString(QByteArray::fromBase64(password1));
    QString port_1 = global::getSettingsValue("port_1", "settings").toString();

    db.setHostName(hostName_1);
    db.setDatabaseName(databaseName_1);
    db.setUserName(userName_1);
    db.setPassword(password_1);
    db.setPort(port_1.toInt());
    db.open();

    QString hostName_2 = global::getSettingsValue("hostName_2", "settings").toString();
    QString databaseName_2 = global::getSettingsValue("databaseName_2", "settings").toString();
    QString userName_2 = global::getSettingsValue("userName_2", "settings").toString();
    QByteArray password2 = global::getSettingsValue("password_2", "settings").toByteArray();
    QString password_2 = QString(QByteArray::fromBase64(password2));
    QString port_2 = global::getSettingsValue("port_2", "settings").toString();

    dbAsterisk.setHostName(hostName_2);
    dbAsterisk.setDatabaseName(databaseName_2);
    dbAsterisk.setUserName(userName_2);
    dbAsterisk.setPassword(password_2);
    dbAsterisk.setPort(port_2.toInt());
    dbAsterisk.open();

    if(!db.open() && !dbAsterisk.open())
    {
        QString state = "twoDb";
        QMessageBox::critical(nullptr, "Ошибка", "Отсутствует подключение к базам данных!", QMessageBox::Ok);
        DatabasesConnectDialog *databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbAsterisk, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();
    }
    else if(!db.open())
    {
        QString state = "db";
        QMessageBox::critical(nullptr, "Ошибка", "Отсутствует подключение к базе контактов!", QMessageBox::Ok);
        DatabasesConnectDialog *databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbAsterisk, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();

    }
    else if(!dbAsterisk.open())
    {
        QString state = "dbAsterisk";
        QMessageBox::critical(nullptr, "Ошибка", "Отсутствует подключение к базе звонков!", QMessageBox::Ok);
        DatabasesConnectDialog *databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbAsterisk, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();
    }

    QString languages = global::getSettingsValue("language", "settings").toString();
    QTranslator qtTranslator;
    if(languages == "Русский (default)")
    {
        qtTranslator.load("..\\translations\\qt_ru");
    }
    else if(languages == "Українська")
    {
        qtTranslator.load("..\\translations\\qt_uk");
    }
    else if(languages == "English")
    {
        qtTranslator.load("..\\translations\\qt_en");
    }
    app.installTranslator(&qtTranslator);

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
