#include <MQUnifiedsensor.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

/************************Hardware Related Macros************************************/
#define Board "Arduino UNO"
#define MQ9_Pin_1 A0
#define MQ9_Pin_3 A2

// #define MAXDO 3
// #define MAXCS 4
// #define MAXCLK 5

/***********************Software Related Macros************************************/
#define MQ9_Type "MQ-9"

#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO

#define RatioMQ9CleanAir 9.6 // RS / R0 = 60 ppm

/*****************************Globals***********************************************/
// Declare Sensor
MQUnifiedsensor MQ9_1(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ9_Pin_1, MQ9_Type);
MQUnifiedsensor MQ9_3(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ9_Pin_3, MQ9_Type);

// Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void setup()
{
    Serial.begin(9600);
    MQ9_1.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ9_3.setRegressionMethod(1);

    MQ9_1.init();

    Serial.print("MQ9 Calibrating please wait");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ9_1.update();
        calcR0 += MQ9_1.calibrate(RatioMQ9CleanAir);
        Serial.print(".");
    }
    MQ9_1.setR0(calcR0 / 10);
    Serial.println(" done!.");

    if (isinf(calcR0))
    {
        Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
        while (1)
            ;
    }

    if (calcR0 == 0)
    {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
        while (1)
            ;
    }

    Serial.print("MQ9 Calibrating please wait.");
    calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ9_3.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0 += MQ9_3.calibrate(RatioMQ9CleanAir);
        Serial.print(".");
    }
    MQ9_3.setR0(calcR0 / 10);
    Serial.println("  done!.");

    if (isinf(calcR0))
    {
        Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
        while (1)
            ;
    }
    if (calcR0 == 0)
    {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
        while (1)
            ;
    }

    // while (!Serial)
    //     delay(1); // wait for Serial on Leonardo/Zero, etc

    // Serial.println("MAX31855 test");
    // // wait for MAX chip to stabilize
    // delay(500);
    // Serial.print("Initializing sensor...");
    // if (!thermocouple.begin())
    // {
    //     Serial.println("ERROR.");
    //     while (1)
    //         delay(10);
    // }

    /*****************************  MQ CAlibration ********************************************/
    Serial.println("** Values from MQ-9 ****");
    Serial.println("|    LPG   |  CH4 |   CO  |");
}

void loop()
{
    MQ9_1.update(); // Update data, the arduino will read the voltage from the analog pin
    MQ9_3.update();

    /*
    MQ9

    Exponential regression:
    GAS     | a      | b
    LPG     | 1000.5 | -2.186
    CH4     | 4269.6 | -2.648
    CO      | 599.65 | -2.244
    */

    MQ9_1.setA(4269.6);
    MQ9_1.setB(-2.648);               // Configure the equation to to calculate LPG concentration
    float ch4_1 = MQ9_1.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    MQ9_1.setA(599.65);
    MQ9_1.setB(-2.244);
    float co_1 = MQ9_1.readSensor();

    MQ9_3.setA(4269.6);
    MQ9_3.setB(-2.648);
    float ch4_3 = MQ9_3.readSensor();

    MQ9_3.setA(599.65);
    MQ9_3.setB(-2.244);
    float co_3 = MQ9_3.readSensor();

    // double c = thermocouple.readCelsius();
    // if (isnan(c))
    // {
    //     Serial.println("Thermocouple fault(s) detected!");
    //     uint8_t e = thermocouple.readError();
    //     if (e & MAX31855_FAULT_OPEN)
    //         Serial.println("FAULT: Thermocouple is open - no connections.");
    //     if (e & MAX31855_FAULT_SHORT_GND)
    //         Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    //     if (e & MAX31855_FAULT_SHORT_VCC)
    //         Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
    // }

    Serial.println((String) "gasData : " +
                   ch4_1 + ", " + co_1 + ", " +
                   ch4_3 + ", " + co_3);

    delay(500); // Sampling frequency
}
