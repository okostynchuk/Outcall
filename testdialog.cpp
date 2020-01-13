#include "testdialog.h"
#include "ui_testdialog.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>

testDialog::testDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::testDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->pushButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->pushButton_2, &QAbstractButton::clicked, this, &testDialog::onClear);
}

testDialog::~testDialog()
{
    delete ui;
}

void testDialog::onClear()
{
    //123
    //456
    QSqlQueryModel *setquery1 = new QSqlQueryModel;
    setquery1->setQuery("SELECT * FROM entry");
    QTableView *tv = new QTableView(this);
    tv->setModel(setquery1);
    ui->tableView->setModel(setquery1);
}

