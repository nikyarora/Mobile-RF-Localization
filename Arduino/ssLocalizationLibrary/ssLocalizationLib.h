#ifndef ssLocalizationLib_h
#define ssLocalizationLib_h

#include "Arduino.h"

class ssLocalizationLib
{
  public:
    ssLocalizationLib();
    void generateMatrices();
    void add();
    void subtract();
    void multiply();
  private:
};

#endif