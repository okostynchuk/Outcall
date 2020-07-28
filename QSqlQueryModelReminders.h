#ifndef QSQLQUERYMODELREMINDERS_H
#define QSQLQUERYMODELREMINDERS_H

#include <QSqlQueryModel>
#include <QObject>
#include <QTableView>

class QSqlQueryModelReminders : public QSqlQueryModel
{
    Q_OBJECT

public:
    QSqlQueryModelReminders();
    QVariant data(const QModelIndex &index, int role) const;
    void setParentTable(QTableView *p);
    QTableView *parentTable;
};

#endif // QSQLQUERYMODELREMINDERS_H
