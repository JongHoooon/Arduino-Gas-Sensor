#include <MQUnifiedsensor.h>

/************************Hardware Related Macros************************************/
#define Board "Arduino UNO"
#define MQ9_Pin A0 
#define MQ8_Pin A1

/***********************Software Related Macros************************************/
#define MQ9_Type "MQ-9" 
#define MQ8_Type "MQ-8"

#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO

#define RatioMQ9CleanAir 9.6  // RS / R0 = 60 ppm
#define RatioMQ8CleanAir 70   // RS / R0 = 70 ppm 

/*****************************Globals***********************************************/
// Declare Sensor
MQUnifiedsensor MQ9(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ9_Pin, MQ9_Type);
MQUnifiedsensor MQ8(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ8_Pin, MQ8_Type);

void setup()
{
    Serial.begin(9600);
    MQ9.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ8.setRegressionMethod(1); // _PPM =  a*ratio^b

    MQ9.init();
    MQ8.init();

    Serial.print("MQ9 Calibrating please wait");
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

    Serial.print("MQ8 Calibrating please wait.");
    calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ8.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0 += MQ8.calibrate(RatioMQ8CleanAir);
        Serial.print(".");
    }
    MQ8.setR0(calcR0 / 10);
    Serial.println("  done!.");

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
    MQ8.update();
    /*
    MQ9 

    Exponential regression:
    GAS     | a      | b
    LPG     | 1000.5 | -2.186
    CH4     | 4269.6 | -2.648
    CO      | 599.65 | -2.244
    */

    /*

    MQ8 
    Exponential regression:
    GAS     | a      | b
    H2      | 976.97  | -0.688
    LPG     | 10000000 | -3.123
    CH4     | 80000000000000 | -6.666
    CO      | 2000000000000000000 | -8.074
    Alcohol | 76101 | -1.86
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

    MQ8.setA(2000000000000000000);
    MQ8.setB(-8.074);
    float mq8_co = MQ8.readSensor();

    Serial.print("|    ");
    Serial.print(LPG);
    Serial.print("    |    ");
    Serial.print(CH4);
    Serial.print("    |    ");
    Serial.print(CO);
    Serial.print("    |    ");
    Serial.print(mq8_co);
    Serial.println("    |");

    delay(500); // Sampling frequency
}
