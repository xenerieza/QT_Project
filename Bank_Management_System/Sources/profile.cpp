#include "profile.h"
#include "ui_profile.h"
#include "database.h"
#include <QMessageBox>

profile::profile(const QString &UserEmail,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::profile),
    Email(UserEmail)
{
    ui->setupUi(this);
    this->setFixedSize(760,530);
    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    showCustomerInfo();

    // Disabled the text editing.
    ui->lineEdit_AccNo->setReadOnly(true);
    ui->lineEdit_Name->setReadOnly(true);
    ui->lineEdit_PhoneNo->setReadOnly(true);
    ui->lineEdit_Email->setReadOnly(true);
    ui->lineEdit_Birth->setReadOnly(true);
    ui->lineEdit_City->setReadOnly(true);
    ui->lineEdit_Country->setReadOnly(true);
    ui->textEdit_Address->setReadOnly(true);
}

profile::~profile()
{
    delete ui;
}

void profile::showCustomerInfo()
{
    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    QSqlQuery Sqry(DB_Conn);
    Sqry.prepare("SELECT * FROM Register WHERE User_Email = :email");
    Sqry.bindValue(":email",Email);

    if(Sqry.exec() && Sqry.next())
    {
        QString customerAccountNo = Sqry.value("AccountNo").toString();
        QString customerName = Sqry.value("Name").toString();
        QString customerPhoneNumber = Sqry.value("Phone_Number").toString();
        QString customerEmail = Sqry.value("User_Email").toString();
        QString customerBirth = Sqry.value("Birth").toString();
        QString customerCity = Sqry.value("City").toString();
        QString customerCountry = Sqry.value("Country").toString();
        QString customerAddress = Sqry.value("Address").toString();

        ui->lineEdit_AccNo->setText(customerAccountNo);
        ui->lineEdit_Name->setText(customerName);
        ui->lineEdit_PhoneNo->setText(customerPhoneNumber);
        ui->lineEdit_Email->setText(customerEmail);
        ui->lineEdit_Birth->setText(customerBirth);
        ui->lineEdit_City->setText(customerCity);
        ui->lineEdit_Country->setText(customerCountry);
        ui->textEdit_Address->setPlainText(customerAddress);
    }
    else
    {
        QMessageBox::critical(this, "Profile Status","Query or Database Error!\nError: " + Sqry.lastError().text());
    }
}
