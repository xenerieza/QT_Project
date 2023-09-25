#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>


namespace Ui {
class profile;
}

class profile : public QDialog
{
    Q_OBJECT

public:
    explicit profile(const QString &UserEmail,QWidget *parent = nullptr);
    ~profile();

    void showCustomerInfo();

private:
    Ui::profile *ui;
    QString Email;
};

#endif // PROFILE_H
