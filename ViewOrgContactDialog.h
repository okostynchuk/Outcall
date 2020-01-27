#ifndef VIEWORGCONTACTDIALOG_H
#define VIEWORGCONTACTDIALOG_H

#include <QDialog>

namespace Ui {
class ViewOrgContactDialog;
}

class ViewOrgContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewOrgContactDialog(QWidget *parent = 0);
    ~ViewOrgContactDialog();

private:
    Ui::ViewOrgContactDialog *ui;
};

#endif // VIEWORGCONTACTDIALOG_H
