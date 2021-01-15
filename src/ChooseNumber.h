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

class PlaceCallDialog;

namespace Ui {
class ChooseNumber;
}

class ChooseNumber : public QDialog
{
    Q_OBJECT

signals:
    void sendNumber(const QString& number);

public:
    explicit ChooseNumber(QWidget* parent = 0);
    ~ChooseNumber();

    void setValues(const QStringList &numbers);

private slots:
    void onCall(const QString& number);

    void showEvent(QShowEvent* event);

    bool eventFilter(QObject* target, QEvent* event);

private:
    Ui::ChooseNumber* ui;

    QSqlDatabase m_db;

    QList<QLineEdit*> m_phones;
};

#endif // CHOOSENUMBER_H
