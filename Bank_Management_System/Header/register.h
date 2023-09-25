#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QMessageBox>
#include "login.h"


namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

    void clearText();

private slots:
    void on_pushButton_Register_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Back_clicked();

private:
    Ui::Register *ui;
    Login* login;

    QString Name = " ";
    QString PhoneNumber = "-1";
    QString Email = " ";
    QString Pass = " ";
    QString Birth = " ";
    QString City = " ";
    QString Country = " ";
    QString Address = " ";
    double Balance = 0.0;
};

#endif // REGISTER_H
