#include "ViewOrgContactDialog.h"
#include "ui_ViewOrgContactDialog.h"

ViewOrgContactDialog::ViewOrgContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewOrgContactDialog)
{
    ui->setupUi(this);
}

ViewOrgContactDialog::~ViewOrgContactDialog()
{
    delete ui;
}
