#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QStringList>
#include <QLineEdit>
#include <QKeyEvent>

class ViewContactDialog;

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool, int, int);

public slots:
    void receiveOrgID(QString &);

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isInnerPhone(QString *str);
    bool isPhone(QString *str);
    bool isVyborID(QString *str);

    void setPos(int, int);

    void keyPressEvent(QKeyEvent* event);

public:
    explicit EditContactDialog(QWidget *parent = 0);
    ~EditContactDialog();

    void setValuesContacts(QString &);

private:
    Ui::EditContactDialog *ui;

    AddOrgToPerson *addOrgToPerson;
    ViewContactDialog *viewContactDialog;

    QList <QLineEdit *> phonesList;

    QValidator *validator;

    QString updateID;
    QString number;
    QStringList oldNumbers;
};

#endif // EDITCONTACTDIALOG_H
