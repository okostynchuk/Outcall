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
    void sendData(bool update, qint32 x, qint32 y);

public slots:
    void receiveOrgName(const QString& id, const QString& name);
    void setPos(qint32 x, qint32 y);

public:
    explicit EditContactDialog(QWidget* parent = 0);
    ~EditContactDialog();

    void setValues(const QString& id);
    void hideBackButton();

private slots:
    void onSave();
    void onReturn();
    void onTextChanged();
    void onCursorPosChanged();

    void on_addOrgButton_clicked();
    void on_deleteOrgButton_clicked();

    bool isPhone(QString* str);

    void keyPressEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

    bool eventFilter(QObject*, QEvent* event);

private:
    Ui::EditContactDialog* ui;

    QSqlDatabase db;

    QPointer<AddOrgToPerson> addOrgToPerson;

    QList<QLineEdit*> phonesList;

    QStringList oldPhonesList;

    QValidator* Validator;

    QString contactId;
    QString orgId;
    QString number;

    QTextCursor textCursor;
};

#endif // EDITCONTACTDIALOG_H
