#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include <QDialog>

namespace Ui {
class ContactsDialog;
}

class ContactsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactsDialog(QWidget *parent = 0);
    ~ContactsDialog();

protected slots:
    void onAdd();
    void onDelete();

private:
    Ui::ContactsDialog *ui;
};

#endif // CONTACTSDIALOG_H
