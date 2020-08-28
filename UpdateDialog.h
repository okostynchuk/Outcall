#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QSimpleUpdater.h>

#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

public slots:
    void resetFields();
    void checkForUpdates();
    void updateChangelog (const QString& url);
    void displayAppcast (const QString& url, const QByteArray& reply);

private:
    Ui::UpdateDialog *ui;
    QSimpleUpdater *m_updater;
};

#endif // UPDATEDIALOG_H
