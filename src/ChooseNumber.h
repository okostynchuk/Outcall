#ifndef CHOOSENUMBER_H
#define CHOOSENUMBER_H

#include "Global.h"
#include "PlaceCallDialog.h"
#include "AsteriskManager.h"

#include <QDialog>
#include <QValidator>
#include <QEvent>
#include <QLineEdit>
#include <QSqlDatabase>
#include <QSqlQuery>

class PlaceCallDialog;

namespace Ui {
class ChooseNumber;
}

class ChooseNumber : public QDialog
{
    Q_OBJECT

signals:
    void sendNumber(const QString& number);
    void phonesOrderChanged();

public:
    explicit ChooseNumber(QWidget* parent = 0);
    ~ChooseNumber();

    void setValues(const QString id, qint32 status);

private slots:
    void onCall(const QString& number);

    void phonePriorityChanged();

    void showEvent(QShowEvent* event);

    bool eventFilter(QObject* target, QEvent* event);

    void on_saveButton_clicked();

private:
    Ui::ChooseNumber* ui;

    QSqlDatabase m_db;

    QList<QLineEdit*> m_phones;
    QList<QLineEdit*> m_phonesComments;

    enum Statuses
    {
        call,
        orderChange,
    };
};

#endif // CHOOSENUMBER_H
