#ifndef FUNDTRANSFER_H
#define FUNDTRANSFER_H

#include <QDialog>


namespace Ui {
class Fundtransfer;
}

class Fundtransfer : public QDialog
{
    Q_OBJECT

public:
    explicit Fundtransfer(const QString &UserEmail, QWidget *parent = nullptr);
    ~Fundtransfer();

    void showCustomerInfo();

    void updateFTransferBalanceLabel(QString Balance);

signals:
    void sendData(QString Balance);

private slots:
    void on_pushButton_Send_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Fundtransfer *ui;
    QString Email;

    double Amount = 0.0;
    double EFT_Price = 2.86;

    int senderAccNo = 1;
    double NewSenderAmount = 0.0;
    QString senderName = "";
    double senderBalance = 0.0;

    double NewReceiverAmount = 0.0;
    int ReceiverAccNo = 0;
    QString ReceiverName = "";
    double ReceiverBalance = 0.0;
    QString ReceiverEmail = "";
};

#endif // FUNDTRANSFER_H
