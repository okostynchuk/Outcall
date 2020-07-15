#include "AddReminderDialog.h"
#include "ui_AddReminderDialog.h"

AddReminderDialog::AddReminderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddReminderDialog)
{
    ui->setupUi(this);
}

AddReminderDialog::~AddReminderDialog()
{
    delete ui;
}
