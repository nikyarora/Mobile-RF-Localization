ILPS Demo firmware
=================
Loligo LPS example Arduino source code based on the Decawave two way ranging algorithm for the EVK.
All boards communicate at 115200bps (8-n-1).


Directory structure
===================
/libraries    - Dependancies and main libraries
/lps-main     - Main file for the LPS v1.1 board
/lpsmini-main - Main sources for LPSMini v1.0
/hexfiles     - Working builds of current source and bootloader
  --/lps_v11_demo_<date released>.hex         - LPS v1.0 Two way ranging demo firmware
  --/lpsmini_v10_demo_<date released>.hex     - LPSMini v1.0 Two way ranging demo firmware
  --/lpsmini_v10_imudmp_<date released>.hex   - LPSMini v1.0 IMU demo using mpu9250 internal Digital motion processor
  --/lps_v11_optiboot_<date released>.hex     - LPS Bootloader (Optiboot)
  --/lpsmini_v10_optiboot_<date released>.hex - LPSMini Bootloader (Optiboot)

Add the contents of the libraries folder to your arduino library path. 


DIP Switch on LPS
=================
In this demo software the dip is used both to select between TAG / ANCHOR mode and the address of the chip:

DIP 1-4 | Address of the board. 
DIP 5-7 | Unused
DIP 8   | ON - TAG | OFF - ANCHOR


A note on update rate
=====================
A number of things affect the update rate. In order to get LPSMini more stable the poll rate was lowered from previous releases. 
This can easily be changed in libraries/deca_instance/lps.h on around line 160, search for 'instancesettagsleepdelay'. Lower the 
values of the sleep delays to increase the update rate. Either you can simply change the functions to run as:

instancesettagsleepdelay(50,50); 

or change the defined values in the Decawave instance header: deca_instance.h, look for 'POLL_SLEEP_DELAY'. 
Note that setting these values too low will cause the LPS to hang. Play around with them. 
Values around 50-100 should work well for LPS, and 200-300 for LPSmini. 
When not using the intertial measurements these values can be lower giving you a faster update rate. 

The second thing that will have an even greater impact depending on how many anchors you have is the poll mask. 
The poll mask decides which of the anchors the tag polls to start ranging with. The fewer number of anchors a tag 
has to scan in each cycle the faster the cycles. A mask of 0xff will poll anchors 0-7, 0xf polls anchors 0-3 and 0xf0 anchors 4-7. 
You find this in the same lps.h around line 150. If you only have 4 anchors set this value to 0x00f; That is, the 
first 4 bits are ones and make sure your anchors are configured as address 0-3.

Further, by default the LPS use 110k transfer rate to get long range (up to 300+m has been achieved in unobstructed areas). If
you change this to use the 6.8m transfer rate your ranging will be faster but the maximum range will be shorter. There is experimental
support for this by adding the #define LPS_FAST_RANGING to the top of your lps-main.ino or lpsmini-main.ino. Note that you have to
reprogram all your tags and anchors to use the same datarate. (NOTE-TO-SELF:this should be placed on a DIP setting)

Inertial Measurements
=====================
If you are not using the inertial measurements you can disable them. Look at the top of your lps-main.ino (or lpsmini-main.ino) 
for the #define LPS_V11_ACC or #define LPSMINI_ACC. Commenting these lines out will disable the inertial measurements and will
reduce the memory footprint. For example: 

Change:
#define LPS_V11_ACC

to

// #define LPS_V11_ACC

to disable the accelerometer for the LPS (not mini). Note that the current release has the mini intertial measurements disabled.
You can enable them by uncommenting the "#define LPS_V11_ACC" line. 


Programming the bootloader using avrdude
========================================
For LPS:
avrdude  -c stk500v2 -p atmega328p -P usb -b 115200 -e -u -U lock:w:0x3f:m -U efuse:w:0x05:m -U hfuse:w:0xDE:m -U lfuse:w:0xFF:m \
         -U flash:w:hexfiles/lps_v11_optiboot_<date released>.hex -U lock:w:0x2f:m

For LPSmini:
avrdude  -c stk500v2 -p atmega328p -P usb -b 115200 -e -u -U lock:w:0x3f:m -U efuse:w:0x05:m -U hfuse:w:0xDE:m -U lfuse:w:0xFF:m \
         -U flash:w:hexfiles/lpsmini_v10_optiboot_<date released>.hex -U lock:w:0x2f:m


Compiling and programming using the arduino suite
=================================================
Select the following board / processor combination depending on what board you are using:
  LPS:     Board=Arduino Nano, Processor=ATmega328
  LPSMini: Board=Arduino Pro or Pro Mini, Processor=ATmega328 (3.3V, 8MHz)
Make sure you set the correct serial port. Note that when using the serial monitor you need to select
115200 as your baudrate. 


Debugging
=========
Add the following variables to your preferences.txt: 
build.verbose=true
upload.verbose=true
(you'll find it by opening File/Preferences in the arduino suite, at the bottom: "More preferences can be edited directly in the file...")
I you have issues make sure to turn these on, copy the text output to a textfile and include this in your email for help. 
