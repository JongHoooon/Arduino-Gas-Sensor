#include <MQUnifiedsensor.h>

/************************Hardware Related Macros************************************/
#define Board "Arduino UNO"
#define MQ9_Pin_1 A0 
#define MQ9_Pin_2 A1
#define MQ9_Pin_3 A2

/***********************Software Related Macros************************************/
#define MQ9_Type "MQ-9" 

#define Voltage_Resolution 5
#define ADC_Bit_Resolution 10 // For arduino UNO/MEGA/NANO

#define RatioMQ9CleanAir 9.6  // RS / R0 = 60 ppm

/*****************************Globals***********************************************/
// Declare Sensor
MQUnifiedsensor MQ9_1(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ9_Pin_1, MQ9_Type);
MQUnifiedsensor MQ9_2(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ9_Pin_2, MQ9_Type);
// MQUnifiedsensor MQ9_3(Board, Voltage_Resolution, ADC_Bit_Resolution, MQ9_Pin_3, MQ9_Type);

void setup()
{
    Serial.begin(9600);
    MQ9_1.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ9_2.setRegressionMethod(1);
    // MQ9_3.setRegressionMethod(1);

    MQ9_1.init();
 
    // MQ9_3.init();

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
        while (1);
    }

    if (calcR0 == 0)
    {
        Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
        while (1);
    }

    Serial.print("MQ9 Calibrating please wait.");
    calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ9_2.update(); // Update data, the arduino will read the voltage from the analog pin
        calcR0 += MQ9_2.calibrate(RatioMQ9CleanAir);
        Serial.print(".");
    }
    MQ9_2.setR0(calcR0 / 10);
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

//     Serial.print("MQ9 Calibrating please wait.");
//     calcR0 = 0;
//     for (int i = 1; i <= 10; i++)
//     {
//         MQ9_3.update(); // Update data, the arduino will read the voltage from the analog pin
//         calcR0 += MQ9_3.calibrate(RatioMQ9CleanAir);
//         Serial.print(".");
//     }
//     MQ9_3.setR0(calcR0 / 10);
//     Serial.println("  done!.");

//     if (isinf(calcR0))
//     {
//         Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
//         while (1);
//     }
//     if (calcR0 == 0)
//     {
//         Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
//         while (1);
//     }

    /*****************************  MQ CAlibration ********************************************/
    Serial.println("** Values from MQ-9 ****");
    Serial.println("|    LPG   |  CH4 |   CO  |");
}

void loop()
{
    MQ9_1.update(); // Update data, the arduino will read the voltage from the analog pin
    MQ9_2.update();
    /*
    MQ9 

    Exponential regression:
    GAS     | a      | b
    LPG     | 1000.5 | -2.186
    CH4     | 4269.6 | -2.648
    CO      | 599.65 | -2.244
    */

    MQ9_1.setA(1000.5);
    MQ9_1.setB(-2.186);             // Configure the equation to to calculate LPG concentration
    float LPG = MQ9_1.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    MQ9_1.setA(4269.6);
    MQ9_1.setB(-2.648);             // Configure the equation to to calculate LPG concentration
    float CH4 = MQ9_1.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    MQ9_1.setA(599.65);
    MQ9_1.setB(-2.244);            // Configure the equation to to calculate LPG concentration
    float CO = MQ9_1.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup

    MQ9_2.setA(599.65);
    MQ9_2.setB(-2.244);
    float co_2 = MQ9_2.readSensor();

    // Serial.print("|    ");
    // Serial.print(LPG);
    // Serial.print("    |    ");
    // Serial.print(CH4);
    // Serial.print("    |    ");
    // Serial.print(CO);
    // Serial.print("    |    ");
    // Serial.print(mq8_co);
    // Serial.println("    |");
    
    Serial.println((String) "gasData : " + LPG + ", " + CH4 + ", " + CO + ", " + co_2);


    delay(500); // Sampling frequency
}
