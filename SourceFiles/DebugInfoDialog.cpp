/*
 * Класс служит для вывода результатов отладки.
 */

#include "DebugInfoDialog.h"
#include "ui_DebugInfoDialog.h"

#include "Global.h"

#include <QDesktopServices>

DebugInfoDialog::DebugInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugInfoDialog)
{
    ui->setupUi(this);

    ui->textEdit->setReadOnly(true);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() & Qt::WindowMinimizeButtonHint);

    connect(ui->exit,  &QAbstractButton::clicked, this, &DebugInfoDialog::onExit);
    connect(ui->clear, &QAbstractButton::clicked, this, &DebugInfoDialog::onClear);
}

DebugInfoDialog::~DebugInfoDialog()
{
    delete ui;
}

/**
 * Выполняет очистку окна отладки.
 */
void DebugInfoDialog::onClear() const
{
    ui->textEdit->clear();
}

/**
 * Выполняет закрытие окна.
 */
void DebugInfoDialog::onExit()
{
    hide();
}

/**
 * Выполняет обновление информации об отладке.
 */
void DebugInfoDialog::updateDebug(const QString &info)
{
    ui->textEdit->appendPlainText(info);
}

/**
 * Выполняет обработку нажатий клавиш.
 * Особая обработка для клавиши Enter.
 */
void DebugInfoDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
        onClear();
    else
        QDialog::keyPressEvent(event);
}
