#include "OutCALL.h"
#include "Global.h"
#include "Windows.h"
#include "DatabasesConnectDialog.h"

#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QTranslator>
#include <QMessageBox>
#include <QDebug>
#include <QSqlDatabase>
#include <QRegularExpressionValidator>
#include <QSettings>

static const QString PARTIAL_DOWN (".part");

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QString appVersion = g_CurrentAppVersion;
    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(appVersion);
    app.setOrganizationName(ORGANIZATION_NAME);

    g_AppSettingsFolderPath = QDir::homePath() + "/" + QString(APP_NAME);
    g_AppDirPath = QApplication::applicationDirPath();

    QDir dir("C:\\" + QString(APP_NAME));
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    int fileAmount = dir.count();

    QStringList namesOfDirectories;
    namesOfDirectories = dir.entryList();

    QDir oldAppDir("C:\\" + QString(APP_NAME) + "\\");

    QRegularExpressionValidator folderValidator(QRegularExpression("\\.part[A-Za-z0-9-_\\.\\+]*"));
    QRegularExpressionValidator fileValidator(QRegularExpression("\\.part[A-Za-z0-9-_\\.\\+]*\\.[A-Za-z0-9]*"));
    int pos = 0;

    if (oldAppDir.exists())
    {
        for (int i = 0; i < fileAmount; ++i)
        {
            QString str = namesOfDirectories.at(i);

            if (fileValidator.validate(str, pos) == QValidator::Acceptable)
                oldAppDir.remove(namesOfDirectories.at(i));

            if (folderValidator.validate(str, pos) == QValidator::Acceptable)
            {
                QDir folder ("C:\\" + QString(APP_NAME) + "\\" + namesOfDirectories.at(i));
                folder.removeRecursively();
            }
        }
    }

    QSettings uninstallFolder("Microsoft\\Windows\\CurrentVersion", "Uninstall");
    QStringList childFolders = uninstallFolder.childGroups();

    for (int i = 0; i < childFolders.length(); ++i)
    {
        QSettings uninstallFolder("Microsoft\\Windows\\CurrentVersion\\Uninstall" , childFolders.at(i));

        if (uninstallFolder.contains("DisplayName"))
            if (uninstallFolder.value("DisplayName").toString() == QString(APP_NAME) && uninstallFolder.value("DisplayVersion").toString() != appVersion)
            {
                QProcess* process = new QProcess;

                process->start("cmd.exe /C start REG DELETE HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + childFolders.at(i) + " /f");

                process->deleteLater();
            }
    }

    QString languages = global::getSettingsValue("language", "settings").toString();
    QTranslator qtMain;
    QTranslator qtBase;

    if (languages == "Русский (по умолчанию)")
    {
        qtMain.load(":/translations/russian.qm");
        app.installTranslator(&qtMain);

        qtBase.load(":/translations/qtbase_ru.qm");
        app.installTranslator(&qtBase);
    }
    else if (languages == "Українська")
    {
        qtMain.load(":/translations/ukrainian.qm");
        app.installTranslator(&qtMain);

        qtBase.load(":/translations/qtbase_uk.qm");
        app.installTranslator(&qtBase);
    }
    else if (languages == "English")
    {
        qtMain.load(":/translations/english.qm");
        app.installTranslator(&qtMain);
    }

    QProcess tasklist;
    tasklist.start(
          "tasklist",
          QStringList() << "/FO" << "CSV"
                  << "/FI" << QString("IMAGENAME eq " + QString(APP_NAME) + ".exe"));
    tasklist.waitForFinished();
    QString output = tasklist.readAllStandardOutput();

    if (output.count(QString(APP_NAME) + ".exe") != 1)
        if (QCoreApplication::arguments().last() != "restart")
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(QObject::tr("Приложение уже запущено!"));
            msgBox.exec();

            return 1;
        }

    if (global::getSettingsValue("show_call_popup", "general").toString().isEmpty())
        global::setSettingsValue("show_call_popup", true, "general");

    global::setSettingsValue("auto_sign_in",  true, "general");
    global::setSettingsValue("auto_startup",  true, "general");

    QSettings settings("Microsoft\\Windows\\CurrentVersion", "Explorer");
    settings.beginGroup("Shell Folders");

    QFile::link(QApplication::applicationFilePath(), settings.value("Startup").toString().replace("/", "\\") + "/" + QString(APP_NAME) + ".lnk");

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

    QSqlDatabase dbCalls = QSqlDatabase::addDatabase("QMYSQL", "Calls");
    QString hostName_2 = global::getSettingsValue("hostName_2", "settings").toString();
    QString databaseName_2 = global::getSettingsValue("databaseName_2", "settings").toString();
    QString userName_2 = global::getSettingsValue("userName_2", "settings").toString();
    QByteArray password2 = global::getSettingsValue("password_2", "settings").toByteArray();
    QString password_2 = QString(QByteArray::fromBase64(password2));
    QString port_2 = global::getSettingsValue("port_2", "settings").toString();

    dbCalls.setHostName(hostName_2);
    dbCalls.setDatabaseName(databaseName_2);
    dbCalls.setUserName(userName_2);
    dbCalls.setPassword(password_2);
    dbCalls.setPort(port_2.toUInt());
    dbCalls.open();

    if (!db.isOpen() && !dbCalls.isOpen())
    {
        QString state = "twoDb";

        QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базам данных!"), QMessageBox::Ok);

        DatabasesConnectDialog* databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbCalls, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();
    }
    else if (!db.isOpen())
    {
        QString state = "db";

        QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе контактов!"), QMessageBox::Ok);

        DatabasesConnectDialog* databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbCalls, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();

    }
    else if (!dbCalls.isOpen())
    {
        QString state = "dbCalls";

        QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе звонков!"), QMessageBox::Ok);

        DatabasesConnectDialog* databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbCalls, state);
        databasesConnectDialog->exec();
        databasesConnectDialog->deleteLater();
    }

    opened = true;

    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray password3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(password3));
    QString port_3 = global::getSettingsValue("port_3", "settings").toString();

    if (!hostName_3.isEmpty() && !databaseName_3.isEmpty() && !userName_3.isEmpty() && !password_3.isEmpty() && !port_3.isEmpty())
    {
        QSqlDatabase dbOrders = QSqlDatabase::addDatabase("QODBC", "Orders");
        dbOrders.setDatabaseName("DRIVER={SQL Server Native Client 10.0};"
                                "Server="+hostName_3+","+port_3+";"
                                "Database="+databaseName_3+";"
                                "Uid="+userName_3+";"
                                "Pwd="+password_3);
        dbOrders.open();

        if (dbOrders.isOpen())
            MSSQLopened = true;
        else
            QMessageBox::critical(nullptr, QObject::tr("Ошибка"), QObject::tr("Отсутствует подключение к базе заказов!"), QMessageBox::Ok);
    }

    QString username  = global::getSettingsValue("username", "settings").toString();
    QByteArray secret = global::getSettingsValue("password", "settings").toByteArray();
    AsteriskManager manager(username, QString(QByteArray::fromBase64(secret)));

    app.setStyleSheet("QMenu                         {margin: 0px; padding: 0px; background-color: #F2F2F2;}"
                      "QMenu::item                   {background-color: #F2F2F2;}"
                      "QMenu::separator              {background-color: #A9A9A9; height: 1px;}"
                      "QMenu::item:selected          {background-color: #18B7FF; color: #000000;}"
                      "QMenu::item:disabled          {background-color: #F2F2F2; color: #A9A9A9;}"
                      "QMenu::item:disabled:selected {background-color: #F2F2F2; color: #A9A9A9;}");

    OutCall outcall;
    outcall.show();

    return app.exec();
}
