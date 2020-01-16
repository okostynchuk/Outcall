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
    void onTableClicked(const QModelIndex &);
    void onDelete();
    void on_lineEdit_returnPressed();

private:
    Ui::ContactsDialog *ui;
    QSqlQueryModel *query1;
    QSqlQueryModel *query2;
    QWidget* addImageLabel(int &i) const;
    QWidget* createEditButton() const;
};

#endif // CONTACTSDIALOG_H
