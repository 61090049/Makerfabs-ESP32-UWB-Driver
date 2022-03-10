#include <SPI.h>
#include "../lib/DW1000/DW1000Ranging.h"
#include "config.h"

#define DEV_ADDR ADDR_ANCHOR_1

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;  

void setup()
{
    Serial.begin(115200);
    delay(1000);

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachBlinkDevice(newBlink);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);

    //DW1000Ranging.useRangeFilter(true);

    DW1000Ranging.startAsAnchor(DEV_ADDR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    //DW1000Ranging.startAsAnchor(DEV_ADDR, DW1000.MODE_SHORTDATA_FAST_LOWPOWER);
    //DW1000Ranging.startAsAnchor(DEV_ADDR, DW1000.MODE_LONGDATA_FAST_LOWPOWER);
    //DW1000Ranging.startAsAnchor(DEV_ADDR, DW1000.MODE_SHORTDATA_FAST_ACCURACY);
    //DW1000Ranging.startAsAnchor(DEV_ADDR, DW1000.MODE_LONGDATA_FAST_ACCURACY);
    //DW1000Ranging.startAsAnchor(DEV_ADDR, DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}

void loop()
{
    DW1000Ranging.loop();
}

void newRange()
{
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}

void newBlink(DW1000Device *device)
{
    Serial.print("blink; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
}
