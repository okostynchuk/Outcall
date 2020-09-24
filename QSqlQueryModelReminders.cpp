/*
 * Переопределение класса QSqlQueryModel для закрашивания
 * в таблице строк с делегированными пользователю напоминаниями.
 */

#include "QSqlQueryModelReminders.h"
#include "RemindersDialog.h"

#include <QPainter>
#include <QCheckBox>

QSqlQueryModelReminders::QSqlQueryModelReminders()
{
}

/**
 * Выполняет установку родительской таблицы.
 */
void QSqlQueryModelReminders::setParentTable(QTableView* p)
{
    parentTable = p;
}

/**
 * Выполняет закраску нужных строк таблицы (переопределение функции).
 */
QVariant QSqlQueryModelReminders::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole && parentTable->indexWidget(index.sibling(index.row(), 1))->findChild<QCheckBox*>() == nullptr)
        return QBrush(QColor(254, 252, 196));

    return QSqlQueryModel::data(index, role);
}
