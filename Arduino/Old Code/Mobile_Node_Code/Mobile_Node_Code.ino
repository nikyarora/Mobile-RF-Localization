// Mobile_Node_Code.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF22 driver to control a RF22 radio.
// It is designed to work with the other example rf22_reliable_datagram_server
// Tested on Duemilanove, Uno with Sparkfun RFM22 wireless shield
// Tested on Flymaple with sparkfun RFM22 wireless shield
// Tested on ChiKit Uno32 with sparkfun RFM22 wireless shield

#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>

#define CLIENT_ADDRESS 0x01
// Singleton instance of the radio driver
RH_RF22 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

// Data to broadcast to all anchor nodes. This currently represents the mobile node ID.
uint8_t data[] = {CLIENT_ADDRESS};

void loop()
{
  Serial.println("Broadcasting ID to all other reachable anchor nodes.");
    //Broadcast the message to all other reachable nodes.
  if (manager.sendtoWait(data, sizeof(data), RH_BROADCAST_ADDRESS))
  {
    Serial.println("Broadcast Successful");
  }
  else
    Serial.println("sendtoWait failed");
  delay(1000);
}

