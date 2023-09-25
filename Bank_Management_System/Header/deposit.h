#ifndef DEPOSIT_H
#define DEPOSIT_H

#include <QDialog>


namespace Ui {
class deposit;
}

class deposit : public QDialog
{
    Q_OBJECT

public:
    explicit deposit(QWidget *parent = nullptr);
    ~deposit();

signals:
    void sendData(QString Balance);
    void logDataUpdated();

private slots:    
    void on_pushButton_Deposit_clicked();

    void on_pushButton_Withdraw_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::deposit *ui;
    QString Email;
};

#endif // DEPOSIT_H
