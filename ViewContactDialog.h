#ifndef VIEWCONTACTDIALOG_H
#define VIEWCONTACTDIALOG_H

#include <QDialog>

namespace Ui {
class ViewContactDialog;
}

class ViewContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewContactDialog(QWidget *parent = 0);
    ~ViewContactDialog();

private:
    Ui::ViewContactDialog *ui;
};

#endif // VIEWCONTACTDIALOG_H
