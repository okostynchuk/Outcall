#ifndef QCUSTOMWIDGET_H
#define QCUSTOMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QKeyEvent>

class QCustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QCustomWidget(QWidget* parent = 0);

private slots:
    void keyPressEvent(QKeyEvent* event);
};

#endif // QCUSTOMWIDGET_H
