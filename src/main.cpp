#include <Wire.h>
#include "Adafruit_FRAM_I2C.h"
//#include "NVRAM.h"
//#include "CyclicNVRAMBuffer.h"
#include "NVRAMBaselineDetector.h"

/* Example code for the Adafruit I2C FRAM breakout */

/* Connect SCL (6)    to analog 5 (3 on pro micro)
   Connect SDA (5)   to analog 4 (2 on pro micro)
   Connect VDD  (8)  to 5.0V DC
   Connect GROUND (4) to common ground */
   
Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();

long count = 0;

void framWrite(uint16_t addr, byte data)
{
    fram.write8(addr, data);
}

byte framRead(uint16_t addr)
{
    return fram.read8(addr);
}

double getMax(double x, double y)
{
  return max(x, y);
}

NVRAM nvram(framWrite, framRead);

NVRAMBaselineDetector bd(0.05,  1000, 7*12,  &nvram, getMax);
// NVRAMBaselineDetector bd1(0.05, getMax, 1000, 7*12, 2*60*60, &nvram);
// NVRAMBaselineDetector bd2(0.05, getMax, 1000, 7*12, 2*60*60, &nvram);
// NVRAMBaselineDetector bd3(0.05, getMax, 1000, 7*12, 2*60*60, &nvram);



void setup(void) 
{
    Wire.begin();

    delay(100);
    Serial.begin(9600);

    while(!Serial)
    {
        //
    }
    
    if (fram.begin(0x50)) 
    {  
        // you can stick the new i2c addr in here, e.g. begin(0x51);
        Serial.println("Found I2C FRAM");
    } 
    else 
    {
        Serial.println(F("I2C FRAM not identified ... check your connections?\r\n"));
        Serial.println(F("Will continue in case this processor doesn't support repeated start\r\n"));
    }

    bd.init();
    // bd1.init();
    // bd2.init();
    // bd3.init();
/*
    for (int i = 0; i < 6; i++)
    {
        cb[i] = new CyclicNVRAMBuffer(1000 * (i + 1), 12, &nvram);
        cb[i]->initialize();
    }

    for (int i = 0; i < 6; i++)
    {
        CyclicNVRAMBuffer * b = cb[i];

        for (int j = 0; j<20; j++)
        {
            Serial.print("Pos:"); Serial.println(b->getPosition());
            for (int k = 0; k < 12; k++)
            {
                Serial.print(b->get(k), 2); Serial.print(" | ");
            }
            Serial.println();

            b->put(i + 0.01*j);
        }
    }

    for (int i = 0; i < 6; i++)
    {
        CyclicNVRAMBuffer * b = cb[i];

        for (int k = 0; k < 12; k++)
        {
            Serial.print(b->get(k), 2); Serial.print(" | ");
        }
        Serial.println();
    }
*/
}

void loop(void) 
{
    //return;
    bd.push(1.00*random(1000));
    // bd2.push(1.00*random(1000));
    // bd3.push(1.00*random(1000));
    delay(1000);
}
