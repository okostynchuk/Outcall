#include "NotesDialog.h"
#include "ui_NotesDialog.h"
#include "CallHistoryDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QTime>
#include <QSqlQuery>
#include <QKeyEvent>

NotesDialog::NotesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotesDialog)
{
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    settingsDialog = new SettingsDialog();
    my_number = settingsDialog->getExtension();

    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->saveButton, &QAbstractButton::clicked, this, &NotesDialog::onSave);
    connect(ui->updateButton, &QAbstractButton::clicked, this, &NotesDialog::onUpdate);
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
}

NotesDialog::~NotesDialog()
{
    delete ui;
    delete settingsDialog;
    deleteObjects();
}

void NotesDialog::setCallId(QString &uniqueid, QString &state_call)
{
    callId = uniqueid;
    state = state_call;
    loadNotes();
}

void NotesDialog::loadNotes()
{
    query = new QSqlQueryModel;

    query->setQuery("SELECT datetime, author, note FROM calls WHERE uniqueid = '" + callId + "' ORDER BY datetime DESC");

    query->setHeaderData(0, Qt::Horizontal, QObject::tr("Дата и время"));
    query->setHeaderData(1, Qt::Horizontal, tr("Автор"));
    query->setHeaderData(2, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(query);

    ui->tableView->setWordWrap(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");

    if (state == "save_disable")
    {
       ui->label->setDisabled(true);
       ui->textEdit->setDisabled(true);
       ui->saveButton->setDisabled(true);
    }
}

void NotesDialog::onSave()
{
    QSqlDatabase db;
    QSqlQuery query(db);
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if (ui->textEdit->toPlainText().simplified().isEmpty())
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Содержание заметки не может быть пустым!"), QMessageBox::Ok);
        return;
    }

    QString author;

    query.prepare("SELECT entry_name FROM entry_phone WHERE entry_phone = " + my_number);
    query.exec();
    if (query.next())
        author = query.value(0).toString();
    else
        author = my_number;

    query.prepare("INSERT INTO calls (uniqueid, datetime, note, author) VALUES(?, ?, ?, ?)");
    query.addBindValue(callId);
    query.addBindValue(dateTime);
    query.addBindValue(ui->textEdit->toPlainText().simplified());
    query.addBindValue(author);
    query.exec();

    if (state == "all")
        emit sendDataToAllCalls();
    else if (state == "missed")
       emit sendDataToMissed();
    else if (state == "received")
       emit sendDataToReceived();
    else if (state == "placed")
        emit sendDataToPlaced();

    close();
    QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Заметка успешно добавлена!"), QMessageBox::Ok);
    destroy(true);
}

void NotesDialog::onTextChanged()
{
    if(ui->textEdit->toPlainText().simplified().length() > 255)
        ui->textEdit->textCursor().deletePreviousChar();
}

void NotesDialog::onUpdate()
{
    deleteObjects();
    loadNotes();
}

bool NotesDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object->objectName() == "textEdit")
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return)
            {
                object->setObjectName("textEdit2");
                return true;
            }
        }
    }
    else if (object->objectName() == "textEdit2")
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return)
            {
                object->setObjectName("textEdit");
                return true;
            }
        }
    }
    return false;
}

void NotesDialog::deleteObjects()
{
    delete query;
}
