#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(const QString &UserEmail,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    login = new Login;

    dash = new dashboard(UserEmail);
    bill = new Billpay(UserEmail);
    dep = new deposit();
    fund = new Fundtransfer(UserEmail);
    prf = new profile(UserEmail);

    ui->setupUi(this);
    ui->tabWidget->addTab(dash, "Dashboard");
    ui->tabWidget->addTab(bill, "Payments");
    ui->tabWidget->addTab(dep, "Deposit | Withdraw");
    ui->tabWidget->addTab(fund, "FundTransfer");
    ui->tabWidget->addTab(prf, "Profile");

    connect(dep,  SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    connect(fund, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    connect(bill, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
}


MainWindow::~MainWindow()
{
    delete ui;
    disconnect(dep,  SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    disconnect(fund, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    disconnect(bill, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    disconnect(dash, SIGNAL(logDataUpdated()), dep, SLOT(logDataUpdated()));
    delete dep;
    delete dash;
    delete bill;
    delete fund;
    delete prf;
}

void MainWindow::receiveData(QString Balance)
{
    dash->updateDashBalanceLabel(Balance);
    fund->updateFTransferBalanceLabel(Balance);
}



void MainWindow::on_BacktoLogin_triggered()
{
    this->close();
    login->show();

}


void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionAbout_triggered()
{
    QString Title = "About Project";
    QString Text = "Welcome to the Bank Mangement System\n";
    QString Text1 = "In this project, I aimed to make Deposits-Withdrawals, Bill Payments and Money Transfers.\n";
    QString Text2 = "Each process is carefully designed with its own structures.\n";
    QString Text3 = "As a result, a Bank Management System close to today's systems has been successfully implemented.";
    QString FText = Text + Text1 + Text2 + Text3;
    QMessageBox::information(this, Title, FText);
}


void MainWindow::on_actionAbout_QT_triggered()
{
    QApplication::aboutQt();
}

