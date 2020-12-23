/*
 * Класс служит для выбора сотрудников, которые получат напоминание.
 */

#include "ChooseEmployee.h"
#include "ui_ChooseEmployee.h"

ChooseEmployee::ChooseEmployee(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ChooseEmployee)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_extensions = g_asteriskManager->m_extensionNumbers.values();

    m_extensions.removeAll(g_personalNumberName);
    m_extensions.removeOne("");

    ui->listWidget->addItems(m_extensions);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &ChooseEmployee::onSearch);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &ChooseEmployee::onChoose);
    connect(ui->listWidget_2, &QListWidget::itemClicked, this, &ChooseEmployee::onRemove);
}

ChooseEmployee::~ChooseEmployee()
{
    delete ui;
}

/**
 * Задаёт позицию окна в зависимости от позиции окна-родителя.
 */
void ChooseEmployee::setPos(qint32 x, qint32 y)
{
    this->move(x, y);
}

/**
 * Выполняет добавление выбранного сотрудника в список получателей напоминания.
 */
void ChooseEmployee::onChoose(QListWidgetItem* item)
{
    ui->listWidget_2->addItem(ui->listWidget->takeItem(ui->listWidget->row(item)));
    ui->listWidget_2->sortItems(Qt::AscendingOrder);

    m_results.removeAll(item->text());
}

/**
 * Выполняет добавление всех сотрудников в список получателей напоминания.
 */
void ChooseEmployee::on_chooseAllButton_clicked()
{
    if (ui->listWidget->count() == 0)
        return;

    m_results.clear();

    ui->listWidget->clear();

    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(m_extensions);
    ui->listWidget_2->sortItems(Qt::AscendingOrder);
}

/**
 * Выполняет удаление выбранного сотрудника из списка получателей напоминания.
 */
void ChooseEmployee::onRemove(QListWidgetItem* item)
{
    if (ui->lineEdit->text().isEmpty())
        ui->listWidget->addItem(ui->listWidget_2->takeItem(ui->listWidget_2->row(item)));
    else
    {
        ui->listWidget_2->takeItem(ui->listWidget_2->row(item));

        if (item->text().contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            m_results.append(item->text());

        ui->listWidget->clear();
        ui->listWidget->addItems(m_results);
    }

    ui->listWidget->sortItems(Qt::AscendingOrder);
}

/**
 * Выполняет удаление всех сотрудников из списка получателей напоминания.
 */
void ChooseEmployee::on_removeAllButton_clicked()
{
    if (ui->listWidget_2->count() == 0)
        return;

    ui->listWidget->clear();

    if (ui->lineEdit->text().isEmpty())
        ui->listWidget->addItems(m_extensions);
    else
    {
        for (qint32 i = 0; i < m_extensions.length(); ++i)
        {
            QString item = m_extensions.at(i);

            if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
                m_results.append(item);
        }

        ui->listWidget->addItems(m_results);
    }

    ui->listWidget->sortItems(Qt::AscendingOrder);

    ui->listWidget_2->clear();
}

/**
 * Выполняет поиск по списку сотрудников.
 */
void ChooseEmployee::onSearch()
{
    m_results.clear();

    for (qint32 i = 0; i < m_extensions.length(); ++i)
    {
        QString item = m_extensions.at(i);

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            m_results.append(item);
    }

    for (qint32 i = 0; i < ui->listWidget_2->count(); ++i)
    {
        QString item = ui->listWidget_2->item(i)->text();

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            m_results.removeAll(item);
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(m_results);
    ui->listWidget->sortItems(Qt::AscendingOrder);
}

/**
 * Получает список сотрудников из классов
 * AddReminderDialog, EditReminderDialog.
 */
void ChooseEmployee::setValues(QStringList& employee)
{
    employee.removeAll(g_personalNumberName);

    ui->listWidget_2->addItems(employee);
    ui->listWidget_2->sortItems(Qt::AscendingOrder);

    for (qint32 i = 0; i < ui->listWidget_2->count(); ++i)
        qDeleteAll(ui->listWidget->findItems(ui->listWidget_2->item(i)->text(), Qt::MatchFixedString));
}

/**
 * Выполняет сохранение списка получателей напоминания
 * при нажатии кнопки "Применить".
 */
void ChooseEmployee::on_applyButton_clicked()
{
    QStringList employee;

    if (ui->listWidget_2->count() == 0)
        employee << g_personalNumberName;
    else
        for (qint32 i = 0; i < ui->listWidget_2->count(); ++i)
            employee << ui->listWidget_2->item(i)->text();

    emit sendEmployee(employee);

    close();
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиш Enter, стрелка влево,
 * стрелка вправо и Esc.
 */
void ChooseEmployee::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
    {
        QStringList employee;

        if (ui->listWidget_2->count() == 0)
            employee << g_personalNumberName;
        else
            for (qint32 i = 0; i < ui->listWidget_2->count(); ++i)
                employee << ui->listWidget_2->item(i)->text();

        emit sendEmployee(employee);

        close();
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (ui->listWidget_2->hasFocus())
            ui->listWidget_2->QListWidget::itemClicked(ui->listWidget_2->currentItem());
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (ui->listWidget->hasFocus())
            ui->listWidget->QListWidget::itemClicked(ui->listWidget->currentItem());
    }
    else if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}
