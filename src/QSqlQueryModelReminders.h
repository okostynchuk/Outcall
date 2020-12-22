#ifndef QSQLQUERYMODELREMINDERS_H
#define QSQLQUERYMODELREMINDERS_H

#include <QSqlQueryModel>
#include <QObject>
#include <QTableView>

class QSqlQueryModelReminders : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit QSqlQueryModelReminders(QObject* parent = 0);

    void setParentTable(const QTableView* parentTable);
    QVariant data(const QModelIndex& index, qint32 role) const;

    const QTableView* m_parentTable;
};

#endif // QSQLQUERYMODELREMINDERS_H
