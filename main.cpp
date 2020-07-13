#include "OutCALL.h"
#include "Global.h"
#include "Notifier.h"
#include "Windows.h"
#include "DatabasesConnectDialog.h"

#include <QApplication>
#include <QLockFile>
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
#include <QtSql>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication app(argc, argv);

//    QLockFile lockFile(QDir::temp().absoluteFilePath("lurity.lock"));

//    if(!lockFile.tryLock(100))
//    {
//        QMessageBox msgBox;
//        msgBox.setIcon(QMessageBox::Warning);
//        msgBox.setText(QObject::tr("Приложение уже запущено!"));
//        msgBox.exec();
//        return 1;
//    }

    g_LanguagesPath = QApplication::applicationDirPath() + "/translations";
    g_AppSettingsFolderPath = QDir::homePath() + "/OutCALL";
    g_AppDirPath = QApplication::applicationDirPath();

    QString languages = global::getSettingsValue("language", "settings").toString();
    QTranslator qtTranslator;
    if (languages == "Русский (по умолчанию)")
       qtTranslator.load(":/russian.qm");
    else if (languages == "Українська")
       qtTranslator.load(":/ukrainian.qm");
    else if (languages == "English")
       qtTranslator.load(":/english.qm");
    app.installTranslator(&qtTranslator);

    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray password3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(password3));
    QString port_3 = global::getSettingsValue("port_3", "settings").toString();

    if (!hostName_3.isEmpty() && !databaseName_3.isEmpty() && !userName_3.isEmpty() && !password3.isEmpty() && !port_3.isEmpty())
    {
        QSqlDatabase dbMSSQL = QSqlDatabase::addDatabase("QODBC", "Third");
        dbMSSQL.setDatabaseName("DRIVER={SQL Server Native Client 10.0};"
                                "Server="+hostName_3+","+port_3+";"
                                "Database="+databaseName_3+";"
                                "Uid="+userName_3+";"
                                "Pwd="+password_3);
        bool ok = dbMSSQL.open();
        if (ok)
            MSSQLopened = true;
        else
             QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе Access!"), QMessageBox::Ok);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
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
    db.setPort(port_1.toUInt());
    db.open();

    QSqlDatabase dbAsterisk = QSqlDatabase::addDatabase("QMYSQL","Second");
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
    dbAsterisk.setPort(port_2.toUInt());
    dbAsterisk.open();

    if(!db.isOpen() && !dbAsterisk.isOpen())
    {
        QString state = "twoDb";
        QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базам данных!"), QMessageBox::Ok);
        DatabasesConnectDialog *databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbAsterisk, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();
    }
    else if(!db.isOpen())
    {
        QString state = "db";
        QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе контактов!"), QMessageBox::Ok);
        DatabasesConnectDialog *databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbAsterisk, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();

    }
    else if(!dbAsterisk.isOpen())
    {
        QString state = "dbAsterisk";
        QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе звонков!"), QMessageBox::Ok);
        DatabasesConnectDialog *databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbAsterisk, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();
    }

    opened = true;

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
        bCallRequest = QString(argv[1]).contains("Dial#####");

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
