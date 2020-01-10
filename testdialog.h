#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>

namespace Ui {
class testDialog;
}

class testDialog : public QDialog
{
    Q_OBJECT

public:
    explicit testDialog(QWidget *parent = 0);
    ~testDialog();

private:
    void onClear();
    Ui::testDialog *ui;
};

#endif // TESTDIALOG_H
