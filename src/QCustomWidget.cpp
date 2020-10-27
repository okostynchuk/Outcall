#include "QCustomWidget.h"

QCustomWidget::QCustomWidget(QWidget* parent) : QWidget(parent)
{
}

void QCustomWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        QWidget::close();
    else
        QWidget::keyPressEvent(event);
}

