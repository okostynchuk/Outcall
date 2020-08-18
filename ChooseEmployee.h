#ifndef CHOOSEEMPLOYEE_H
#define CHOOSEEMPLOYEE_H

#include "AsteriskManager.h"

#include <QDialog>
#include <QListWidgetItem>
#include <QKeyEvent>

namespace Ui {
class ChooseEmployee;
}

class ChooseEmployee : public QDialog
{
    Q_OBJECT

signals:
    void sendEmployee(QString);

public:
    explicit ChooseEmployee(QWidget *parent = 0);
    ~ChooseEmployee();

private slots:
    void onChoose(QListWidgetItem *item);
    void onSearch();

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::ChooseEmployee *ui;

    QStringList extensions;
};

#endif // CHOOSEEMPLOYEE_H
