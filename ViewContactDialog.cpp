#include "ViewContactDialog.h"
#include "ui_ViewContactDialog.h"

ViewContactDialog::ViewContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewContactDialog)
{
    ui->setupUi(this);
}

ViewContactDialog::~ViewContactDialog()
{
    delete ui;
}
