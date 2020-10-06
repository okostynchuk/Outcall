#ifndef EDITCONTACTDIALOG_H
#define EDITCONTACTDIALOG_H

#include "AddOrgToPerson.h"

#include <QDialog>
#include <QValidator>
#include <QLineEdit>
#include <QKeyEvent>
#include <QPointer>
#include <QLineEdit>
#include <QTextCursor>

class ViewContactDialog;

namespace Ui {
class EditContactDialog;
}

class EditContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool, qint32, qint32);

public slots:
    void receiveOrgName(QString, QString);
    void setPos(qint32, qint32);

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();
    void onCursorPosChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString* str);

    void keyPressEvent(QKeyEvent*);
    void closeEvent(QCloseEvent*);

    bool eventFilter(QObject*, QEvent*);

public:
    explicit EditContactDialog(QWidget *parent = 0);
    ~EditContactDialog();

    void setValues(QString);
    void hideBackButton();

private:
    Ui::EditContactDialog *ui;

    QSqlDatabase db;

    QPointer<AddOrgToPerson> addOrgToPerson;

    QList<QLineEdit*> phonesList;

    QStringList oldPhonesList;

    QValidator* phonesValidator;
    QValidator* vyborIdValidator;

    QString contactId;
    QString orgId;
    QString number;

    QTextCursor textCursor;
};

#endif // EDITCONTACTDIALOG_H
