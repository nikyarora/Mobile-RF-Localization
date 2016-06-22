/*
MS5611-01BA.cpp - Interfaces a Measurement Specialities MS5611-01BA with Arduino
See http://www.meas-spec.com/downloads/MS5611-01BA01.pdf for the device datasheet

Copyright (C) 2011 Fabio Varesano <fvaresano@yahoo.it>

Development of this code has been supported by the Department of Computer Science,
Universita' degli Studi di Torino, Italy within the Piemonte Project
http://www.piemonte.di.unito.it/


This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "MS561101BA.h"
#define CONVERSION_TIME 10000l // conversion time in microseconds

#include <I2cMaster.h>


MS561101BA::MS561101BA(I2cMasterBase *arg_i2c) : _i2c(arg_i2c) {
  ;
}

void MS561101BA::init(uint8_t address) {  
    _addr =  address<<1;
  
  // disable internal pullups of the ATMEGA which Wire enable by default
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
    // deactivate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    cbi(PORTC, 4);
    cbi(PORTC, 5);
  #else
    // deactivate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    cbi(PORTD, 0);
    cbi(PORTD, 1);
  #endif
 
  reset(); // reset the device to populate its internal PROM registers
  delay(1000); // some safety time
  readPROM(); // reads the PROM into object variables for later use
}

float MS561101BA::getPressure(uint8_t OSR) {
  // see datasheet page 7 for formulas
  
  uint32_t rawPress = rawPressure(OSR);
  if(rawPress == NULL) {
    return NULL;
  }
  
  int32_t dT = getDeltaTemp(OSR);
  if(dT == NULL) {
    return NULL;
  }
  
  int64_t off  = ((uint32_t)_C[1] <<16) + (((int64_t)dT * _C[3]) >> 7);
  int64_t sens = ((uint32_t)_C[0] <<15) + (((int64_t)dT * _C[2]) >> 8);
  return ((( (rawPress * sens ) >> 21) - off) >> 15) / 100.0;
}

int64_t MS561101BA::getIntPressure(uint8_t OSR) {
  // see datasheet page 7 for formulas
  
  uint32_t rawPress = rawPressure(OSR);
  if(rawPress == NULL) {
    return NULL;
  }
  
  int32_t dT = getDeltaTemp(OSR);
  if(dT == NULL) {
    return NULL;
  }
  
  int64_t off  = ((uint32_t)_C[1] <<16) + (((int64_t)dT * _C[3]) >> 7);
  int64_t sens = ((uint32_t)_C[0] <<15) + (((int64_t)dT * _C[2]) >> 8);
  return ((( (rawPress * sens ) >> 21) - off) >> 15);
}


float MS561101BA::getTemperature(uint8_t OSR) {
  // see datasheet page 7 for formulas
  int64_t dT = getDeltaTemp(OSR);
  
  if(dT != NULL) {
    return (2000 + ((dT * _C[5]) >> 23)) / 100.0;
  }
  else {
    return NULL;
  }
}

int64_t MS561101BA::getIntTemperature(uint8_t OSR) {
  // see datasheet page 7 for formulas
  int64_t dT = getDeltaTemp(OSR);
  
  if(dT != NULL) {
    return (2000 + ((dT * _C[5]) >> 23));
  }
  else {
    return NULL;
  }
}

int32_t MS561101BA::getDeltaTemp(uint8_t OSR) {
  uint32_t rawTemp = rawTemperature(OSR);
  if(rawTemp != NULL) {
    return (int32_t)(rawTemp - ((uint32_t)_C[4] << 8));
  }
  else {
    return NULL;
  }
}

//TODO: avoid duplicated code between rawPressure and rawTemperature methods
//TODO: possible race condition between readings.. serious headache doing this.. help appreciated!

uint32_t MS561101BA::rawPressure(uint8_t OSR) {
  unsigned long now = micros();
  if(lastPresConv != 0 && (now - lastPresConv) >= CONVERSION_TIME) {
    lastPresConv = 0;
    pressCache = getConversion(MS561101BA_D1 + OSR);
  }
  else {
    if(lastPresConv == 0 && lastTempConv == 0) {
      startConversion(MS561101BA_D1 + OSR);
      lastPresConv = now;
    }
    return pressCache;
  }
}

uint32_t MS561101BA::rawTemperature(uint8_t OSR) {
  unsigned long now = micros();
  if(lastTempConv != 0 && (now - lastTempConv) >= CONVERSION_TIME) {
    lastTempConv = 0;
    tempCache = getConversion(MS561101BA_D2 + OSR);
  }
  else {
    if(lastTempConv == 0 && lastPresConv == 0) { // no conversions in progress
      startConversion(MS561101BA_D2 + OSR);
      lastTempConv = now;
    }
  }
  return tempCache;
}


// see page 11 of the datasheet
void MS561101BA::startConversion(uint8_t command) {
    // initialize pressure conversion
    _i2c->start(_addr | I2C_WRITE);
    _i2c->write(command);
    _i2c->stop();
}

uint32_t MS561101BA::getConversion(uint8_t command) {
    union {uint32_t val; uint8_t raw[4]; } conversion = {0};
  
    // start read sequence
    _i2c->start(_addr | I2C_WRITE);
    _i2c->write(0x00);
    _i2c->stop();
  
    _i2c->start(_addr | I2C_READ);
    conversion.raw[2] = _i2c->read(false);
    conversion.raw[1] = _i2c->read(false);
    conversion.raw[0] = _i2c->read(true);
    _i2c->stop();
  
    return conversion.val;
}


/**
 * Reads factory calibration and store it into object variables.
*/
int MS561101BA::readPROM() {
  for (int i=0;i<MS561101BA_PROM_REG_COUNT;i++) {
    bool r = _i2c->start(_addr | I2C_WRITE);
    r = _i2c->write(MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE));
    _i2c->stop();

    r = _i2c->start(_addr | I2C_READ);
    _C[i] = _i2c->read(false) << 8 | _i2c->read(true);
    _i2c->stop();
  }
  return 0;
}


/**
 * Send a reset command to the device. With the reset command the device
 * populates its internal registers with the values read from the PROM.
*/
void MS561101BA::reset() {
    _i2c->start(_addr | I2C_WRITE);
    _i2c->write(MS561101BA_RESET);
    _i2c->stop();
}



