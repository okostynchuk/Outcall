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

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    extensions = g_pAsteriskManager->extensionNumbers.values();

    extensions.removeAll(my_number);
    extensions.removeOne("");

    ui->listWidget->addItems(extensions);

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

    results.removeAll(item->text());
}

/**
 * Выполняет добавление всех сотрудников в список получателей напоминания.
 */
void ChooseEmployee::on_chooseAllButton_clicked()
{
    if (ui->listWidget->count() == 0)
        return;

    results.clear();

    ui->listWidget->clear();

    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(extensions);
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
            results.append(item->text());

        ui->listWidget->clear();
        ui->listWidget->addItems(results);
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
        ui->listWidget->addItems(extensions);
    else
    {
        for (qint32 i = 0; i < extensions.length(); ++i)
        {
            QString item = extensions.at(i);

            if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
                results.append(item);
        }

        ui->listWidget->addItems(results);
    }

    ui->listWidget->sortItems(Qt::AscendingOrder);

    ui->listWidget_2->clear();
}

/**
 * Выполняет поиск по списку сотрудников.
 */
void ChooseEmployee::onSearch()
{
    results.clear();

    for (qint32 i = 0; i < extensions.length(); ++i)
    {
        QString item = extensions.at(i);

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            results.append(item);
    }

    for (qint32 i = 0; i < ui->listWidget_2->count(); ++i)
    {
        QString item = ui->listWidget_2->item(i)->text();

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            results.removeAll(item);
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(results);
    ui->listWidget->sortItems(Qt::AscendingOrder);
}

/**
 * Получает список сотрудников из классов
 * AddReminderDialog, EditReminderDialog.
 */
void ChooseEmployee::setValues(const QStringList& employee)
{
    this->employee = employee;
    this->employee.removeAll(my_number);

    ui->listWidget_2->addItems(this->employee);
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
        employee << my_number;
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
            employee << my_number;
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
