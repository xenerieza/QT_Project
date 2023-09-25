#ifndef BILLPAY_H
#define BILLPAY_H

#include <QDialog>


namespace Ui {
class Billpay;
}

class Billpay : public QDialog
{
    Q_OBJECT

public:
    explicit Billpay(const QString &UserEmail, QWidget *parent = nullptr);
    ~Billpay();

    void MetersSubsNo(const QString &UserEmail);
    void CalculateUtilityPrices(const QString &SelectedItem);

signals:
    void sendData(QString Balance);

private slots:
    void on_pushButton_Utility_clicked();

    void on_pushButton_Internet_clicked();

    void on_pushButton_Mobil_clicked();

    void on_pushButton_Car_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Billpay *ui;
    QString Email;

    // UtilityPrices hesaplama sonuçları için değişkenler
    double ElcUsageAmount = 0.0;
    double GasUsageAmount = 0.0;
    double WaterUsageAmount = 0.0;
    double ElectricityPrice = 0.0;
    double GasPrice = 0.0;
    double WaterPrice = 0.0;


    // Utility Bill Payment.
    /*  -------------------------------------------------------------------------------------------------------------  */
    // GAS variables and calculation in Türkiye.
    // Gas usage amount -> This value; It is calculated by subtracting the first index value from the last index value.
    // Last index - first index = Gas usage amount (m3)
    // Correctionfactor: This factor is used to calculate the actual energy content of consumed natural gas.
    double CorrectionFactor = 0.91;
    // Adjusted Consumption: It is obtained by applying the correction coefficient to the consumption value.
    double AdjConsumption =  GasUsageAmount * CorrectionFactor;
    //Calorific Value (CV) describes how much heat is generated when a known volume of gas is completely burned away.
    //first CalorificValue -> Kcal / m3
    double fCalorificValue = 9200;
    //last Calorific Value -> Kwh / m3
    double lCalorificValue = fCalorificValue / 860.42;
    //Consumption Amount of Energy  The consumption value is obtained by the correction coefficient and the average upper calorific value and its application.
    double ConspAEnergy = AdjConsumption * lCalorificValue; // Kwh
    double ConspAmount = 0.5047;
    //double GasPrice = ConspAEnergy * ConspAmount;

    // Electricity variables and calculation in Türkiye
    // Kwh on your bill.
    //double ElcUsageAmount;
    // UnitElc -> 1 unit of electricity. We assumed the price of one unit of electricity to be 1.6
    double UnitElc = 1.6;
    //double ElectricityPrice = ElcUsageAmount * UnitElc;

    // Water variables and calculation in Türkiye
    // m3 on your bill.
    //double WaterUsageAmount;
    // UnitWater -> 1 unit of water. We assumed the price of one unit of water to be 16.5 (Average of Türkiye);
    double UnitWater = 16.5;
    // ECT -> environmental consumption tax. per cubic meter 1.5 (2023)
    double ECT = 1.5;
    // Last water price
    //double WaterPrice = (WaterUsageAmount * UnitWater) + ECT;
    /*  -------------------------------------------------------------------------------------------------------------  */


    /*  -------------------------------------------------------------------------------------------------------------  */
    // Internet Bill Payment
    // Internet tariff price in Türkiye. In this example we choose tariff price of 100mb download speed and 12 months.
    int TurkNet = 250;
    int Turkcell_Net = 419;
    int KabloNet = 284;
    int TTNET = 285; // Türk Telekom NET
    /*  -------------------------------------------------------------------------------------------------------------  */

    /*  -------------------------------------------------------------------------------------------------------------  */
    // Mobile phone package price 1 months and variables.
    int Turkcell_Phone = 230;
    int Vodafone_Phone = 185;
    int TT_Phone = 145;
    /*  -------------------------------------------------------------------------------------------------------------  */


    //  Car Bill Payment
    /*  -------------------------------------------------------------------------------------------------------------  */
    //  There are multiple factors and values ​​that make up a vehicle's MTV.
    //  It is calculated by multiplying engine power, engine displacement, fuel type, vehicle age and finally the market value of the vehicle.
    //  At this point, we will accept the MTV(Motor Vehicle Tax = Motorlu Taşıtlar Vergisi) value as 1000 in Türkiye.
    int Car_MTV = 1000;
    /*  -------------------------------------------------------------------------------------------------------------  */


    // Main Functions
    // Calculate all function
    // Utility Invoices Functions
    void PayUtility();

    // Internet Invoices Functions
    void PayNet();

    // Mobile Phone Invoices Functions
    void PayMobile();

    // Vehicle Invoices Functions
    void PayMTV();

};

#endif // BILLPAY_H
