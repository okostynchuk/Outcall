/*
 * Класс служит для осуществления звонков.
 */

#include "PlaceCallDialog.h"
#include "ui_PlaceCallDialog.h"

#include "Global.h"
#include "AsteriskManager.h"

#include <QHeaderView>
#include <QSqlQuery>
#include <QModelIndex>
#include <QDesktopWidget>

PlaceCallDialog::PlaceCallDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PlaceCallDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    m_geometry = saveGeometry();

    QRegularExpression regExp("^[\\+]?[0-9]*$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phoneLine->setValidator(validator);

    connect(ui->tableView, &QAbstractItemView::clicked, this, &PlaceCallDialog::showNumber);

    connect(ui->callButton,   &QAbstractButton::clicked, this, &PlaceCallDialog::onCallButton);
    connect(ui->cancelButton, &QAbstractButton::clicked, this, &PlaceCallDialog::onCancelButton);

    connect(ui->comboBox,   &QComboBox::currentTextChanged,  this, &PlaceCallDialog::clearEditText);
    connect(ui->comboBox_2, static_cast<void (QComboBox::*)(qint32)>(&QComboBox::currentIndexChanged), this, &PlaceCallDialog::onOrgChanged);

    m_queryModel = new QSqlQueryModel(this);

    ui->orgLabel->hide();
    ui->comboBox_2->hide();

    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);

    ui->number->setText(g_personalNumber);
}

PlaceCallDialog::~PlaceCallDialog()
{
    delete ui;
}

/**
 * Выполняет операции для последующего выбора номера и совершения звонка.
 */
void PlaceCallDialog::showNumber(const QModelIndex& index)
{
    QString id = m_queryModel->data(m_queryModel->index(index.row(), 0)).toString();
    QString phones = m_queryModel->data(m_queryModel->index(index.row(), 2)).toString();

    QStringList phonesList = phones.split(QRegularExpression("\n"));

    if (phonesList.count() == 1)
    {
        QString number = phonesList.at(0);
        QString protocol = global::getSettingsValue(g_personalNumber, "extensions").toString();

        g_asteriskManager->originateCall(g_personalNumber, number, protocol, g_personalNumber);

        ui->phoneLine->setText(number);
    }
    else
    {
        m_chooseNumber = new ChooseNumber;
        m_chooseNumber->setValues(id, 0);
        connect(m_chooseNumber, &ChooseNumber::sendNumber, this, &PlaceCallDialog::receiveNumber);
        m_chooseNumber->show();
        m_chooseNumber->setAttribute(Qt::WA_DeleteOnClose);
    }
}

/**
 * Выполняет запрос при смене организации в поле со списком.
 */
void PlaceCallDialog::onOrgChanged()
{
    QString queryString = "SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_type FROM entry_phone "
                          "WHERE entry_person_org_id = '" + m_orgsId.at(ui->comboBox_2->currentIndex()) + "' "
                          "GROUP BY entry_id ORDER BY entry_name ASC";

    setModel(queryString);
}

/**
 * Получает выбранный номер из класса ChooseNumber.
 */
void PlaceCallDialog::receiveNumber(QString number)
{
    ui->phoneLine->setText(number);
}

/**
 * Выполняет вывод списка контактов по фильтру.
 */
void PlaceCallDialog::onUpdate()
{
    QString queryString = "SELECT entry_id, entry_name, GROUP_CONCAT(DISTINCT entry_phone ORDER BY entry_id SEPARATOR '\n'), entry_type FROM entry_phone ";

    if (ui->comboBox->currentIndex() == 0)
        queryString.append("WHERE entry_name LIKE '%" + ui->lineEdit->text().trimmed() + "%' ");
    else if (ui->comboBox->currentIndex() == 1)
        queryString.append("WHERE entry_phone LIKE '%" + ui->lineEdit->text().trimmed() + "%' ");
    else if (ui->comboBox->currentIndex() == 2)
    {
        QSqlQuery query(m_db);

        query.prepare("SELECT entry_id, entry_name FROM entry_phone WHERE entry_type = 'org' AND entry_name LIKE '%" + ui->lineEdit->text().trimmed() + "%' GROUP BY entry_id");
        query.exec();

        if (!m_orgsId.isEmpty())
        {
           m_orgsId.clear();
           m_orgsName.clear();
        }

        while (query.next())
        {
            m_orgsId.append(query.value(0).toString());
            m_orgsName.append(query.value(1).toString());
        }

        if (!m_orgsId.isEmpty())
        {
            queryString.append("WHERE entry_person_org_id = " + m_orgsId.at(0) + " ");

            ui->orgLabel->show();
            ui->orgLabel->setText(tr("Сотрудники организации"));

            ui->comboBox_2->blockSignals(true);

            ui->comboBox_2->clear();
            ui->comboBox_2->addItems(m_orgsName);
            ui->comboBox_2->show();

            ui->comboBox_2->blockSignals(false);
        }
        else
        {
            ui->comboBox_2->hide();
            ui->orgLabel->hide();
            ui->tableView->setModel(NULL);
            return;
        }
    }

    queryString.append("GROUP BY entry_id ORDER BY entry_name ASC");

    setModel(queryString);
}

/**
 * Выполняет установку модели таблицы.
 */
void PlaceCallDialog::setModel(const QString& queryString)
{
    m_queryModel->setQuery(queryString);

    m_queryModel->setHeaderData(0, Qt::Horizontal, tr("ID"));
    m_queryModel->setHeaderData(1, Qt::Horizontal, tr("ФИО / Название"));
    m_queryModel->setHeaderData(2, Qt::Horizontal, tr("Телефон"));

    ui->tableView->setModel(NULL);
    ui->tableView->setModel(m_queryModel);

    ui->tableView->setColumnHidden(3, true);

    ui->tableView->horizontalHeader()->setDefaultSectionSize(maximumWidth());

    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();

    if (ui->tableView->model()->columnCount() != 0)
    {
        ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    }
}

/**
 * Выполняет операции для последующего поиска по списку
 * при нажатии клавиши Enter, находясь в поле поиска.
 */
void PlaceCallDialog::on_lineEdit_returnPressed()
{
    ui->phoneLine->clear();

    if (ui->lineEdit->text().isEmpty())
    {
        ui->lineEdit->clear();

        ui->orgLabel->clear();
        ui->orgLabel->hide();

        ui->comboBox_2->hide();

        ui->tableView->setModel(NULL);

        return;
    }

    onUpdate();
}

/**
 * Выполняет очистку полей ввода / вывода.
 */
void PlaceCallDialog::clearEditText()
{
    ui->phoneLine->clear();

    ui->orgLabel->hide();

    ui->comboBox_2->hide();

    ui->tableView->setModel(NULL);
}

/**
 * Выполняет обработку появления окна.
 */
void PlaceCallDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    ui->lineEdit->setFocus();
}

/**
 * Выполняет обработку закрытия окна.
 */
void PlaceCallDialog::closeEvent(QCloseEvent*)
{
    hide();

    ui->comboBox->setCurrentIndex(0);

    ui->lineEdit->clear();

    ui->orgLabel->hide();

    ui->phoneLine->clear();

    ui->tableView->setModel(NULL);

    restoreGeometry(m_geometry);

    QDesktopWidget desktopWidget;
    QRect screen = desktopWidget.screenGeometry(this);

    move(screen.center() - rect().center());
}

/**
 * Выполняет совершение звонка при нажатии кнопки "Позвонить".
 */
void PlaceCallDialog::onCallButton()
{
    if (!ui->phoneLine->text().isEmpty())
    {
        QString to = ui->phoneLine->text();
        QString from = g_personalNumber;
        QString protocol = global::getSettingsValue(from, "extensions").toString();

        g_asteriskManager->originateCall(from, to, protocol, from);
    }
}

/**
 * Выполняет закрытие окна при нажатии кнопки "Отменить".
 */
void PlaceCallDialog::onCancelButton()
{
    QDialog::close();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Esc.
 */
void PlaceCallDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}

/**
 * Выполняет совершение звонка при нажатии клавиши Enter,
 * находясь в поле ввода номера.
 */
void PlaceCallDialog::on_phoneLine_returnPressed()
{
    onCallButton();
}
