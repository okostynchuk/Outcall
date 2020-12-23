#ifndef CHOOSEEMPLOYEE_H
#define CHOOSEEMPLOYEE_H

#include "AsteriskManager.h"
#include "Global.h"

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
    void sendEmployee(const QStringList& employee);

public slots:
    void setValues(QStringList& employee);
    void setPos(qint32 x, qint32 y);

public:
    explicit ChooseEmployee(QWidget* parent = 0);
    ~ChooseEmployee();

private slots:
    void onChoose(QListWidgetItem* item);
    void onRemove(QListWidgetItem* item);
    void onSearch();

    void on_chooseAllButton_clicked();
    void on_removeAllButton_clicked();
    void on_applyButton_clicked();

    void keyPressEvent(QKeyEvent* event);

private:
    Ui::ChooseEmployee* ui;

    QStringList m_extensions;
    QStringList m_results;
};

#endif // CHOOSEEMPLOYEE_H
