#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dashboard.h"
#include "billpay.h"
#include "deposit.h"
#include "fundtransfer.h"
#include "profile.h"
#include "login.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &UserEmail, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void receiveData(QString Balance);

private slots:
    void on_BacktoLogin_triggered();

    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

    void on_actionAbout_QT_triggered();

private:
    Ui::MainWindow *ui;
    Login *login;
    dashboard *dash;
    Billpay *bill;
    deposit *dep;
    Fundtransfer *fund;
    profile *prf;
};

#endif // MAINWINDOW_H
