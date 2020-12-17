#include "Outcall.h"
#include "Global.h"
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

static const QString PARTIAL_DOWN(".part");

/**
 * Выполняет запуск программы, подключение к базам данных, загрузку языковых файлов,
 * удаление старой версии программы после обновления.
 */
qint32 main(qint32 argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName(ORGANIZATION_NAME);

    g_appSettingsFolderPath = QDir::homePath() + "/" + app.applicationName();
    g_appDirPath = QApplication::applicationDirPath();
    global::setSettingsValue("InstallDir", g_appDirPath.replace("/", "\\"));

    QDir dir(g_appDirPath);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    qint32 fileAmount = dir.count();

    QStringList namesOfDirectories;
    namesOfDirectories = dir.entryList();

    QDir oldAppDir(g_appDirPath + "\\");

    QRegularExpressionValidator folderValidator(QRegularExpression("\\.part[A-Za-z0-9-_\\.\\+]*"));
    QRegularExpressionValidator fileValidator(QRegularExpression("\\.part[A-Za-z0-9-_\\.\\+]*\\.[A-Za-z0-9]*"));
    qint32 pos = 0;

    if (oldAppDir.exists())
    {
        for (qint32 i = 0; i < fileAmount; ++i)
        {
            QString str = namesOfDirectories.at(i);

            if (fileValidator.validate(str, pos) == QValidator::Acceptable)
                oldAppDir.remove(namesOfDirectories.at(i));

            if (folderValidator.validate(str, pos) == QValidator::Acceptable)
            {
                QDir folder(g_appDirPath + "\\" + namesOfDirectories.at(i));
                folder.removeRecursively();
            }
        }
    }

    QSettings uninstallFolder("Microsoft\\Windows\\CurrentVersion", "Uninstall");
    QStringList childFolders = uninstallFolder.childGroups();

    for (qint32 i = 0; i < childFolders.length(); ++i)
    {
        QSettings uninstallFolder("Microsoft\\Windows\\CurrentVersion\\Uninstall" , childFolders.at(i));

        if (uninstallFolder.contains("DisplayName"))
            if (uninstallFolder.value("DisplayName").toString() == app.applicationName() && uninstallFolder.value("DisplayVersion").toString() != app.applicationVersion())
            {
                QProcess* process = new QProcess;

                process->start("cmd.exe /C start REG DELETE HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + childFolders.at(i) + " /f");

                process->deleteLater();
            }
    }

    QString language = global::getSettingsValue("language", "settings").toString();
    QTranslator translator;
    QTranslator contextTranslator;

    if (language == "Русский")
    {
        translator.load(":/translations/russian.qm");
        contextTranslator.load(":/translations/russianContext.qm");
        app.installTranslator(&translator);
        app.installTranslator(&contextTranslator);
    }
    else if (language == "Українська")
    {        
        translator.load(":/translations/ukrainian.qm");
        contextTranslator.load(":/translations/ukrainianContext.qm");
        app.installTranslator(&translator);
        app.installTranslator(&contextTranslator);
    }
    else if (language == "English")
    {
        translator.load(":/translations/english.qm");
        app.installTranslator(&translator);
    }

    QProcess tasklist;
    tasklist.start(
          "tasklist",
          QStringList() << "/FO" << "CSV"
                  << "/FI" << QString("IMAGENAME eq " + app.applicationName() + ".exe"));
    tasklist.waitForFinished();
    QString output = tasklist.readAllStandardOutput();

    if (output.count(app.applicationName() + ".exe") != 1)
        if (QCoreApplication::arguments().QList::last() != "restart")
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(QObject::tr("Приложение уже запущено!"));
            msgBox.exec();
            msgBox.deleteLater();
            return 1;
        }

    if (global::getSettingsValue("show_call_popup", "general").toString().isEmpty())
        global::setSettingsValue("show_call_popup", true, "general");

    global::setSettingsValue("auto_sign_in",  true, "general");
    global::setSettingsValue("auto_startup",  true, "general");

    QSettings settings("Microsoft\\Windows\\CurrentVersion", "Explorer");
    settings.beginGroup("Shell Folders");

    //QFile::link(QApplication::applicationFilePath(), settings.value("Startup").toString().replace("/", "\\") + "/" + app.applicationName() + ".lnk");

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QByteArray byte_password = global::getSettingsValue("password_1", "settings").toByteArray();
    QString password = QString(QByteArray::fromBase64(byte_password));

    db.setHostName(global::getSettingsValue("hostName_1", "settings").toString());
    db.setDatabaseName(global::getSettingsValue("databaseName_1", "settings").toString());
    db.setUserName(global::getSettingsValue("userName_1", "settings").toString());
    db.setPassword(password);
    db.setPort(global::getSettingsValue("port_1", "settings").toUInt());
    db.open();

    QSqlDatabase dbCalls = QSqlDatabase::addDatabase("QMYSQL", "Calls");
    QByteArray byte_password_2 = global::getSettingsValue("password_2", "settings").toByteArray();
    QString password_2 = QString(QByteArray::fromBase64(byte_password_2));

    dbCalls.setHostName(global::getSettingsValue("hostName_2", "settings").toString());
    dbCalls.setDatabaseName(global::getSettingsValue("databaseName_2", "settings").toString());
    dbCalls.setUserName(global::getSettingsValue("userName_2", "settings").toString());
    dbCalls.setPassword(password_2);
    dbCalls.setPort(global::getSettingsValue("port_2", "settings").toUInt());
    dbCalls.open();

    if (!db.isOpen() && !dbCalls.isOpen())
    {
        MsgBoxError(QObject::tr("Отсутствует подключение к базам данных!"));

        DatabasesConnectDialog* databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbCalls, "twoDbs");
        databasesConnectDialog->exec();
        databasesConnectDialog->setAttribute(Qt::WA_DeleteOnClose);
    }
    else if (!db.isOpen())
    {
        MsgBoxError(QObject::tr("Отсутствует подключение к базе контактов!"));

        DatabasesConnectDialog* databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbCalls, "db");
        databasesConnectDialog->exec();
        databasesConnectDialog->setAttribute(Qt::WA_DeleteOnClose);

    }
    else if (!dbCalls.isOpen())
    {
        MsgBoxError(QObject::tr("Отсутствует подключение к базе звонков!"));

        DatabasesConnectDialog* databasesConnectDialog = new DatabasesConnectDialog;
        databasesConnectDialog->setDatabases(db, dbCalls, "dbCalls");
        databasesConnectDialog->exec();
        databasesConnectDialog->setAttribute(Qt::WA_DeleteOnClose);
    }

    if (db.isOpen() && dbCalls.isOpen())
        g_dbsOpened = true;
    else
        return 0;

    QString hostName_3 = global::getSettingsValue("hostName_3", "settings").toString();
    QString databaseName_3 = global::getSettingsValue("databaseName_3", "settings").toString();
    QString userName_3 = global::getSettingsValue("userName_3", "settings").toString();
    QByteArray byte_password_3 = global::getSettingsValue("password_3", "settings").toByteArray();
    QString password_3 = QString(QByteArray::fromBase64(byte_password_3));
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
            g_ordersDbOpened = true;
        else
            MsgBoxError(QObject::tr("Отсутствует подключение к базе заказов!"));
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

    Outcall outcall;
    outcall.show();

    return app.exec();
}
