// rf22_reliable_datagram_server.pde
// -*- mode: C++ -*-

#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>

// All addresses smaller than 0xFA are reserved for 
// mobile TAG nodes.and 0xFA is reserved for the main Server
// Address of the current Server:
#define SERVER_ADDRESS 0xA1
// Main server address
#define MAIN_SERVER_ADDRESS 0xA0

// Singleton instance of the radio driver
RH_RF22 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

byte data[] = {0,0};
// Dont put this on the stack:
uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];

void loop()
{
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t to;
    if (manager.recvfromAck(buf, &len, &from, &to))
    {
      if (to == RH_BROADCAST_ADDRESS){
        Serial.println("Received Mobile Beam");  
        data[0] = buf[0];
        data[1] = driver.lastRssi();
        
        if (!manager.sendtoWait(data, sizeof(data), MAIN_SERVER_ADDRESS))
          Serial.println("sendtoWait failed");
        else
          Serial.println("message succesfully sent to main server");
        
      }

      // Send a reply back to the originator client

    }
  }
}

