#include "fundtransfer.h"
#include "ui_fundtransfer.h"
#include "database.h"
#include <QMessageBox>
#include <QCloseEvent>

Fundtransfer::Fundtransfer(const QString &UserEmail, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Fundtransfer),
    Email(UserEmail)
{
    ui->setupUi(this);
    ui->lineEdit_Amount->setPlaceholderText("Please enter amount");
    ui->lineEdit_AccName->setPlaceholderText("Recipient Name");
    ui->lineEdit_AccNo->setPlaceholderText("Recipient Bank Account");
    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    showCustomerInfo();
}

Fundtransfer::~Fundtransfer()
{
    delete ui;
}

void Fundtransfer::showCustomerInfo()
{
    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery showQry;
    showQry.prepare("SELECT * FROM Activity WHERE User_Email = :email");
    showQry.bindValue(":email",Email);

    if(showQry.exec() && showQry.next())
    {
        QString customerAccountNo = showQry.value("AccountNo").toString();
        QString customerBalance = showQry.value("Balance").toString();

        ui->label_AccNo->setText(customerAccountNo);
        ui->label_Balance->setText(customerBalance);
    }
}

void Fundtransfer::updateFTransferBalanceLabel(QString Balance)
{
    ui->label_Balance->setText(Balance);
}

void Fundtransfer::on_pushButton_Send_clicked()
{

    QString AccNo = ui->lineEdit_AccNo->text();
    QString Name = ui->lineEdit_AccName->text();
    QString PaymentType = ui->comboBox->currentText();
    Amount = ui->lineEdit_Amount->text().toDouble();

    if(PaymentType == "Payment Type Selection")
    {
        QMessageBox::critical(this, "Payment Type", "Please select a payment type!");
        return;
    }

    // Access the connection name in the database.
    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    QSqlQuery FT_qry(DB_Conn);

    // Begin the Transaction.
    if(!DB_Conn.transaction())
    {
        QMessageBox::critical(this, "Transaction", "Transaction Begin Error!\nError: "+ FT_qry.lastError().text());
        return;
    }

    // Find sender account information
    FT_qry.prepare("SELECT AccountNo, Balance, Name FROM Activity WHERE User_Email = :email");
    FT_qry.bindValue(":email",Email);
    if(!FT_qry.exec() || !FT_qry.first())
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Sender Account Statement", "Account not found or query error!\nError: "+ FT_qry.lastError().text());
        return;
    }

    senderAccNo = FT_qry.value(0).toInt();
    senderBalance = FT_qry.value(1).toDouble();
    senderName = FT_qry.value(2).toString();

    // Find receiver account information
    FT_qry.prepare("SELECT AccountNo, Name, Balance, User_Email FROM Activity WHERE AccountNo = :accNo AND Name = :name");
    FT_qry.bindValue(":accNo",AccNo);
    FT_qry.bindValue(":name",Name);
    if(!FT_qry.exec() || !FT_qry.next())
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Receiver Account Statement", "Account not found or query error!\nError: " + FT_qry.lastError().text());
        return;
    }

    ReceiverAccNo = FT_qry.value(0).toInt();
    ReceiverName = FT_qry.value(1).toString();
    ReceiverBalance = FT_qry.value(2).toDouble();
    ReceiverEmail = FT_qry.value(3).toString();

    // Check the value entered by the user. If the value entered by the user is lower than the senderBalance value, send an error.
    // An electronic funds transfer (EFT), or direct deposit, is a digital money movement from one bank account to another. We assumed that price is 2.86
    if(Amount <= 0.0 || senderBalance < (Amount + EFT_Price))
    {
        QMessageBox::critical(this, "Fundtransfer Status", "Insufficient Balance!");
        return;
    }

    // Get the required values from the database.
    NewSenderAmount = senderBalance - Amount;
    NewReceiverAmount = ReceiverBalance + Amount;

    QString sendAmount = QString::number(NewSenderAmount,'f',3);

    // Start updating the Balance value in the sending account.
    FT_qry.prepare("UPDATE Activity SET Balance = Balance -:amount WHERE AccountNo = :accNo");
    FT_qry.bindValue(":amount", Amount);
    FT_qry.bindValue(":accNo",senderAccNo);
    if(FT_qry.exec())
    {
        QString Title = "Fundtransfer Status";
        QString Text = QString::number(Amount) +" TL was withdrawn from:\nAccount No = " + QString::number(senderAccNo);
        QMessageBox::information(this,Title, Text);
        //Signals for balance_Label
        emit sendData(sendAmount);

    }
    else
    {
        DB_Conn.rollback();
        QMessageBox::critical(this,"Fundtransfer Status","Query error Fund Transfer Error! for sender.\nError: " + FT_qry.lastError().text());
        return;
    }

    FT_qry.prepare("SELECT * FROM Activity WHERE User_Email = :email");
    FT_qry.bindValue(":email",Email);

    if(FT_qry.exec() && FT_qry.next())
    {
        QString customerAccountNo = FT_qry.value("AccountNo").toString();
        QString customerBalance = FT_qry.value("Balance").toString();

        ui->label_AccNo->setText(customerAccountNo);
        ui->label_Balance->setText(customerBalance);
    }
    else
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Fundtransfer Status", "Query error or Fund Transfer Error! for receiver.\nError: " + FT_qry.lastError().text());
        return;
    }


    // Start updating the Balance value in the receiver account.
    FT_qry.prepare("UPDATE Activity SET Balance = Balance + :amount WHERE AccountNo = :accNo");
    FT_qry.bindValue(":amount",Amount);
    FT_qry.bindValue(":accNo",ReceiverAccNo);
    if(FT_qry.exec())
    {
        QString Title = "Fundtransfer Status";
        QString Text = QString::number(Amount) + " TL added to Account: " + QString::number(ReceiverAccNo) +
                       " and \nNew Balance: " + QString::number(NewReceiverAmount) + " TL";
        QMessageBox::information(this, Title, Text);
    }
    else
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Fundtransfer Status", "Query error or Fund Transfer Error! for receiver.\nError: " + FT_qry.lastError().text());
        return;
    }


    // Log operations in the database.
    // Start adding to log entry for sender account.
    FT_qry.prepare("INSERT INTO Log (AccountNo, User_Email, Name, Status, OprDate, SubType) "
                   "VALUES (:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subType)");
    FT_qry.bindValue(":accNo",senderAccNo);
    FT_qry.bindValue(":email",Email);
    FT_qry.bindValue(":name",senderName);
    FT_qry.bindValue(":status", "EFT - Cash Withdrawal");
    FT_qry.bindValue(":subType",PaymentType);
    if(!FT_qry.exec())
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Log Entry", "Log entry for sender failed!\nError: " + FT_qry.lastError().text());
        return;
    }
    else
    {
        qDebug() << "Log Entry Completed";
    }

    // Start adding to log entry for receiver account.
    FT_qry.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                   "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subType)");
    FT_qry.bindValue(":accNo",ReceiverAccNo);
    FT_qry.bindValue(":email",ReceiverEmail);
    FT_qry.bindValue(":name",ReceiverName);
    FT_qry.bindValue(":status","EFT - Cash Deposit");
    FT_qry.bindValue(":subType",PaymentType);
    if(!FT_qry.exec())
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Log Entry", "Log entry for receiver account failed!\nError: " + FT_qry.lastError().text());
        return;
    }
    else
    {
        qDebug() << "Log Entry Completed";
    }

    if(DB_Conn.commit())
    {
        qDebug() << "Transaction commit successfully done!";
    }
    else
    {
        DB_Conn.rollback();
        QMessageBox::critical(this, "Transaction", "Transaction Commit Error!\nError: " + FT_qry.lastError().text());
        return;
    }
}

void Fundtransfer::closeEvent(QCloseEvent *event)
{
    event->accept();
}

