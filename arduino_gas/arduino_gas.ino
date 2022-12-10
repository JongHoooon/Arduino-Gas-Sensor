#include <MQUnifiedsensor.h>

/************************Hardware Related Macros************************************/
#define Board ("Arduino UNO")
#define Pin (A0) // Analog input 0 of your arduino

/***********************Software Related Macros************************************/
#define Type ("MQ-9") // MQ4
#define Voltage_Resolution (5)
#define ADC_Bit_Resolution (10) // For arduino UNO/MEGA/NANO
#define RatioMQ9CleanAir (9.6)  // RS / R0 = 60 ppm
/*****************************Globals***********************************************/
// Declare Sensor
MQUnifiedsensor MQ9(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

void setup()
{
    Serial.begin(9600);
    MQ9.setRegressionMethod(1); // _PPM =  a*ratio^b

    MQ9.init();

    Serial.print("Calibrating please wait");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ9.update();
        calcR0 += MQ9.calibrate(RatioMQ9CleanAir);
        Serial.print(".");
    }
    MQ9.setR0(calcR0 / 10);
    Serial.println(" done!.");

    if (isinf(calcR0))
    {
        Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
        while (1);
    }

    if (calcR0 == 0)
    {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
        while (1);
    }

    /*****************************  MQ CAlibration ********************************************/
    Serial.println("** Values from MQ-9 ****");
    Serial.println("|    LPG   |  CH4 |   CO  |");
}

void loop()
{
    MQ9.update(); // Update data, the arduino will read the voltage from the analog pin
    /*
    Exponential regression:
    GAS     | a      | b
    LPG     | 1000.5 | -2.186
    CH4     | 4269.6 | -2.648
    CO      | 599.65 | -2.244
    */

    MQ9.setA(1000.5);
    MQ9.setB(-2.186);             // Configure the equation to to calculate LPG concentration
    float LPG = MQ9.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    MQ9.setA(4269.6);
    MQ9.setB(-2.648);             // Configure the equation to to calculate LPG concentration
    float CH4 = MQ9.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    MQ9.setA(599.65);
    MQ9.setB(-2.244);            // Configure the equation to to calculate LPG concentration
    float CO = MQ9.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    Serial.print("|    ");
    Serial.print(LPG);
    Serial.print("    |    ");
    Serial.print(CH4);
    Serial.print("    |    ");
    Serial.print(CO);
    Serial.println("    |");

    delay(500); // Sampling frequency
}