#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include <QDialog>
#include <QValidator>
#include <QStringList>

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditContactDialog(QWidget *parent = 0);
    void setValuesContacts(QString &);
    void setValuesCallHistory(QString &);
    ~EditContactDialog();

protected slots:
    void onSave();
    void onComboBoxSelected();

protected:
    bool eventFilter(QObject*, QEvent*event);

private:
    Ui::EditContactDialog *ui;
    QValidator *validator;
    QString updateID;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QString number;
    QStringList numbers;

signals:
    void sendData(bool);
};

#endif // EDITCONTACTDIALOG_H
