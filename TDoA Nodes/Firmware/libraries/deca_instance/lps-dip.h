#ifndef LPS_DIP_H
#define LPS_DIP_H

#include "lps.h"

namespace LPS
{

    uint8_t dipNibble(uint16_t v)
    {
        //   V1   V2    ad1  ad_real 
#if (defined(LPS_V11))
        if (v > 967) return 0;
        if (v > 865) return 1;
        if (v > 782) return 2;
        if (v > 715) return 3;
        if (v > 658) return 4;
        if (v > 610) return 5;
        if (v > 568) return 6;
        if (v > 532) return 7;
        if (v > 500) return 8;
        if (v > 472) return 9;
        if (v > 447) return 10;
        if (v > 424) return 11;
        if (v > 404) return 12;
        if (v > 386) return 13;
        if (v > 369) return 14;
#endif
        
#if (defined(LPSMINI_V10))
        if (v > 967) return 0;
        if (v > 865) return 1;
        if (v > 782) return 2;
        if (v > 715) return 3;
        if (v > 658) return 4;
        if (v > 610) return 5;
        if (v > 568) return 6;
        if (v > 532) return 7;
        if (v > 490) return 8;
        if (v > 465) return 9;
        if (v > 440) return 10;
        if (v > 417) return 11;
        if (v > 397) return 12;
        if (v > 378) return 13;
        if (v > 360) return 14;
#endif
        
#if (defined(LPS_V10))
        if (v > 950) return 0;
        if (v > 840) return 1;
        if (v > 760) return 2;
        if (v > 695) return 3;
        if (v > 635) return 4;
        if (v > 590) return 5;
        if (v > 548) return 6;
        if (v > 515) return 7;
        if (v > 485) return 8;
        if (v > 450) return 9;
        if (v > 430) return 10;
        if (v > 409) return 11;
        if (v > 389) return 12;
        if (v > 368) return 13;
        if (v > 353) return 14;
#endif
        return 15;
    }

    // Must be run without SPI enabled
    uint8_t readDIP()
    {
        uint8_t ret = 0x00;

        // Makde sure DWM module is in reset
        pinMode(RSTPin, OUTPUT);
        digitalWrite(RSTPin, LOW);

        analogReference(DEFAULT);    

        // Power DIP
        pinMode(DIPPOWER, OUTPUT);
        digitalWrite(DIPPOWER, DIPPOWER_DIR);

        pinMode(DIPANALOG0TO3, INPUT);
        digitalWrite(DIPANALOG0TO3, LOW);
        pinMode(DIPANALOG4TO7, INPUT);
        digitalWrite(DIPANALOG4TO7, LOW);

        // Read analog values
        delay(50);
        uint16_t lnibble = analogRead(DIPANALOG0TO3);
        uint16_t hnibble = analogRead(DIPANALOG4TO7);
        ret = LPS::dipNibble(lnibble) | (LPS::dipNibble(hnibble) << 4);

        // Release DWM from reset
        digitalWrite(RSTPin, HIGH);
#ifdef LPSMINI_v10
        pinMode(RSTPin, INPUT);
#else
        // Only allow pullup b/c we have level shifting logic
        // on lpsv1.1
        pinMode(RSTPin, INPUT_PULLUP);
#endif

        // Debug print the result of the DIP settings
#if 0
        Serial.print(F("DIP L:"));
        Serial.print(lnibble, DEC);
        Serial.print(F(", "));
        Serial.print(LPS::dipNibble(lnibble), DEC);
        Serial.print(F(" H:"));
        Serial.print(hnibble, DEC);
        Serial.print(F(", "));
        Serial.print(LPS::dipNibble(hnibble), DEC);

        Serial.print(F(":0x"));
        Serial.println(ret, HEX);
#endif
        // Turn off DIP power
        pinMode(DIPPOWER, INPUT);

        return ret;
    }

} // namespace LPS


#endif
