#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

    bool LoginUser(QString &Email, QString &password);

private slots:
    void on_pushButton_SignIn_clicked();

    void on_pushButton_SignUp_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
