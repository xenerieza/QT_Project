#include "deposit.h"
#include "ui_deposit.h"
#include "database.h"
#include <QMessageBox>
#include <QCloseEvent>

deposit::deposit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deposit)
{
    ui->setupUi(this);
    ui->lineEdit_Amount->setPlaceholderText("Enter Amount");
    ui->lineEdit_AccNo->setPlaceholderText("Enter Account No");

    ui->lineEdit_W_Amount->setPlaceholderText("Enter Amount");
    ui->lineEdit_W_AccNo->setPlaceholderText("Enter Account No");

    QSqlDatabase DB_Conn = Database::instance().getDatabase();
}

deposit::~deposit()
{
    delete ui;
}


void deposit::on_pushButton_Deposit_clicked()
{
    double depositAmount = ui->lineEdit_Amount->text().toDouble();
    int accountNumber = ui->lineEdit_AccNo->text().toInt();
    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    QSqlQuery dpQry;
    dpQry.prepare("SELECT User_Email, Balance, Name FROM Activity WHERE AccountNo = :accNo");
    dpQry.bindValue(":accNo", accountNumber);
    if(dpQry.exec())
    {
        if(dpQry.next())
        {
            QString DepMail = dpQry.value(0).toString();
            double checkAmount = dpQry.value(1).toDouble();
            QString DepName = dpQry.value(2).toString();
            double newAmount = checkAmount + depositAmount;
            QString f3Amount = QString::number(newAmount,'f',3);
            QSqlQuery qry;
            qry.prepare("UPDATE Activity SET Balance = :newAmount WHERE AccountNo = :AccountNo");
            qry.bindValue(":newAmount", f3Amount);
            qry.bindValue(":AccountNo", accountNumber);

            if (qry.exec())
            {
                QString Title = "Deposit Status";
                QString Text = "Deposit Successfull!\nAvailable Balance = ";
                QMessageBox::information(this,Title, Text +  QString::number(newAmount,'f',3));

                ui->lineEdit_Amount->clear();
                ui->lineEdit_AccNo->clear();

                // Signals for balance_Label
                emit sendData(f3Amount);


                QSqlQuery addDepositLogQuery;

                addDepositLogQuery.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                           "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subt)");
                addDepositLogQuery.bindValue(":accNo",accountNumber);
                addDepositLogQuery.bindValue(":email",DepMail);
                addDepositLogQuery.bindValue(":name",DepName);
                addDepositLogQuery.bindValue(":status","Cash Deposit");
                addDepositLogQuery.bindValue(":subt","-");
                if(addDepositLogQuery.exec())
                {
                    qDebug() << "Log entry completed";
                    // Signals for tableview
                    emit logDataUpdated();
                }
                else
                {
                    QMessageBox::critical(this, "Log Entry", "Log entry failed!\nError: " + addDepositLogQuery.lastError().text());
                    return;
                }
            }
            else
            {
                QMessageBox::critical(this, "Deposit Status", "The Deposit Failed!");
                return;
            }
        }
    }

    else
    {
        QMessageBox::critical(this, "Account Status", "Account Not Found!");
        return;
    }
}



void deposit::on_pushButton_Withdraw_clicked()
{
    int accNo = ui->lineEdit_W_AccNo->text().toInt();
    double withdrawAmount = ui->lineEdit_W_Amount->text().toDouble();

    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery wdrQry;
    wdrQry.prepare("SELECT User_Email, Balance, Name FROM Activity WHERE AccountNo = :accNo");
    wdrQry.bindValue(":accNo", accNo);
    if(wdrQry.exec())
    {
        if(wdrQry.next())
        {
            QString WithMail = wdrQry.value(0).toString();
            double checkAmount = wdrQry.value(1).toDouble();
            QString Withname = wdrQry.value(2).toString();

            if(checkAmount >= withdrawAmount)
            {
                double newAmount = checkAmount-withdrawAmount;
                QString f3Amount = QString::number(newAmount,'f',3);
                QSqlQuery checkQuery;
                checkQuery.prepare("UPDATE Activity SET Balance = :newAmount WHERE AccountNo = :AccNo");
                checkQuery.bindValue(":newAmount",f3Amount);
                checkQuery.bindValue(":AccNo",accNo);

                if(checkQuery.exec())
                {
                    QString Title = "Withdraw Status";
                    QString Text = "Withdraw Succesfull.\nNewAmount = " + QString::number(newAmount);
                    QMessageBox::information(this, Title, Text);
                    ui->lineEdit_W_Amount->clear();
                    ui->lineEdit_W_AccNo->clear();

                    // Signals for label_Balance
                    emit sendData(f3Amount);

                    QSqlQuery addWthdrLogQuery;

                    addWthdrLogQuery.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                             "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subt)");
                    addWthdrLogQuery.bindValue(":accNo",accNo);
                    addWthdrLogQuery.bindValue(":email",WithMail);
                    addWthdrLogQuery.bindValue(":name",Withname);
                    addWthdrLogQuery.bindValue(":status","Cash Withdrawal");
                    addWthdrLogQuery.bindValue(":subt","-");
                    if(addWthdrLogQuery.exec())
                    {
                        // Signals for tableview
                        emit logDataUpdated();
                        qDebug() << "Log entry completed";
                    }
                    else
                    {
                        QMessageBox::critical(this, "Log Entry", "Log entry failed!\nError: " + addWthdrLogQuery.lastError().text());
                        return;
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Account Status", "Customer Account Not Found!");
                    return;
                }
            }
            else
            {
                QMessageBox::critical(this, "Withdraw Status", "Insufficient Balance!");
            }
        }
    }
    else
    {
        QMessageBox::critical(this, "Account Status", "Customer Account Not Found!");
        qDebug() << "Account Not Found!" << wdrQry.lastError().text();
    }
}

void deposit::closeEvent(QCloseEvent *event)
{
    event->accept();
}

