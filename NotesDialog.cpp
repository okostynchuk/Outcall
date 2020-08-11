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

    my_number = global::getExtensionNumber("extensions");

    connect(ui->textEdit, SIGNAL(objectNameChanged(QString)), this, SLOT(onSave()));
    connect(ui->saveButton, &QPushButton::clicked, this, &NotesDialog::onSave);
    connect(ui->updateButton, &QPushButton::clicked, this, &NotesDialog::onUpdate);
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    ui->tableView->setStyleSheet("QTableView { selection-color: black; selection-background-color: #18B7FF; }");
}

NotesDialog::~NotesDialog()
{
    deleteObjects();
    delete ui;
}

void NotesDialog::setCallId(QString uniqueid, QString state_call)
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
    query->insertColumn(2);
    query->setHeaderData(2, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(query);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
        ui->tableView->setIndexWidget(query->index(row_index, 2), addWidgetNote(row_index));

    ui->tableView->setColumnHidden(3, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

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
    QString note = ui->textEdit->toPlainText().simplified();

    if (note.isEmpty())
    {
        QMessageBox::critical(this, QObject::tr("Ошибка"), QObject::tr("Содержание заметки не может быть пустым!"), QMessageBox::Ok);

        return;
    }

    QRegularExpression hrefRegExp("(https?:\\/\\/(?:www\\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|www\\.[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|https?:\\/\\/(?:www\\.|(?!www))[a-zA-Z0-9]+\\.[^\\s]{2,}|www\\.[a-zA-Z0-9]+\\.[^\\s]{2,})");
    QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
    QStringList hrefs;

    if (hrefIterator.hasNext())
    {
        QRegularExpressionMatch match = hrefIterator.next();
        QString href = match.captured(1);

        hrefs << href;
    }

    for (int i = 0; i < hrefs.length(); ++i)
        note.replace(QRegExp("(https?:\\/\\/(?:www\\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|www\\.[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\\.[^\\s]{2,}|https?:\\/\\/(?:www\\.|(?!www))[a-zA-Z0-9]+\\.[^\\s]{2,}|www\\.[a-zA-Z0-9]+\\.[^\\s]{2,})"), "<a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a>");

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
    query.addBindValue(note);
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
}

void NotesDialog::onTextChanged()
{
    if (ui->textEdit->toPlainText().simplified().length() > 255)
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

QWidget* NotesDialog::addWidgetNote(int row_index)
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* noteLabel = new QLabel(wgt);

    layout->addWidget(noteLabel, 0, Qt::AlignTop);

    noteLabel->setText(query->data(query->index(row_index, 3)).toString());
    noteLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    wgt->setLayout(layout);

    widgets.append(wgt);
    layouts.append(layout);
    labels.append(noteLabel);

    return wgt;
}

void NotesDialog::deleteObjects()
{
    for (int i = 0; i < widgets.size(); ++i)
        widgets[i]->deleteLater();

    for (int i = 0; i < layouts.size(); ++i)
        layouts[i]->deleteLater();

    for (int i = 0; i < labels.size(); ++i)
        labels[i]->deleteLater();

    widgets.clear();
    layouts.clear();
    labels.clear();

    delete query;
}
