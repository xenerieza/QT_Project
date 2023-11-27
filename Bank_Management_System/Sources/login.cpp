#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include "register.h"
#include "database.h"
#include <QMessageBox>
#include <QCloseEvent>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    setWindowTitle("Login");
    // Placeholder text
    ui->lineEdit_User->setPlaceholderText("e.g. bank@gmail.com");
    ui->lineEdit_Password->setPlaceholderText("Password");
    // Clear Button Enable
    ui->lineEdit_User->setClearButtonEnabled(true);
    ui->lineEdit_Password->setClearButtonEnabled(true);
    // Password mode set
    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
    // icons
    QIcon user("your_path/member_icon");
    QIcon pass("your_path/password_icon");
    ui->lineEdit_User->addAction(user, QLineEdit::LeadingPosition);
    ui->lineEdit_Password->addAction(pass, QLineEdit::LeadingPosition);

    QSqlDatabase DB_Conn = Database::instance().getDatabase();
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_SignIn_clicked()
{
    QString userEmail = ui->lineEdit_User->text();
    QString userPassword = ui->lineEdit_Password->text();

    if(!userEmail.isEmpty() && !userPassword.isEmpty())
    {
        if(LoginUser(userEmail, userPassword))
        {
            MainWindow *mainWindow = new MainWindow(userEmail);
            this->close();
            mainWindow->show();
        }
    }
    else
    {
        QMessageBox::critical(this,"Login Statement", "Please enter all required values!");
        ui->lineEdit_User->clear();
        ui->lineEdit_Password->clear();
    }
}

bool Login::LoginUser(QString &Email, QString &password)
{
    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    if(!DB_Conn.isValid())
    {
        qDebug() << "Invalid Database Connection.";
        return false;
    }

    if (!DB_Conn.isOpen())
    {
        if (!DB_Conn.open())
        {
            qDebug() << "Failed to open database connection." << DB_Conn.lastError().text();
            return false;
        }
    }


    QSqlQuery loginUser(DB_Conn);
    loginUser.prepare("SELECT * FROM Login WHERE User_Email = :Email AND Pass=:password");
    loginUser.bindValue(":Email", Email);
    loginUser.bindValue(":password",password);

    if(loginUser.exec() && loginUser.next())
    {
        QMessageBox::information(this, "Login Statement", "Customer login successfully!");
        return true;
    }
    else
    {
        QMessageBox::critical(this, "Login Statement", "Login failed or user not found!\nError: " + loginUser.lastError().text());
    }

    return false; // Login Failed.
}

void Login::on_pushButton_SignUp_clicked()
{
    Register* registerWindow = new Register;
    registerWindow->show();
    this->close();
}

void Login::closeEvent(QCloseEvent *event)
{
    event->accept();
}


