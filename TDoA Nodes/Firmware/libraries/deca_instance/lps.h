#ifndef _LPS_H
#define _LPS_H

#include <deca_device_api.h>
#include <deca_instance.h>
#include <deca_spi.h>

// pins used for the connection with the sensor
// the other you need are controlled by the SPI library):
#ifdef __AVR_ATmega328P__
  const int IRQPin = 2;
  #define IRQPort PORTD
  const int RSTPin = 4;

  #ifdef LPS_V11
    const int DIPPOWER = 3;
    #define DIPPOWER_DIR LOW
  #endif

  #ifdef LPSMINI_V10
    const int DIPPOWER = 3;
    #define DIPPOWER_DIR HIGH

    #define    MPU9250_ADDRESS            0x69
    #define    MAG_ADDRESS                0x0C
    #define    BAROMETER_ADDRESS          0x77
 
    #define    GYRO_FULL_SCALE_250_DPS    0x00  
    #define    GYRO_FULL_SCALE_500_DPS    0x08
    #define    GYRO_FULL_SCALE_1000_DPS   0x10
    #define    GYRO_FULL_SCALE_2000_DPS   0x18
 
    #define    ACC_FULL_SCALE_2_G        0x00  
    #define    ACC_FULL_SCALE_4_G        0x08
    #define    ACC_FULL_SCALE_8_G        0x10
    #define    ACC_FULL_SCALE_16_G       0x18
  #endif

  #ifdef LPS_V10
    const int DIPPOWER = 11; // MOSI
    #define DIPPOWER_DIR LOW
  #endif

  #ifdef LPS_V09
    const int DIPPOWER = 11; // Just dummy mapping
    #define DIPPOWER_DIR LOW
  #endif

  const int DIPANALOG0TO3 = A6;
  const int DIPANALOG4TO7 = A7;
  const int deca_chipSelectPin = 9;
  const int ACC_INT     = 3;
#endif

#ifdef __AVR_ATmega32U4__
  const int IRQPin = 3;
  #define IRQPort PORTD
  const int RSTPin = 4;
  const int DIPPOWER = 11; // MOSI
  const int DIPNPOWER = 3;
  const int DIPANALOG0TO3 = 6;
  const int DIPANALOG4TO7 = 7;
  const int deca_chipSelectPin = 9;
#endif


namespace LPS
{
    void reset_DW1000()
    {
        // Reset the decawave chip
        pinMode(RSTPin, OUTPUT);
        digitalWrite(RSTPin, LOW);
        delay(1);
        digitalWrite(RSTPin, HIGH);
#ifdef LPSMINI_v10
        pinMode(RSTPin, INPUT);
#else
        // Only allow pullup b/c we have level shifting logic
        // on lpsv1.1
        pinMode(RSTPin, INPUT_PULLUP);
#endif
    }

    uint32 initDWRanging(int instance_mode, uint8 instance_addr)
    {
        uint32 devID ;
        instanceConfig_t instConfig;
        int result;

        openspi();
        spi_setSpeedSlow();

        // reset the DW1000
        LPS::reset_DW1000();

        devID = instancereaddeviceid() ;
        if(DWT_DEVICE_ID != devID) 
        {
            // Failed to read Decaid
            return(-1) ;
        }

        result = instance_init() ;
        if (0 > result) return(-1) ; // Some failure has occurred

        spi_setSpeedFast();

        instancesetrole(instance_mode) ;     // Set this instance role
        instance_init_s(instance_mode);

        // Channel and parameter settings
        instConfig.channelNumber = 5;
        instConfig.preambleCode = 3;
        instConfig.pulseRepFreq = DWT_PRF_16M;
        instConfig.pacSize = DWT_PAC32;
        instConfig.nsSFD = 1;
#ifndef LPS_FAST_RANGING
        instConfig.sfdTO = (1025 + 64 - 32);
        instConfig.dataRate = DWT_BR_110K;
        instConfig.preambleLen = DWT_PLEN_1024;
#else
        instConfig.sfdTO = (256 + 64 - 32);
        instConfig.dataRate = DWT_BR_6M8;
        instConfig.preambleLen = DWT_PLEN_256;
#endif

        // Set operating channel etc
        instance_config_nootp(&instConfig) ;                  

        //instancesetantennadelays(514.9067);
        instancesetantennadelays_i(0x4042); // Equals 514.9067

        // Setup initial Payload configuration
        instanceAddressConfig_t ipc ;
        ipc.forwardToFRAddress = 0xDECA020000000001;
        ipc.tagAddress = 0xDECA010000000000 | instance_addr;
        ipc.anchorAddress = 0xDECA020000000000;
        ipc.anchorAddress |= instance_addr;
        ipc.anchorListSize = ANCHOR_LIST_SIZE ;

        ipc.sendReport = 1 ;  //1 => anchor sends TOF report to tag

        // The poll mask decides which of the anchors the tag polls to start ranging with.
        // The fewer number of anchors a tag has to scan in each cycle the faster the cycles
        // A mask of 0xff will poll anchors 0-7, 0xf polls anchors 0-3 and 0xf0 anchors 4-7. 

        //ipc.anchorPollMask = (uint64_t)0xffff; // anchor poll mask - polling anchors 0-19
        ipc.anchorPollMask = (uint64_t)0x3ff;    // anchor poll mask - polling anchors 0-9

        instancesetaddresses(&ipc);

        // The sleep delays is how long the tag will sleep inbetween ranging cycles. 
        // This is normally set to around 50-100ms for the LPS and 200+ for LPSmini
#ifdef LPS_DEMO
        // Note that a sleep time that is too short (<50?) will cause LPS to hang, most 
        // likely because it's alarm goes off before it get's properly tucked into sleep. 
        // (the real reason why has not been investigated entirely yet)
        // The blink delay is not really relevant in the current mode but kept here for legacy.
    #if defined(LPSMINI_V10)
        instancesettagsleepdelay(3*POLL_SLEEP_DELAY, 3*BLINK_SLEEP_DELAY); //set the Tag sleep time
    #else
        instancesettagsleepdelay(POLL_SLEEP_DELAY, BLINK_SLEEP_DELAY); //set the Tag sleep time
    #endif

#else
        instancesettagsleepdelay(0, 0); //set the Tag sleep time
#endif

        //this is for ARM to ARM tag/anchor (using normal response times 10ms)
        instancesetblinkreplydelay(FIXED_REPLY_DELAY);

        //set the default response delays
        instancesetreplydelay(FIXED_REPLY_DELAY, 0);

        return devID;
    }

    void process_deca_irq(void)
    {
        uint8_t pinvalue=bitRead(IRQPort, IRQPin);
        do{
            instance_process_irq(0);
            pinvalue=bitRead(IRQPort, IRQPin);
        }
#ifdef LPS_V11
        while(pinvalue == 0); //while IRS line active 
#else
        while(pinvalue == 1); //while IRS line active 
#endif
    }

    void initInterrupt()
    {
        // Setup IRQ pin
#ifdef LPS_V11
        digitalWrite(IRQPin, HIGH);
        pinMode(IRQPin, INPUT_PULLUP);
        delay(50);
        attachInterrupt(0, LPS::process_deca_irq, FALLING);
#else
        digitalWrite(IRQPin, LOW);
        pinMode(IRQPin, INPUT);
        delay(50);
        attachInterrupt(0, LPS::process_deca_irq, RISING);
#endif
    }
    
} // namespace LPS

#endif
