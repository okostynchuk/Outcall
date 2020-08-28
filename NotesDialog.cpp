#include "NotesDialog.h"
#include "ui_NotesDialog.h"

#include "CallHistoryDialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QTime>
#include <QSqlQuery>
#include <QList>

NotesDialog::NotesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotesDialog)
{
    ui->setupUi(this);

    QRegularExpression regExp("^[0-9]*$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->comboBox_list->hide();

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    connect(ui->saveButton, &QPushButton::clicked, this, &NotesDialog::onSave);
    connect(ui->updateButton, &QPushButton::clicked, this, &NotesDialog::onUpdate);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &NotesDialog::onTextChanged);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
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

    go = "default";
    page = "1";

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
    QSqlDatabase db;
    QSqlQuery queryCount(db);

    QString queryString = "SELECT COUNT(*) FROM calls WHERE ";

    if (loadState == "byId")
        queryString.append("uniqueid = '" + callId + "' ");
    else
    {
        if (isInternalPhone(&phoneNumber))
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
            }

            queryString.append(") AND author = '" + my_number + "'");
        }
    }

    queryCount.prepare(queryString);
    queryCount.exec();
    queryCount.first();

    count = queryCount.value(0).toInt();

    query = new QSqlQueryModel;

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        remainder = count % ui->comboBox_list->currentText().toInt();

        if (remainder)
            remainder = 1;
        else
            remainder = 0;

        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }

    if (go == "previous" && page != "1")
        page = QString::number(page.toInt() - 1);
    else if (go == "previousStart" && page != "1")
        page = "1";
    else if (go == "next" && page.toInt() < pages.toInt())
        page = QString::number(page.toInt() + 1);
    else if (go == "next" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "nextEnd" && page.toInt() < pages.toInt())
        page = pages;
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        page = ui->lineEdit_page->text();
    else if (go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (go == "default" && page.toInt() >= pages.toInt())
        page = pages;
    else if (go == "default" && page == "1")
        page = "1";

    ui->lineEdit_page->setText(page);
    ui->label_pages->setText(tr("из ") + pages);

    queryString = "SELECT datetime, author, note FROM calls WHERE ";

    if (loadState == "byId")
        queryString.append("uniqueid = '" + callId + "' ");
    else
    {
        if (isInternalPhone(&phoneNumber))
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
            }

            queryString.append(") AND author = '" + my_number + "'");
        }
    }

    queryString.append(" ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
    {
        queryString.append("0, "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    }
    else
    {
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() - ui->comboBox_list->currentText().toInt()) + " , " + QString::number(ui->comboBox_list->currentText().toInt()));
    }


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
    QString note = ui->textEdit->toPlainText().trimmed();

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

    go = "default";

    onUpdate();

    //QMessageBox::information(this, QObject::tr("Уведомление"), QObject::tr("Заметка успешно добавлена!"), QMessageBox::Ok);
}

void NotesDialog::onTextChanged()
{
    if (ui->textEdit->toPlainText().trimmed().length() > 255)
        ui->textEdit->textCursor().deletePreviousChar();
}

void NotesDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        if (ui->textEdit->hasFocus())
            return;
        else
            onSave();
    }
    else
        QDialog::keyPressEvent(event);
}

void NotesDialog::onUpdate()
{
    deleteObjects();
    loadNotes();
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

bool NotesDialog::isInternalPhone(QString* str)
{
    int pos = 0;

    QRegularExpressionValidator validator(QRegularExpression("[2][0-9]{2}"));

    if(validator.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

void NotesDialog::on_previousButton_clicked()
{
    go = "previous";

    onUpdate();
}

void NotesDialog::on_nextButton_clicked()
{
    go = "next";

    onUpdate();
}

void NotesDialog::on_previousStartButton_clicked()
{
    go = "previousStart";

    onUpdate();
}

void NotesDialog::on_nextEndButton_clicked()
{
    go = "nextEnd";

    onUpdate();;
}

void NotesDialog::on_lineEdit_page_returnPressed()
{
    go = "enter";

    onUpdate();
}
