#include "NotesDialog.h"
#include "ui_NotesDialog.h"

#include "CallHistoryDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QTime>
#include <QSqlQuery>
#include <QKeyEvent>
#include <QList>

NotesDialog::NotesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotesDialog)
{
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

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

void NotesDialog::receiveData(QString uniqueid, QString phone, QString loadState)
{
    callId = uniqueid;

    phoneNumber = phone;

    this->loadState = loadState;

    loadNotes();
}

void NotesDialog::hideAddNote()
{
    ui->saveButton->setVisible(false);
    ui->textEdit->setVisible(false);
    ui->label->setVisible(false);
    QWidget::resize(550, 275);
}

void NotesDialog::loadNotes()
{
    query = new QSqlQueryModel;

    QString queryString = "SELECT datetime, author, note FROM calls WHERE ";

    if (loadState == "byId")
        queryString.append("uniqueid = '" + callId + "' ");
    else
    {
        if (isInnerPhone(&phoneNumber))
            queryString.append("phone_number = '" + phoneNumber + "' AND author = '" + my_number +"'");
        else
        {
            QSqlDatabase db;
            QSqlQuery query(db);

            query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = (SELECT entry_id FROM entry_phone WHERE entry_phone = '" + phoneNumber + "')");
            query.exec();

            while (query.next())
                numbersList.append(query.value(0).toString());

            if (numbersList.isEmpty())
                queryString.append("( phone_number = '" + phoneNumber + "'");
            else
            {
                for (int i = 0; i < numbersList.size(); i++)
                {
                    if (i == 0)
                        queryString.append("( phone_number = '" + numbersList[i] + "'");
                    else
                        queryString.append(" OR phone_number = '" + numbersList[i] + "'");
                }

                queryString.append(") AND author = '" + my_number + "'");
            }
        }
    }

    queryString.append(" ORDER BY datetime DESC");

    query->setQuery(queryString);

    query->setHeaderData(0, Qt::Horizontal, QObject::tr("Дата и время"));
    query->setHeaderData(1, Qt::Horizontal, tr("Автор"));
    query->insertColumn(2);
    query->setHeaderData(2, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(query);

    for (int row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(query->data(query->index(row_index, 3)).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(query->index(row_index, 2), addWidgetNote(row_index, "URL"));
        else
            ui->tableView->setIndexWidget(query->index(row_index, 2), addWidgetNote(row_index, ""));
    }

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

    query.prepare("INSERT INTO calls (uniqueid, datetime, note, author, phone_number) VALUES(?, ?, ?, ?, ?)");
    query.addBindValue(callId);
    query.addBindValue(dateTime);
    query.addBindValue(note);
    query.addBindValue(my_number);
    query.addBindValue(phoneNumber);
    query.exec();

    emit sendData();

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

QWidget* NotesDialog::addWidgetNote(int row_index, QString url)
{
    QWidget* wgt = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* noteLabel = new QLabel(wgt);

    layout->addWidget(noteLabel);

    QString note = query->data(query->index(row_index, 3)).toString();

    if (url == "URL")
    {
        QRegularExpressionMatchIterator hrefIterator = hrefRegExp.globalMatch(note);
        QStringList hrefs;

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            if (!hrefs.contains(href))
                hrefs << href;
        }

        for (int i = 0; i < hrefs.length(); ++i)
            note.replace(QRegularExpression("(^|\\s)" + QRegularExpression::escape(hrefs.at(i)) + "(\\s|$)"), QString(" <a href='" + hrefs.at(i) + "'>" + hrefs.at(i) + "</a> "));
    }

    noteLabel->setText(note);
    noteLabel->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::LinksAccessibleByMouse);
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

bool NotesDialog::isInnerPhone(QString *str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[2][0-9]{2}"));

    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}
