#ifndef EDITORGCONTACTDIALOG_H
#define EDITORGCONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QValidator>
#include <QKeyEvent>
#include <QSqlDatabase>
#include <QTextCursor>

namespace Ui {
class EditOrgContactDialog;
}

class EditOrgContactDialog : public QDialog
{
    Q_OBJECT

signals:
    void sendData(bool, int, int);

public slots:
    void setPos(int, int);

public:
    explicit EditOrgContactDialog(QWidget *parent = 0);
    ~EditOrgContactDialog();

    void setValues(QString);
    void hideBackButton();

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();
    void onCursorPosChanged();

    bool isPhone(QString*);

    void keyPressEvent(QKeyEvent*);

    bool eventFilter(QObject*, QEvent*);

private:
    Ui::EditOrgContactDialog *ui;

    QSqlDatabase db;

    QList<QLineEdit*> phonesList;

    QStringList oldPhonesList;

    QValidator* phonesValidator;
    QValidator* vyborIdValidator;

    QString contactId;
    QString firstNumber;
    QString secondNumber;
    QString thirdNumber;
    QString fourthNumber;
    QString fifthNumber;
    QString number;
    QStringList numbers;

    QTextCursor textCursor;
};

#endif // EDITORGCONTACTDIALOG_H
