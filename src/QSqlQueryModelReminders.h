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

    void setParentTable(const QTableView* parentTable);
    QVariant data(const QModelIndex& index, qint32 role) const;

    const QTableView* parentTable;
};

#endif // QSQLQUERYMODELREMINDERS_H
