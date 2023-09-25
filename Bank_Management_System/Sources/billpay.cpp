#include "billpay.h"
#include "ui_billpay.h"
#include "database.h"
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>

Billpay::Billpay(const QString &UserEmail, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Billpay),
    Email(UserEmail)
{
    ui->setupUi(this);
    this->setFixedSize(760,530);

    // SetPlaceholder text
    ui->lineEdit_Utility->setPlaceholderText("Gas(m3) | Elc(Kwh) | Water(m3)");

    // Clear Button Enable
    ui->lineEdit_Utility->setClearButtonEnabled(true);


    // Utility Bill ComboBox
    // Clear the comboBox items.
    ui->comboBox_Utility->clear();
    // Add item to comboBox_utility
    ui->comboBox_Utility->addItem("Choose Your Bill");
    ui->comboBox_Utility->addItem("Electricity");
    ui->comboBox_Utility->addItem("Water");
    ui->comboBox_Utility->addItem("Gas");

    // Internet Bill comboBox
    // Clear the comboBox
    ui->comboBox_Internet->clear();
    // Add item to comboBox_Internet
    ui->comboBox_Internet->addItem("Choose Your Net Company");
    ui->comboBox_Internet->addItem("TTNET");
    ui->comboBox_Internet->addItem("TurkNet");
    ui->comboBox_Internet->addItem("Turkcell SuperOnline");
    ui->comboBox_Internet->addItem("Turksat Kablonet");

    // Mobile Bill comboBox
    // Clear the comboBox
    ui->comboBox_Mobil->clear();
    // Add item to comboBox_Mobil
    ui->comboBox_Mobil->addItem("Choose Your Phone Company");
    ui->comboBox_Mobil->addItem("Turkcell");
    ui->comboBox_Mobil->addItem("Vodafone");
    ui->comboBox_Mobil->addItem("Turk Telekom Mobile");

    // Car Bill comboBox
    // Clear the comboBox
    ui->comboBox_Car->clear();
    // Add item to comboBox_Car;
    ui->comboBox_Car->addItem("Choose Your Bill");
    ui->comboBox_Car->addItem("MTV");

    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    MetersSubsNo(Email);
}

Billpay::~Billpay()
{
    delete ui;
}

// Get meter and subs number in the database.
void Billpay::MetersSubsNo(const QString &UserEmail)
{
    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery qry;
    qry.prepare("SELECT * FROM Activity WHERE User_Email = :email");
    qry.bindValue(":email", UserEmail);

    if(qry.exec() && qry.next())
    {
        QString customerMeterNo = qry.value("MeterNo").toString();
        QString customerSubsNo = qry.value("SubsNo").toString();

        ui->label_meterNo->setText(customerMeterNo);
        ui->label_SubNo->setText(customerSubsNo);
    }
}

// Utility Bill Payment Functions
void Billpay::CalculateUtilityPrices(const QString &SelectedItem)
{
    if(SelectedItem == "Electricity" && !ui->lineEdit_Utility->text().isEmpty())
    {
        ElcUsageAmount = ui->lineEdit_Utility->text().toDouble();
        ElectricityPrice = ElcUsageAmount * UnitElc;
    }
    else if(SelectedItem == "Gas" && !ui->lineEdit_Utility->text().isEmpty())
    {
        GasUsageAmount = ui->lineEdit_Utility->text().toDouble();
        GasPrice = GasUsageAmount * ConspAmount * lCalorificValue;
    }
    else if(SelectedItem == "Water" && !ui->lineEdit_Utility->text().isEmpty())
    {
        WaterUsageAmount = ui->lineEdit_Utility->text().toDouble();
        WaterPrice = (WaterUsageAmount * UnitWater) + ECT;
    }
}

void Billpay::PayUtility()
{

    QString SelectedItem = ui->comboBox_Utility->currentText();

    if (SelectedItem == "Choose Your Bill")
    {
        QMessageBox::critical(this, "Payment Statement", "Please Select Bill!");
        return;
    }


    CalculateUtilityPrices(SelectedItem);

    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery Uqry;
    Uqry.prepare("SELECT AccountNo, Balance, Name FROM Activity WHERE User_Email = :email");
    Uqry.bindValue(":email", Email);

    if (Uqry.exec())
    {
        if (Uqry.next())
        {
            int AccNo = Uqry.value(0).toInt();
            double checkAmount = Uqry.value(1).toDouble();
            QString Name = Uqry.value(2).toString();

            double totalUtilityPrice = ElectricityPrice + GasPrice + WaterPrice;

            if (checkAmount >= totalUtilityPrice)
            {
                double newAmount = checkAmount - totalUtilityPrice;
                QString f3Amount = QString::number(newAmount,'f',3);

                QSqlQuery checkUpdate;
                checkUpdate.prepare("UPDATE Activity SET Balance = :newAmount WHERE User_Email = :email");
                checkUpdate.bindValue(":newAmount", f3Amount);
                checkUpdate.bindValue(":email", Email);

                if (checkUpdate.exec())
                {
                    QString Title = "Payment Status";
                    QString Text = "Bill Payment is Complete.\nAvailable Balance = " + f3Amount + " TL";

                    emit sendData(f3Amount);

                    QMessageBox::information(this, Title, Text);

                    QSqlQuery addBillPayQuery;

                    addBillPayQuery.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                            "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subt)");
                    addBillPayQuery.bindValue(":accNo",AccNo);
                    addBillPayQuery.bindValue(":email",Email);
                    addBillPayQuery.bindValue(":name",Name);
                    addBillPayQuery.bindValue(":status", "Bill Payment");
                    addBillPayQuery.bindValue(":subt",SelectedItem);
                    if(addBillPayQuery.exec())
                    {
                        qDebug() << "Log entry completed";
                    }
                    else
                    {
                        QMessageBox::critical(this, "Log Entry", "Log entry failed!\nError: " + addBillPayQuery.lastError().text());
                        addBillPayQuery.finish();
                        return;
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Payment Status", "User Not Found!");
                }
            }
            else
            {
                QMessageBox::critical(this, "Payment Status", "Insufficient Balance!");
            }
        }
    }
    else
    {
        QMessageBox::critical(this, "Account Statement", "Account Not Found!");
    }
}
void Billpay::on_pushButton_Utility_clicked()
{
    PayUtility();
}

// Internet Bill Payment Functions
void Billpay::PayNet()
{
    QString SelectedItem = ui->comboBox_Internet->currentText();

    if (SelectedItem == "Choose Your Net Company")
    {
        QMessageBox::critical(this, "Payment Statement", "Please Select Your Net Company!");
        return;
    }

    // Check company selection
    int NetAmount = -1;

    if(SelectedItem == "TurkNet")
    {
        NetAmount = TurkNet;
    }
    else if(SelectedItem == "Turkcell SuperOnline")
    {
        NetAmount = Turkcell_Net;
    }
    else if(SelectedItem == "TTNET")
    {
        NetAmount = TTNET;
    }
    else if(SelectedItem == "Turksat Kablonet")
    {
        NetAmount = KabloNet;
    }

    if(NetAmount == -1)
    {
        QMessageBox::critical(this, "Payment Statement", "Invalid Net Company Selection!");
        return;
    }

    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    QSqlQuery Nqry;
    Nqry.prepare("SELECT AccountNo, Balance, Name FROM Activity WHERE User_Email = :email");
    Nqry.bindValue(":email",Email);
    if(Nqry.exec())
    {
        if(Nqry.next())
        {
            int AccNo = Nqry.value(0).toInt();
            double checkAmount = Nqry.value(1).toDouble();
            QString Name = Nqry.value(2).toString();

            if(checkAmount >= NetAmount)
            {
                double NewAmount = checkAmount - NetAmount;
                QString f3Amount = QString::number(NewAmount,'f',3);

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE Activity SET Balance = :newAmount WHERE User_Email = :email");
                updateQuery.bindValue(":newAmount", f3Amount);
                updateQuery.bindValue(":email",Email);

                if(updateQuery.exec())
                {
                    QString Title = "Payment Status";
                    QString Text = "Bill Payment is Complete.\nAvailable Balance = " + f3Amount + " TL";

                    QMessageBox::information(this, Title, Text);

                    emit sendData(f3Amount);

                    QSqlQuery addNetLog;

                    addNetLog.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                      "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subt)");
                    addNetLog.bindValue(":accNo",AccNo);
                    addNetLog.bindValue(":email",Email);
                    addNetLog.bindValue(":name",Name);
                    addNetLog.bindValue(":status", "Net Bill Payment");
                    addNetLog.bindValue(":subt",SelectedItem);
                    if(addNetLog.exec())
                    {
                        qDebug() << "Log entry completed";
                    }
                    else
                    {
                        QMessageBox::critical(this, "Log Entry", "Log entry failed!\nError: " + addNetLog.lastError().text());
                        addNetLog.finish();
                        return;
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Account Status", "User Not Found!");
                }
            }
            else
            {
                QMessageBox::critical(this, "Payment Status", "Insufficient Balance!");
            }
        }
    }
    else
    {
        QMessageBox::critical(this, "Account Status", "User Not Found!");
    }
}

void Billpay::on_pushButton_Internet_clicked()
{
    PayNet();
}


// Mobile Bill Payment Functions
void Billpay::PayMobile()
{
    QString SelectedItem = ui->comboBox_Mobil->currentText();

    if (SelectedItem == "Choose Your Phone Company")
    {
        QMessageBox::critical(this, "Payment Statement", "Please Select Your Phone Company!");
        return;
    }

    // Check company selection
    int NetAmount = -1;

    if(SelectedItem == "Turkcell")
    {
        NetAmount = Turkcell_Phone;
    }
    else if(SelectedItem == "Vodafone")
    {
        NetAmount = Vodafone_Phone;
    }
    else if(SelectedItem == "Turk Telekom Mobile")
    {
        NetAmount = TT_Phone;
    }

    if(NetAmount == -1)
    {
        QMessageBox::critical(this, "Payment Statement", "Invalid Phone Company Selection!");
        return;
    }

    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery Nqry;
    Nqry.prepare("SELECT AccountNo, Balance, Name FROM Activity WHERE User_Email = :email");
    Nqry.bindValue(":email",Email);
    if(Nqry.exec())
    {
        if(Nqry.next())
        {
            int accNo = Nqry.value(0).toInt();
            double checkAmount = Nqry.value(0).toDouble();
            QString Name = Nqry.value(2).toString();

            if(checkAmount >= NetAmount)
            {
                double NewAmount = checkAmount - NetAmount;
                QString f3Amount = QString::number(NewAmount,'f',3);

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE Activity SET Balance = :newAmount WHERE User_Email = :email");
                updateQuery.bindValue(":newAmount", f3Amount);
                updateQuery.bindValue(":email",Email);

                if(updateQuery.exec())
                {
                    QString Title = "Payment Status";
                    QString Text = "Bill Payment is Complete.\nAvailable Balance = " + f3Amount + " TL";

                    QMessageBox::information(this, Title, Text);

                    emit sendData(f3Amount);

                    QSqlQuery addPhoneLog;
                    addPhoneLog.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                        "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subt)");
                    addPhoneLog.bindValue(":accNo",accNo);
                    addPhoneLog.bindValue(":email",Email);
                    addPhoneLog.bindValue(":name",Name);
                    addPhoneLog.bindValue(":status","Phone Bill Payment");
                    addPhoneLog.bindValue(":subt",SelectedItem);
                    if(addPhoneLog.exec())
                    {
                        qDebug() << "Log entry completed";
                    }
                    else
                    {
                        QMessageBox::critical(this, "Log Entry", "Log entry failed!\nError: " + addPhoneLog.lastError().text());
                        addPhoneLog.finish();
                        return;
                    }

                }
                else
                {
                    QMessageBox::critical(this, "Account Status", "User Not Found!");
                }
            }
            else
            {
                QMessageBox::critical(this, "Payment Status", "Insufficient Balance!");
            }
        }
    }
    else
    {
        QMessageBox::critical(this, "Account Status", "User Not Found!");
    }
}

void Billpay::on_pushButton_Mobil_clicked()
{
    PayMobile();
}


void Billpay::on_pushButton_Car_clicked()
{
    QString SelectedItem = ui->comboBox_Car->currentText();

    if (SelectedItem == "Choose Your Bill")
    {
        QMessageBox::critical(this, "Payment Statement", "Please Select Your Bill");
        return;
    }

    // Check company selection
    int NetAmount = -1;

    if(SelectedItem == "MTV")
    {
        NetAmount = Car_MTV;
    }

    if(NetAmount == -1)
    {
        QMessageBox::critical(this, "Payment Statement", "Invalid Bill Selection!");
        return;
    }

    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery Cqry;
    Cqry.prepare("SELECT AccountNo, Balance, Name FROM Activity WHERE User_Email = :email");
    Cqry.bindValue(":email",Email);
    if(Cqry.exec())
    {
        if(Cqry.next())
        {
            int accNo = Cqry.value(0).toInt();
            double checkAmount = Cqry.value(1).toDouble();
            QString Name = Cqry.value(2).toString();

            if(checkAmount >= NetAmount)
            {
                double NewAmount = checkAmount - NetAmount;
                QString f3Amount = QString::number(NewAmount,'f',3);

                emit sendData(f3Amount);

                QSqlQuery updateQuery;
                updateQuery.prepare("UPDATE Activity SET Balance = :newAmount WHERE User_Email = :email");
                updateQuery.bindValue(":newAmount", f3Amount);
                updateQuery.bindValue(":email",Email);

                if(updateQuery.exec())
                {
                    QString Title = "Payment Status";
                    QString Text = "Bill Payment is Complete.\nAvailable Balance = " + f3Amount + " TL";

                    QMessageBox::information(this, Title, Text);

                    QSqlQuery addCarLog;
                    addCarLog.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                      "VALUES(:accNo, :email, :name, :status, CURRENT_TIMESTAMP, :subt)");
                    addCarLog.bindValue(":accNo",accNo);
                    addCarLog.bindValue(":email",Email);
                    addCarLog.bindValue(":name",Name);
                    addCarLog.bindValue(":status","CAR MTV Payment");
                    addCarLog.bindValue(":subt",SelectedItem);
                    if(addCarLog.exec())
                    {
                        qDebug() << "Log entry completed";
                    }
                    else
                    {
                        QMessageBox::critical(this, "Log Entry", "Log entry failed!\nError: " + addCarLog.lastError().text());
                        addCarLog.finish();
                        return;
                    }
                }
                else
                {
                    QMessageBox::critical(this, "Account Status", "User Not Found!");
                }
            }
            else
            {
                QMessageBox::critical(this, "Payment Status", "Insufficient Balance!");
            }
        }
    }
    else
    {
        QMessageBox::critical(this, "Account Status", "User Not Found!");
    }
}

void Billpay::closeEvent(QCloseEvent *event)
{
    event->accept();
}
