#include "register.h"
#include "ui_register.h"
#include "login.h"
#include "database.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDate>


Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);

    login = new Login;

    this->setFixedSize(600, 480);
    //Change window title
    setWindowTitle("User Register");
    // Placeholder text
    ui->lineEdit_Name->setPlaceholderText("Name and Surname");
    ui->lineEdit_PhoneNumber->setPlaceholderText("12346123");
    ui->lineEdit_Email->setPlaceholderText("e.g. user@gmail.com");
    ui->lineEdit_Password->setPlaceholderText("Password");
    ui->lineEdit_Birth->setPlaceholderText("mm/dd/yyyy");
    ui->lineEdit_City->setPlaceholderText("City");
    ui->lineEdit_Country->setPlaceholderText("Country");
    ui->textEdit_Address->setPlaceholderText("Write your address");
    // Clear Button Enable
    ui->lineEdit_Name->setClearButtonEnabled(true);
    ui->lineEdit_PhoneNumber->setClearButtonEnabled(true);
    ui->lineEdit_Email->setClearButtonEnabled(true);
    ui->lineEdit_Password->setClearButtonEnabled(true);
    ui->lineEdit_Birth->setClearButtonEnabled(true);
    ui->lineEdit_City->setClearButtonEnabled(true);
    ui->lineEdit_Country->setClearButtonEnabled(true);
    // Hide Password
    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
}

Register::~Register()
{
    delete ui;
    delete login;
}

void Register::clearText()
{
    ui->lineEdit_Name->clear();
    ui->lineEdit_PhoneNumber->clear();
    ui->lineEdit_Email->clear();
    ui->lineEdit_Password->clear();
    ui->lineEdit_Birth->clear();
    ui->lineEdit_City->clear();
    ui->lineEdit_Country->clear();
    ui->textEdit_Address->clear();
}


void Register::on_pushButton_Register_clicked()
{
    Name = ui->lineEdit_Name->text();
    PhoneNumber = ui->lineEdit_PhoneNumber->text();
    Email = ui->lineEdit_Email->text();
    Pass = ui->lineEdit_Password->text();
    Birth = ui->lineEdit_Birth->text();
    City = ui->lineEdit_City->text();
    Country = ui->lineEdit_Country->text();
    Address = ui->textEdit_Address->toPlainText();

    QSqlDatabase DB_Conn = Database::instance().getDatabase();


    // Turn on the device and this device generates random data based on programming or system.
    std::random_device rd;
    // Choose an algorithm for random number gen.I choose the Mersenne Twister Algorithm for the following reasons.
    /*1- Balanced Distribution: MT is capable of generating high quality random numbers without repetition over along period of time.
    * This makes random numbers difficult to predict and provides a reliable level of randomness.
    *2- Portability: MT algorithm is supported in many programming languages and platforms.
    *3- High Level of Randomness: MT is capable of generating high quality random numbers without repetition over a long
    *period of time.
    *4- Open Source and Documentation: The MT algorithm has extensive open source documentation and is therefore easy to use.
    */
    std::mt19937 gen(rd());
    // Create a random values between 1 - 1000;
    std::uniform_int_distribution<int> AccNoDist(1,1000);
    // Create a random values between 1000000 - 999999;
    std::uniform_int_distribution<int> SubsNoDist(100000,900000);
    std::uniform_int_distribution<int> MeterNoDist(100000,900000);
    int accNo = AccNoDist(gen);
    int SubsNo = SubsNoDist(gen);
    int MeterNo = MeterNoDist(gen);

    // Create a random values for IBAN
    std::uniform_int_distribution<int> StartDigit(1,9);
    int sDigit = StartDigit(gen);
    std::uniform_int_distribution<int> RandomDigits(0,9);
    QString IBAN_Numbers;
    for(int i=0; i<19; ++i)
    {
        int digit = RandomDigits(gen);
        IBAN_Numbers.append(QString::number(digit));
    }
    QString IBAN = "TR" + QString::number(sDigit) + IBAN_Numbers + "5001";

    // Create a random values for Card Number
    std::uniform_int_distribution<int> NineDigit(100000000, 999999999);
    std::uniform_int_distribution<int> SevenDigit(1000000, 9999999);
    QString Nine = QString::number(NineDigit(gen));
    QString Seven = QString::number(SevenDigit(gen));
    QString CardNumber = Nine + Seven;

    // Create CVV value.
    std::uniform_int_distribution<int> CVV(100,999);
    int CVV_Number = CVV(gen);

    // Create and set valid date.
    QDateTime date = QDateTime::currentDateTime();
    QString dateFormat = date.toString("dd/MM/yyyy");
    QDate ValidDate = date.date().addYears(3);
    QString ValidDateText = ValidDate.toString("MM/yyyy");
    QString timeFormat = date.toString("hh:mm:ss");
    QString FDateTime = dateFormat + " " + timeFormat;

    if(!Name.isEmpty() && !PhoneNumber.isEmpty() && !Email.isEmpty() && !Pass.isEmpty() && !Birth.isEmpty() && !City.isEmpty() &&
        !Country.isEmpty() && !Address.isEmpty())
    {
        QSqlQuery checkEmail;
        checkEmail.prepare("SELECT User_Email FROM Register WHERE User_Email=:User_Email");
        checkEmail.bindValue(":User_Email",Email);

        if(checkEmail.exec() && checkEmail.next())
        {
            QMessageBox::critical(this,"Registration Statement", "This e-mail address is already used!");
        }
        else
        {
            QSqlQuery activity_Query;
            activity_Query.prepare("INSERT INTO Activity(AccountNo, User_Email, Balance, Name, OpeningDate, MeterNo, SubsNo) VALUES(:accNo, :email, :balance, :name, :date,  :mNo, :sNo)");
            activity_Query.bindValue(":accNo", accNo);
            activity_Query.bindValue(":email", Email);
            activity_Query.bindValue(":balance",Balance);
            activity_Query.bindValue(":name",Name);
            activity_Query.bindValue(":date",FDateTime);
            activity_Query.bindValue(":mNo", MeterNo);
            activity_Query.bindValue(":sNo",SubsNo);

            QSqlQuery card_Query;
            card_Query.prepare("INSERT INTO Card(User_Email, AccountNo, IBAN, CardNumber, CVV, OpeningDate, ValidDate) "
                               "VALUES(:email, :accNo, :iban, :cnumber, :cvv, :opdate, :vdate)");
            card_Query.bindValue(":email",Email);
            card_Query.bindValue(":accNo",accNo);
            card_Query.bindValue(":iban",IBAN);
            card_Query.bindValue(":cnumber",CardNumber);
            card_Query.bindValue(":cvv",CVV_Number);
            card_Query.bindValue(":opdate",FDateTime);
            card_Query.bindValue(":vdate",ValidDateText);

            QSqlQuery register_Query;
            register_Query.prepare("INSERT INTO Register(AccountNo, Name, Phone_Number, User_Email, Pass, Birth, City, Country, Address) "
                                   "VALUES(:AccountNo, :Name, :Phone_Number, :User_Email, :Pass, :Birth, :City, :Country, :Address)");
            register_Query.bindValue(":AccountNo", accNo);
            register_Query.bindValue(":Name", Name);
            register_Query.bindValue(":Phone_Number", PhoneNumber);
            register_Query.bindValue(":User_Email", Email);
            register_Query.bindValue(":Pass", Pass);
            register_Query.bindValue(":Birth", Birth);
            register_Query.bindValue(":City", City);
            register_Query.bindValue(":Country", Country);
            register_Query.bindValue(":Address", Address);

            QSqlQuery login_Query;
            login_Query.prepare("INSERT INTO Login(AccountNo, User_Email, Pass) VALUES(:AccountNo, :User_Email, :Pass)");
            login_Query.bindValue(":AccountNo", accNo);
            login_Query.bindValue(":User_Email", Email);
            login_Query.bindValue(":Pass", Pass);


            QSqlQuery addLogQuery;
            addLogQuery.prepare("INSERT INTO Log(AccountNo, User_Email, Name, Status, OprDate, SubType)"
                                "VALUES(:accNo, :email, :name, :status, :OprDate, :subtype)");
            addLogQuery.bindValue(":accNo",accNo);
            addLogQuery.bindValue(":email",Email);
            addLogQuery.bindValue(":name",Name);
            addLogQuery.bindValue(":status","Account Created");
            addLogQuery.bindValue(":OprDate",FDateTime);
            addLogQuery.bindValue(":subtype","-");

            if(activity_Query.exec() && card_Query.exec() && register_Query.exec() && login_Query.exec() && addLogQuery.exec())
            {
                QMessageBox::information(this,"Register Statement", "Registration has been done successfully!");
                clearText();
            }
            else
            {
                QString Title = "Register Statement";
                QString Text = "Registration was not completed successfully\nError Activity: "+activity_Query.lastError().text()+ "\nError Card: " + card_Query.lastError().text() +
                               "\nError Register: "   +register_Query.lastError().text()+"\nError Login: " +login_Query.lastError().text()+
                               "\nError Activity: "+"\nError Log: "+addLogQuery.lastError().text();
                QMessageBox::critical(this,Title, Text);
                return;
            }
        }
    }
    else
    {
        QMessageBox::critical(this, "Registration Statement", "Please fill in all required values!");
    }
}


void Register::on_pushButton_Clear_clicked()
{
    clearText();
}


void Register::on_pushButton_Back_clicked()
{
    auto res = QMessageBox::question(this, "Back to Login", "Are you sure you want to go back without creating a registration?"
                                     ,QMessageBox::Yes | QMessageBox::No);

    if(res == QMessageBox::Yes)
    {
        this -> close();
        login->show();
    }
}
