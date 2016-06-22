/* LPS-main
 * Demonstration of tag <-> multiple anchors ranging as 
 * well as accelerometer capability.
 * Based partly on source code from Decawave for their EVK
 * Niklas Casaril 2015
 */

// These are used outside of the cmake build world
// typically for the arduino suite.
//
#define LPS_DEMO
// Only uncomment one of these
// #define LPS_V11
#define LPSMINI_V10
//
// Inertial sensors and output, remove or comment out to avoid inertial 
// measurements in your serial feed
// Only for LPS_V11
// #define LPS_V11_ACC
// Only with LPSMINI_V10
// #define LPSMINI_ACC
//
// In arduino suite, select the following board / processor combination depending on what
// board you are using:
// LPS:     Board=Arduino Nano, Processor=ATmega328
// LPSMini: Board=Arduino Pro or Pro Mini, Processor=ATmega328 (3.3V, 8MHz)


#include <stdint.h>
#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <avr/wdt.h>

#include <deca_device_api.h>
#include <deca_instance.h>
#include <deca_spi.h>
#include <lps.h>
#include <lps-dip.h>
#include <SPI.h>
#include <serialcobs.h>

#ifdef LPS_V11_ACC
#include <Wire.h>
#include <LIS2DH.h>
#endif

#if (defined(LPSMINI_V10) || defined(LPS_V11_ACC))
#include <I2cMaster.h>
TwiMaster i2c(true);
//SoftI2cMaster i2c(A4,A5);

// This function read Nbytes bytes from I2C device at address Address. 
// Put read bytes starting at register Register in the Data array. 
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
    // Set register address
    uint8_t a = (Address<<1);
    i2c.start(a | I2C_WRITE);
    i2c.write(Register);

    i2c.restart(a | I2C_READ);
    uint8_t *p = Data;
    for (uint8_t i=1;i<=Nbytes;i++)
        *(p++) = i2c.read(i==Nbytes);
    i2c.stop();
}

// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
    i2c.start((Address<<1) | I2C_WRITE);
    i2c.write(Register);
    i2c.write(Data);
    i2c.stop();
}
#endif

#ifdef LPSMINI_ACC
#include <MS561101BA.h>
MS561101BA baro = MS561101BA(&i2c);
int32_t temperature = 0, pression = 0;
int16_t mx=0, my=0, mz=0;
#endif

uint8 instance_addr = 0;
int instance_mode = TAG;

#ifdef LPS_V11_ACC
LIS2DH acceleration(&i2c, 0x19);
int16_t ax, ay, az;
uint8_t accel_interval=255; 
#endif

#include "ApplicationMonitor.h"
Watchdog::CApplicationMonitor ApplicationMonitor;

void setup() 
{
    // Setup watchdog: 
    // Note: the watchdog reset does not work with arduino bootloader
    // but only with optiboot
    wdt_reset();
#ifndef LPS_DEMO      
    wdt_enable(WDTO_8S);
    wdt_reset();
#else // LPS_DEMO
    wdt_disable();  
#endif

    Serial.begin(115200);

    ApplicationMonitor.Dump(Serial);
    ApplicationMonitor.EnableWatchdog(Watchdog::CApplicationMonitor::Timeout_4s);

#ifdef LPS_V09
    Serial.println(F("LPS v0.9 Setup"));
#endif
#ifdef LPS_V10
    Serial.println(F("LPS v1.0 Setup"));
#endif
#ifdef LPS_V11
    Serial.println(F("LPS v1.1 Setup"));
#endif
#ifdef LPSMINI_V10
    Serial.println(F("LPSMini v1.0 Setup"));

    // Setup mpu-9250 in i2c bypass mode and 
    // disable INT sink to allow reading of DIP
    I2CwriteByte(MPU9250_ADDRESS,0x37,0x02 | (1<<7));
#endif

#ifdef LPSMINI_ACC
    uint8_t acc_id = 0;
    I2Cread(0x69,0x75, 1, &acc_id);
    if (acc_id == 0x71)
        Serial.println(F("MPU-9250 Acc Ok"));
    else
    {
        Serial.println(F("MPU-9250 Acc fail, who am I:"));
        Serial.println(acc_id, HEX);
        delay(1000);
    }

    // Configure gyroscope range
    I2CwriteByte(MPU9250_ADDRESS,27,GYRO_FULL_SCALE_2000_DPS);
    // Configure accelerometers range
    I2CwriteByte(MPU9250_ADDRESS,28,ACC_FULL_SCALE_16_G);
    
    // Read magnetometer id
    uint8_t mag_id = 0;
    I2Cread(0x0C,0x00, 1, &mag_id);

    if (mag_id == 0x48)
        Serial.println(F("MPU-9250 Mag Ok"));
    else
    {
        Serial.println(F("MPU-9250 Mag fail, who I am:"));
        Serial.println(mag_id, HEX);
        delay(1000);
    }

    // Request first magnetometer single measurement
    I2CwriteByte(MAG_ADDRESS,0x0A,0x01);

    // Init barometer
    baro.init(MS561101BA_ADDR_CSB_LOW);
#endif


#ifndef LPS_V09
    uint8_t dipaddress = LPS::readDIP();
#else
    uint8_t dipaddress = 0x07;
#endif

#if (defined(LPSMINI_V10) && !defined(LPSMINI_ACC))
    // Lower the INT pin of acc to reduce power usage
    I2CwriteByte(MPU9250_ADDRESS,0x37,0x02 | (0<<7));
#endif

    instance_mode = ((dipaddress&0x80) == 0x00) ? ANCHOR : TAG;
    instance_addr = dipaddress&0x7f;

    SPI.begin();
    // initalize the  data ready and chip select pins:
    // Note we are not allowed to drive RSTPin
    pinMode(deca_chipSelectPin, OUTPUT);
    digitalWrite(deca_chipSelectPin, HIGH);

    while(LPS::initDWRanging(instance_mode, instance_addr) == -1)
    {
        Serial.println(F("Could not init DW Ranging"));
        delay(1000);
    }
    Serial.println(F("DWM Ok"));

    if (instance_mode == TAG)
    {
        Serial.print(F("Tag "));
    }
    else
    {
        Serial.print(F("Anchor "));
    }
    Serial.print(F("addr: "));
    Serial.print((uint32_t)(instance_get_addr()>>32), HEX);
    Serial.print((uint32_t)(instance_get_addr()&0xffffffff), HEX);
    Serial.println(F(" "));

#ifdef LPS_V11_ACC
    // Activate pullup on interrupt input
    pinMode(ACC_INT, INPUT_PULLUP);

    acceleration.init();
    Serial.print(F("LIS2DH "));
    if (!acceleration.whoAmI())
    {
        Serial.println(F("fail"));
    }
    else
        Serial.println(F("ok"));
#endif

    LPS::initInterrupt();

    Serial.println(F("Setup done"));
    uint8_t null = 0;
    Serial.write(&null, 1);
}


uint32_t _cycles_without_lps_range = 0;
bool _serial_complete_packet = false;
uint16_t _n = 0;

void loop() {
    uint64 addr = 0;
    uint32_t rangeinmm = 0;
    int16 pwr = 0;

#ifndef LPS_DEMO             
    RangePacket p_range;
    p_range.setFrom(instance_addr&0x7f);

    wdt_reset();

    // Send an empty packet now and then for id purposes
    if (instance_mode == TAG && _cycles_without_lps_range > 0 && (_cycles_without_lps_range % 1024) == 0)
    {
        p_range.anchorid(0xffff);
        unsigned plen = 0;
        uint8_t* pbuf = p_range.getEncodedPacket(&plen);
        Serial.write(pbuf, plen);
    }
    
    if (_serial_complete_packet)
    {
        _serial_complete_packet = false;
        instance_startmeasure();
    }

    // Restart if there are no range results after 60s
    if (_cycles_without_lps_range > 80000)
        asm volatile ("  jmp 0");
#endif

    // Run decawave code
    instance_run();

    if(instancenewrange())
    {
        ApplicationMonitor.IAmAlive();

        _cycles_without_lps_range = 0;
        if (instance_mode == TAG)
        {
            // Tag mode
            addr = instance_get_anchaddr();
            rangeinmm = instance_get_idisti();
            pwr = instance_get_rxpwr();

#ifdef LPS_DEMO             
            Serial.print(F("A"));
            Serial.print((uint32_t)(addr&0xffff), HEX);
            Serial.print(F(" "));

            Serial.print(rangeinmm, DEC);
            Serial.print(F("mm"));

            Serial.print(F(" P"));
            Serial.print(pwr);
            Serial.println(F("dbm"));
#else //  LPS_DEMO
            uint8_t null = 0;
            Serial.write(&null, 1);
            p_range.anchorid(addr&0xffff);
            p_range.distinmm(rangeinmm);
            p_range.power(pwr);
            unsigned plen = 0;
            uint8_t* pbuf = p_range.getEncodedPacket(&plen);
            Serial.write(pbuf, plen);
#endif // LPS_DEMO
        }
        else
        {
            // Anchor mode
            rangeinmm = instance_get_idisti();
            
            Serial.print(F("A"));
            uint16_t a = instance_get_newrangeancaddress();
            Serial.print(a, HEX);
            Serial.print(F("->T"));
            a = instance_get_newrangetagaddress();
            Serial.print(a, HEX);
            Serial.print(F(" "));

            Serial.print(rangeinmm, DEC);
            Serial.println(F("mm"));
        }
    }
    else
    {
        // Only read intertial sensors if it will not interfere with dwm
        int current_state = instance_getstate();
        if (_n++%2048==0 
            && (current_state == TA_SLEEP_DONE || (current_state == TA_RX_WAIT_DATA && instance_mode == ANCHOR))
            )
        {
            ApplicationMonitor.IAmAlive();
#ifdef LPS_V11_ACC
            // read raw accel/gyro measurements from device
            acceleration.getMotion(&ax, &ay, &az);
            
            if (ax!=0 && ay!=0 && az!=0)
            {
#ifdef LPS_DEMO
                // display tab-separated accel/gyro x/y/z values
                Serial.print(F("Accel: "));
                Serial.print(ax);Serial.print("\t");
                Serial.print(ay);Serial.print("\t");
                Serial.println(az);
#else // LPS_DEMO
                AccelerometerPacket ap(ax,ay,az,0x0195);
                unsigned plen = 0;
                uint8_t* pbuf = ap.getEncodedPacket(&plen);
                Serial.write(pbuf, plen);
#endif // LPS_DEMO
            }
#endif // LPS_V11_ACC
            
#ifdef LPSMINI_ACC
            // Read accelerometer and gyroscope
            uint8_t Buf[14];
            I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);

            // Create 16 bits values from 8 bits data
            // Accelerometer
            int16_t ax=-(Buf[0]<<8 | Buf[1]);
            int16_t ay=-(Buf[2]<<8 | Buf[3]);
            int16_t az=Buf[4]<<8 | Buf[5];
        
            // Gyroscope
            int16_t gx=-(Buf[8]<<8 | Buf[9]);
            int16_t gy=-(Buf[10]<<8 | Buf[11]);
            int16_t gz=Buf[12]<<8 | Buf[13];
        
            // Display values
        
            // Accelerometer
            Serial.print (ax,DEC);Serial.print ("\t");
            Serial.print (ay,DEC);Serial.print ("\t");
            Serial.print (az,DEC);Serial.print ("\t");
        
            // Gyroscope
            Serial.print (gx,DEC);Serial.print ("\t");
            Serial.print (gy,DEC);Serial.print ("\t");
            Serial.print (gz,DEC);Serial.print ("\t");

            // Read register Status 1 and only read if DRDY: Data Ready
            uint8_t ST1;
            I2Cread(MAG_ADDRESS,0x02,1,&ST1);
            if ((ST1&0x01))
            {
                // Read magnetometer data  
                I2Cread(MAG_ADDRESS,0x03,7,Buf);
                
                // Magnetometer
                mx=-(Buf[3]<<8 | Buf[2]);
                my=-(Buf[1]<<8 | Buf[0]);
                mz=-(Buf[5]<<8 | Buf[4]);

                // Schedule a new reading
                I2CwriteByte(MAG_ADDRESS,0x0A,0x01);
            }
        
            // Magnetometer
            Serial.print (mx+200,DEC);Serial.print ("\t");
            Serial.print (my-70,DEC); Serial.print ("\t");
            Serial.print (mz-700,DEC);Serial.print ("\t");

            // Barometer
            int32_t newtemperature = 0, newpression = 0;
            newtemperature = baro.getIntTemperature(MS561101BA_OSR_4096);
            if (newtemperature != 0) temperature = newtemperature;
            Serial.print(temperature/100.0);
        
            Serial.print("\t");
            newpression = baro.getIntPressure(MS561101BA_OSR_4096);
            if (newpression != 0) pression = newpression;
            Serial.print(pression/100.0);

            Serial.println(F(""));
#endif        
        }
        _cycles_without_lps_range++;
    }
}


#ifndef LPS_DEMO
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() 
{
    while (Serial.available()) 
    {
        // get the new byte:
        char inChar = (char)Serial.read(); 
        if (inChar == 0) {
            _serial_complete_packet = true;
            break;
        } 
    }
}

#endif

