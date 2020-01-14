#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

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
    void onEdit();

    void onDelete();

private:
    Ui::ContactsDialog *ui;
    QSqlQueryModel *query1;
    QWidget* createEditButton() const;
};

#endif // CONTACTSDIALOG_H
