#include "ChooseEmployee.h"
#include "ui_ChooseEmployee.h"

ChooseEmployee::ChooseEmployee(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseEmployee)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    //setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    extensions = g_pAsteriskManager->extensionNumbers.values();

    ui->listWidget->addItems(extensions);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &ChooseEmployee::onSearch);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &ChooseEmployee::onChoose);
}

ChooseEmployee::~ChooseEmployee()
{
    delete ui;
}

void ChooseEmployee::onChoose(QListWidgetItem *item)
{
    emit sendEmployee(item->text());

    close();
}

void ChooseEmployee::onSearch()
{
    if (ui->lineEdit->text().isEmpty())
    {
        ui->listWidget->clear();
        ui->listWidget->addItems(extensions);

        return;
    }

    QStringList results;

    for (int i = 0; i < extensions.length(); ++i)
    {
        QString item = extensions.at(i);

        if (item.contains(ui->lineEdit->text(), Qt::CaseInsensitive))
            results.append(item);
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(results);
}

void ChooseEmployee::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        if (ui->listWidget->hasFocus())
            ui->listWidget->doubleClicked(ui->listWidget->currentIndex());
    }
    else if (event->key() == Qt::Key_Escape)
        QDialog::close();
    else
        QWidget::keyPressEvent(event);
}
