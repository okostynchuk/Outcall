/*
 * Класс служит для добавления и просмотра заметок к звонкам.
 */

#include "NotesDialog.h"
#include "ui_NotesDialog.h"

#include "CallHistoryDialog.h"

#include <QSqlQuery>
#include <QDateTime>

NotesDialog::NotesDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::NotesDialog)
{
    ui->setupUi(this);

    QRegularExpression regExp("^[0-9]*$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->lineEdit_page->setValidator(validator);

    ui->comboBox_list->hide();

    connect(ui->textEdit,     &QTextEdit::textChanged,   this, &NotesDialog::onTextChanged);
    connect(ui->saveButton,   &QAbstractButton::clicked, this, &NotesDialog::onSave);
    connect(ui->updateButton, &QAbstractButton::clicked, this, &NotesDialog::onUpdate);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
}

NotesDialog::~NotesDialog()
{
    delete ui;
}

/**
 * Получает данные заметок по звонку или номеру из классов CallHistoryDialog,
 * PopupWindow, ViewContactDialog и ViewOrgContactDialog.
 */
void NotesDialog::setValues(const QString& uniqueid, const QString& phone)
{
    m_callId = uniqueid;

    m_phone = phone;

    m_go = "default";

    m_page = "1";

    loadNotes();
}

/**
 * Выполняет скрытие возможности добавления заметок.
 */
void NotesDialog::hideAddNote()
{
    ui->saveButton->setVisible(false);
    ui->textEdit->setVisible(false);
    ui->label->setVisible(false);

    QWidget::resize(550, 275);
}

/**
 * Выполняет вывод и обновление списка заметок.
 */
void NotesDialog::loadNotes()
{
    m_queryModel = new QSqlQueryModel(this);

    QSqlQuery queryCount(m_db);

    QString queryString = "SELECT COUNT(*) FROM calls WHERE ";

    if (m_phone.isEmpty())
        queryString.append("uniqueid = '" + m_callId + "' ");
    else
    {
        if (isInternalPhone(&m_phone))
            queryString.append("phone_number = '" + m_phone + "' AND author = '" + g_personalNumberName +"'");
        else
        {
            QSqlQuery query(m_db);

            query.prepare("SELECT entry_phone FROM entry_phone WHERE entry_id = (SELECT entry_id FROM entry_phone WHERE entry_phone = '" + m_phone + "')");
            query.exec();

            while (query.next())
                m_numbers.append(query.value(0).toString());

            if (m_numbers.isEmpty())
                queryString.append("( phone_number = '" + m_phone + "'");
            else
            {
                for (qint32 i = 0; i < m_numbers.size(); ++i)
                {
                    if (i == 0)
                        queryString.append("( phone_number = '" + m_numbers[i] + "'");
                    else
                        queryString.append(" OR phone_number = '" + m_numbers[i] + "'");
                }
            }

            queryString.append(") AND author = '" + g_personalNumberName + "'");
        }
    }

    queryCount.prepare(queryString);
    queryCount.exec();
    queryCount.first();

    qint32 count = queryCount.value(0).toInt();

    QString pages = ui->label_pages->text();

    if (count <= ui->comboBox_list->currentText().toInt())
        pages = "1";
    else
    {
        qint32 remainder = count % ui->comboBox_list->currentText().toInt();

        if (remainder)
            remainder = 1;
        else
            remainder = 0;

        pages = QString::number(count / ui->comboBox_list->currentText().toInt() + remainder);
    }

    if (m_go == "previous" && m_page != "1")
        m_page = QString::number(m_page.toInt() - 1);
    else if (m_go == "previousStart" && m_page != "1")
        m_page = "1";
    else if (m_go == "next" && m_page.toInt() < pages.toInt())
        m_page = QString::number(m_page.toInt() + 1);
    else if (m_go == "next" && m_page.toInt() >= pages.toInt())
        m_page = pages;
    else if (m_go == "nextEnd" && m_page.toInt() < pages.toInt())
        m_page = pages;
    else if (m_go == "enter" && ui->lineEdit_page->text().toInt() > 0 && ui->lineEdit_page->text().toInt() <= pages.toInt())
        m_page = ui->lineEdit_page->text();
    else if (m_go == "enter" && ui->lineEdit_page->text().toInt() > pages.toInt()) {}
    else if (m_go == "default" && m_page.toInt() >= pages.toInt())
        m_page = pages;
    else if (m_go == "default" && m_page == "1")
        m_page = "1";

    ui->lineEdit_page->setText(m_page);
    ui->label_pages->setText(tr("из ") + pages);

    queryString.replace("COUNT(*)", "datetime, author, note");

    queryString.append(" ORDER BY datetime DESC LIMIT ");

    if (ui->lineEdit_page->text() == "1")
        queryString.append("0, "
                        + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt()) + " ");
    else
        queryString.append("" + QString::number(ui->lineEdit_page->text().toInt() * ui->comboBox_list->currentText().toInt() -
                                                ui->comboBox_list->currentText().toInt()) + " , " +
                           QString::number(ui->comboBox_list->currentText().toInt()));

    m_queryModel->setQuery(queryString);

    m_queryModel->setHeaderData(0, Qt::Horizontal, tr("Дата и время"));
    m_queryModel->setHeaderData(1, Qt::Horizontal, tr("Автор"));
    m_queryModel->insertColumn(2);
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("Заметка"));

    ui->tableView->setModel(m_queryModel);

    for (qint32 row_index = 0; row_index < ui->tableView->model()->rowCount(); ++row_index)
    {
        QRegularExpressionMatchIterator hrefIterator = m_hrefRegExp.globalMatch(m_queryModel->data(m_queryModel->index(row_index, 3)).toString());

        if (hrefIterator.hasNext())
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 2), addWidgetNote(row_index, true));
        else
            ui->tableView->setIndexWidget(m_queryModel->index(row_index, 2), addWidgetNote(row_index, false));
    }

    ui->tableView->setColumnHidden(3, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}

/**
 * Выполняет проверку введенных данных и их последующее сохранение в БД.
 */
void NotesDialog::onSave()
{
    QSqlQuery query(m_db);

    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString note = ui->textEdit->toPlainText().trimmed();

    if (note.isEmpty())
    {
        MsgBoxError(tr("Содержание заметки не может быть пустым!"));

        return;
    }

    query.prepare("INSERT INTO calls (uniqueid, datetime, note, author, phone_number) VALUES(?, ?, ?, ?, ?)");
    query.addBindValue(m_callId);
    query.addBindValue(dateTime);
    query.addBindValue(note);
    query.addBindValue(g_personalNumberName);
    query.addBindValue(m_phone);
    query.exec();

    emit sendData();

    m_go = "default";

    ui->textEdit->clear();

    onUpdate();

    //MsgBoxInformation(tr("Заметка успешно добавлена!"));
}

/**
 * Выполняет удаление последнего символа в тексте,
 * если его длина превышает 255 символов.
 */
void NotesDialog::onTextChanged()
{
    qint32 maxTextLength = 255;

    if (ui->textEdit->toPlainText().length() > maxTextLength)
    {
        qint32 diff = ui->textEdit->toPlainText().length() - maxTextLength;

        QString newStr = ui->textEdit->toPlainText();
        newStr.chop(diff);

        ui->textEdit->setText(newStr);

        QTextCursor cursor(ui->textEdit->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

        ui->textEdit->setTextCursor(cursor);
    }
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Enter.
 */
void NotesDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        if (ui->textEdit->hasFocus())
            return;
        else
            onSave();
    }
    else if (event->key() == Qt::Key_Backspace)
    {
        if (ui->textEdit->hasFocus())
        {
            ui->textEdit->setReadOnly(false);
             ui->textEdit->textCursor().deletePreviousChar();
        }
    }
    else
        QDialog::keyPressEvent(event);
}

/**
 * Выполняет операции для последующего обновления списка заметок.
 */
void NotesDialog::onUpdate()
{
    deleteObjects();
    loadNotes();
}

/**
 * Выполняет добавление виджета для поля "Заметка".
 */
QWidget* NotesDialog::addWidgetNote(qint32 row_index, bool url)
{
    QWidget* widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    QLabel* noteLabel = new QLabel(widget);

    layout->addWidget(noteLabel);

    QString note = m_queryModel->data(m_queryModel->index(row_index, 3)).toString();

    if (url)
    {
        QRegularExpressionMatchIterator hrefIterator = m_hrefRegExp.globalMatch(note);
        QStringList hrefs, hrefsNoCharacters, hrefsReplaceCharacters;

        note.replace("<", "&lt;").replace(">", "&gt;");

        while (hrefIterator.hasNext())
        {
            QRegularExpressionMatch match = hrefIterator.next();
            QString href = match.captured(1);

            hrefs << href;
            href.remove(QRegularExpression("[\\,\\.\\;\\:\\'\\\"\\-\\!\\?\\^\\`\\~\\*\\№\\%\\&\\$\\#\\<\\>\\(\\)\\[\\]\\{\\}]+$"));
            hrefsNoCharacters << href;
        }

        QStringList firstCharList, lastCharList;

        for (qint32 i = 0; i < hrefs.length(); ++i)
        {
            QString hrefReplaceCharacters = QString(hrefs.at(i)).replace("<", "&lt;").replace(">", "&gt;");
            hrefsReplaceCharacters << hrefReplaceCharacters;
            hrefReplaceCharacters = hrefReplaceCharacters.remove(hrefsNoCharacters.at(i));

            if (hrefReplaceCharacters.isEmpty())
                lastCharList << " ";
            else
                lastCharList << hrefReplaceCharacters;
        }

        note.replace(QRegularExpression("\\n"), QString(" <br> "));

        qint32 index = 0;

        for (qint32 i = 0; i < hrefsReplaceCharacters.length(); ++i)
        {
            if (i == 0)
                index = note.indexOf(hrefsReplaceCharacters.at(i));
            else
                index = note.indexOf(hrefsReplaceCharacters.at(i), index + hrefsReplaceCharacters.at(i - 1).size());

            if (index > 0)
                firstCharList << note.at(index - 1);
            else
                firstCharList << "";
        }

        for (qint32 i = 0; i < hrefs.length(); ++i)
        {
            qint32 size;

            if (firstCharList.at(i) == "")
                size = hrefsReplaceCharacters.at(i).size();
            else
                size = hrefsReplaceCharacters.at(i).size() + 1;

            note.replace(note.indexOf(QRegularExpression("( |^|\\^|\\.|\\,|\\(|\\)|\\[|\\]|\\{|\\}|\\;|\\'|\\\"|[a-zA-Z0-9а-яА-Я]|\\`|\\~|\\%|\\$|\\#|\\№|\\@|\\&|\\/|\\\\|\\!|\\*)" + QRegularExpression::escape(hrefsReplaceCharacters.at(i)) + "( |$)")),
                        size, QString(firstCharList.at(i) + "<a href='" + hrefsNoCharacters.at(i) + "'>" + hrefsNoCharacters.at(i) + "</a>" + lastCharList.at(i)));
        }
    }

    noteLabel->setText(note);
    noteLabel->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::LinksAccessibleByMouse);
    noteLabel->setOpenExternalLinks(true);
    noteLabel->setWordWrap(true);

    m_widgets.append(widget);

    return widget;
}

/**
 * Выполняет удаление объектов класса.
 */
void NotesDialog::deleteObjects()
{
    if (!m_queryModel.isNull())
    {
        for (qint32 i = 0; i < m_widgets.size(); ++i)
            m_widgets[i]->deleteLater();

        m_widgets.clear();

        m_queryModel->deleteLater();
    }
}

/**
 * Выполняет проверку номера на соотвествие шаблону внутреннего номера.
 */
bool NotesDialog::isInternalPhone(QString* str)
{
    qint32 pos = 0;

    QRegularExpressionValidator validator1(QRegularExpression("^[0-9]{4}$"));
    QRegularExpressionValidator validator2(QRegularExpression("^[2][0-9]{2}$"));

    if (validator1.validate(*str, pos) == QValidator::Acceptable)
        return true;

    if (validator2.validate(*str, pos) == QValidator::Acceptable)
        return true;

    return false;
}

/**
 * Выполняет операции для последующего перехода на предыдущую страницу.
 */
void NotesDialog::on_previousButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previous";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на следующую страницу.
 */
void NotesDialog::on_nextButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "next";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на первую страницу.
 */
void NotesDialog::on_previousStartButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "previousStart";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на последнюю страницу.
 */
void NotesDialog::on_nextEndButton_clicked()
{
    ui->tableView->scrollToTop();

    m_go = "nextEnd";

    onUpdate();
}

/**
 * Выполняет операции для последующего перехода на заданную страницу.
 */
void NotesDialog::on_lineEdit_page_returnPressed()
{
    ui->tableView->scrollToTop();

    m_go = "enter";

    onUpdate();
}
