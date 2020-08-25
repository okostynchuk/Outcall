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

    connect(ui->exit,  &QPushButton::clicked, this, &DebugInfoDialog::onExit);
    connect(ui->clear, &QPushButton::clicked, this, &DebugInfoDialog::onClear);
}

DebugInfoDialog::~DebugInfoDialog()
{
    delete ui;
}

void DebugInfoDialog::onClear() const
{
    ui->textEdit->clear();
}

void DebugInfoDialog::onExit()
{
    hide();
}

void DebugInfoDialog::updateDebug(const QString &info)
{
    ui->textEdit->appendPlainText(info);
}

void DebugInfoDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return)
        onClear();
    else
        QDialog::keyPressEvent(event);
}
