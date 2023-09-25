#include "dashboard.h"
#include "ui_dashboard.h"
#include "database.h"
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QCloseEvent>

dashboard::dashboard(const QString &UserEmail, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dashboard),
    Email(UserEmail)
{
    ui->setupUi(this);
    this->setFixedSize(760,530);
    dep = new deposit;

    QSqlDatabase DB_Conn = Database::instance().getDatabase();

    logModel = new QSqlQueryModel(this);
    ui->tableView->setModel(logModel);

    showCardInfo();
    showCustInfo();
    showLog();
    ui->tableView->reset();

    connect(dep, SIGNAL(logDataUpdated()), this, SLOT(showLog()));
}

dashboard::~dashboard()
{
    delete ui;
    disconnect(dep, SIGNAL(logDataUpdated()), this, SLOT(showLog()));
    delete dep;
}

void dashboard::showCardInfo()
{

    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    QSqlQuery card_Query;
    card_Query.prepare("SELECT IBAN, CardNumber, CVV, ValidDate FROM Card WHERE User_Email = :email");
    card_Query.bindValue(":email",Email);
    if(card_Query.exec())
    {
        if(card_Query.next())
        {
            QString IBAN = card_Query.value(0).toString();
            QString CardNo = card_Query.value(1).toString();
            int CVV = card_Query.value(2).toInt();
            QString ValidDate = card_Query.value(3).toString();
            QString formattedIBAN = IBAN;
            formattedIBAN.insert(4,"-");
            formattedIBAN.insert(9,"-");
            formattedIBAN.insert(14,"-");
            formattedIBAN.insert(19,"-");
            formattedIBAN.insert(24,"-");
            formattedIBAN.insert(29,"-");

            QString formattedCardNo = CardNo;
            formattedCardNo.insert(4," ");
            formattedCardNo.insert(9," ");
            formattedCardNo.insert(14," ");


            ui->label_IBAN->setText(formattedIBAN);
            ui->label_Card->setText(formattedCardNo);
            ui->label_Valid->setText(ValidDate);
            ui->label_CVV->setText(QString::number(CVV));

        }
        else
        {
            QString Title = "Dashboard Status";
            QString Text = "Customer account not found!.\nError: " + card_Query.lastError().text();
            QMessageBox::critical(this, Title, Text);
        }
    }
    else
    {
        QString Title = "Dashboard Status";
        QString Text = "Database or Query Error.\nError: " + card_Query.lastError().text();
        QMessageBox::critical(this, Title, Text);
    }
}

void dashboard::showCustInfo()
{
    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    QSqlQuery act_Query;
    act_Query.prepare("SELECT Balance, Name FROM Activity WHERE User_Email = :email");
    act_Query.bindValue(":email",Email);
    if(act_Query.exec())
    {
        if(act_Query.next())
        {
            Balance = act_Query.value(0).toDouble();
            QString Balance_Text = QString::number(Balance) + " TL";
            QString Name = act_Query.value(1).toString();

            ui->label_Name->setText(Name);
            ui->label_name->setText(Name);
            ui->label_Balance->setText(Balance_Text);
        }
        else
        {
            QString Title = "Dashboard Status";
            QString Text = "Customer account not found!.\nError: " + act_Query.lastError().text();
            QMessageBox::critical(this, Title, Text);
        }
    }
    else
    {
        QString Title = "Dashboard Status";
        QString Text = "Database or Query Error.\nError: " + act_Query.lastError().text();
        QMessageBox::critical(this, Title, Text);
    }
}

void dashboard::showLog()
{
    QSqlDatabase DB_Conn = Database::instance().getDatabase();
    QSqlQuery Log_Query;
    Log_Query.prepare("SELECT AccountNo, Name, Status, OprDate, SubType FROM Log WHERE User_Email = :email ORDER BY OprDate DESC LIMIT 5");
    Log_Query.bindValue(":email",Email);
    if(Log_Query.exec())
    {
        logModel->setQuery(std::move(Log_Query));
    }
    else
    {
        qDebug() << "Query Error:" << Log_Query.lastError().text();
    }

}

void dashboard::updateDashBalanceLabel(QString Balance)
{
    ui->label_Balance->setText(Balance);
}

void dashboard::updateLogView()
{
    showLog();
}

void dashboard::closeEvent(QCloseEvent *event)
{
    event->accept();
}

