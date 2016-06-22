/*! ----------------------------------------------------------------------------
 * @file	deca_spi.c
 * @brief	SPI access functions
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include <SPI.h>
#include "deca_spi.h"
#include "deca_device_api.h"

#ifdef __AVR_ATmega328P__
const int deca_chipSelectPin = 9;
#endif

#ifdef __AVR_ATmega2560__
const int deca_chipSelectPin = 9;
#endif

#ifdef __AVR_ATmega32U4__
const int deca_chipSelectPin = 9;
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: openspi()
 *
 * Low level abstract function to open and initialise access to the SPI device.
 * returns 0 for success, or -1 for error
 */
int openspi( )
{
    SPI.begin();
    spi_setSpeedFast();
	return 0;
}

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: closespi()
 *
 * Low level abstract function to close the the SPI device.
 * returns 0 for success, or -1 for error
 */
int closespi(void)
{
    SPI.end();
	return 0;
} // end closespi()

/*! ------------------------------------------------------------------------------------------------------------------
 * Function: writetospi()
 *
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 */
#pragma GCC optimize ("O3")
int writetospi
(
    uint16       headerLength,
    const uint8 *headerBuffer,
    uint32       bodylength,
    const uint8 *bodyBuffer
)
{
    uint8_t b;
    //pinMode(deca_chipSelectPin, OUTPUT);
    digitalWrite(deca_chipSelectPin, LOW);

    for (uint16 i=0;i<headerLength;i++)
    {
        b = SPI.transfer(headerBuffer[i]);
    }

    for (uint16 i=0;i<bodylength;i++)
        b = SPI.transfer(bodyBuffer[i]);

    digitalWrite(deca_chipSelectPin, HIGH);
    //pinMode(deca_chipSelectPin, INPUT_PULLUP);
    return 0;
} // end writetospi()


/*! ------------------------------------------------------------------------------------------------------------------
 * Function: readfromspi()
 *
 * Low level abstract function to read from the SPI
 * Takes two separate byte buffers for write header and read data
 * returns the offset into read buffer where first byte of read data may be found,
 * or returns -1 if there was an error
 */
#pragma GCC optimize ("O3")
int readfromspi
(
    uint16       headerLength,
    const uint8 *headerBuffer,
    uint32       readlength,
    uint8       *readBuffer
)
{
    uint8_t b;
    //pinMode(deca_chipSelectPin, OUTPUT);
    digitalWrite(deca_chipSelectPin, LOW);
    uint32 n=0;
    for (uint16 i=0;i<headerLength;i++)
    {
        SPI.transfer(headerBuffer[i]);
    }

    for (uint16 i=0;i<readlength;i++)
        readBuffer[n++] = SPI.transfer(0x00);

    //pinMode(deca_chipSelectPin, INPUT_PULLUP);
    digitalWrite(deca_chipSelectPin, HIGH);

    return 0;//headerLength;
} // end readfromspi()


int spi_setSpeedFast()
{
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    return 0;
}

int spi_setSpeedSlow()
{
    SPI.setClockDivider(SPI_CLOCK_DIV32);
    return 0;
}

uint32_t spi_getTickCount()
{
    return (uint32_t)millis();
}

void spi_enableInterrupts()
{
    interrupts();
}

void spi_disableInterrupts()
{
    noInterrupts();
}



#ifdef __cplusplus
}
#endif
