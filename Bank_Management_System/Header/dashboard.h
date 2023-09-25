#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QDialog>

#include "deposit.h"
#include <QSqlQueryModel>

namespace Ui {
class dashboard;
}

class dashboard : public QDialog
{
    Q_OBJECT

public:
    explicit dashboard(const QString &UserEmail, QWidget *parent = nullptr);
    ~dashboard();

    void showCardInfo();
    void showCustInfo();
    void updateDashBalanceLabel(QString Balance);

public slots:
    void updateLogView();
    void showLog();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::dashboard *ui;
    QString Email;
    double Balance = 0.0;
    deposit* dep;
    QSqlQueryModel* logModel;

};

#endif // DASHBOARD_H
