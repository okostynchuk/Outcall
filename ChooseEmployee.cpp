#include "ChooseEmployee.h"
#include "ui_ChooseEmployee.h"

ChooseEmployee::ChooseEmployee(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseEmployee)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    my_number = global::getSettingsValue(global::getExtensionNumber("extensions"), "extensions_name").toString();

    extensions = g_pAsteriskManager->extensionNumbers.values();

    ui->listWidget->addItems(extensions);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &ChooseEmployee::onSearch);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &ChooseEmployee::onChoose);
    connect(ui->listWidget_2, &QListWidget::itemClicked, this, &ChooseEmployee::onRemove);
}

ChooseEmployee::~ChooseEmployee()
{
    delete ui;
}

void ChooseEmployee::onChoose(QListWidgetItem *item)
{
    ui->listWidget_2->addItem(ui->listWidget->takeItem(ui->listWidget->row(item)));
    ui->listWidget_2->sortItems(Qt::AscendingOrder);

    results.removeAll(item->text());
}

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

void ChooseEmployee::onRemove(QListWidgetItem *item)
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

void ChooseEmployee::on_removeAllButton_clicked()
{
    if (ui->listWidget_2->count() == 0)
        return;

    ui->listWidget->clear();

    if (ui->lineEdit->text().isEmpty())
        ui->listWidget->addItems(extensions);
    else
    {
        for (int i = 0; i < extensions.length(); ++i)
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

void ChooseEmployee::onSearch()
{
    results.clear();

    for (int i = 0; i < extensions.length(); ++i)
    {
        QString item = extensions.at(i);

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            results.append(item);
    }

    for (int i = 0; i < ui->listWidget_2->count(); ++i)
    {
        QString item = ui->listWidget_2->item(i)->text();

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            results.removeAll(item);
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(results);
    ui->listWidget->sortItems(Qt::AscendingOrder);
}

void ChooseEmployee::setValuesReminders(QStringList employee)
{
    this->employee = employee;

    ui->listWidget_2->addItems(this->employee);
    ui->listWidget_2->sortItems(Qt::AscendingOrder);

    for (int i = 0; i < ui->listWidget_2->count(); ++i)
        qDeleteAll(ui->listWidget->findItems(ui->listWidget_2->item(i)->text(), Qt::MatchFixedString));
}

void ChooseEmployee::on_applyButton_clicked()
{
    QStringList employee;

    if (ui->listWidget_2->count() == 0)
        employee << my_number;
    else
        for (int i = 0; i < ui->listWidget_2->count(); ++i)
            employee << ui->listWidget_2->item(i)->text();

    emit sendEmployee(employee);

    close();
}

void ChooseEmployee::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        QStringList employee;

        if (ui->listWidget_2->count() == 0)
            employee << my_number;
        else
            for (int i = 0; i < ui->listWidget_2->count(); ++i)
                employee << ui->listWidget_2->item(i)->text();

        emit sendEmployee(employee);

        close();
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (ui->listWidget_2->hasFocus())
            ui->listWidget_2->itemClicked(ui->listWidget_2->currentItem());
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (ui->listWidget->hasFocus())
            ui->listWidget->itemClicked(ui->listWidget->currentItem());
    }
    else if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}
