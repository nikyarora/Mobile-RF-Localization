// rf22_reliable_datagram_server.pde
// -*- mode: C++ -*-

#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>

// All addresses smaller than 0xFA are reserved for 
// mobile TAG nodes.and 0xFA is reserved for the main Server
// Address of the current Server:
// Server address
#define MAIN_SERVER_ADDRESS 0xA0  // This is the main server.
#define NODE_2_ADDRESS 0xA1  // 
#define NODE_3_ADDRESS 0xA2  // 
#define NODE_4_ADDRESS 0xA3  // 
#define NODE_5_ADDRESS 0xA4  // 
#define NODE_6_ADDRESS 0xA5  // 
#define NODE_7_ADDRESS 0xA6  // 
#define NODE_8_ADDRESS 0xA7  // 
#define NUMBER_OF_NODES 3

// Singleton instance of the radio driver
RH_RF22 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, MAIN_SERVER_ADDRESS);
uint8_t data[NUMBER_OF_NODES+10];
uint8_t rssiReceiptFlags[NUMBER_OF_NODES-1];
void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36

  // Data to be sent to the main computer for trilateration
  // The format is: Mobile Node address, Anchor 1 RSSI (Main), Anchor 2 RSSI, Anchor 3 RSSI) 

  for (int i=0; i<NUMBER_OF_NODES; i++)
  {
    data[i]=0;
  }
  
  // Flags indicating the receipt of rssi of each mobile node from each anchor.

  for (int i=0; i<NUMBER_OF_NODES-1; i++)
  {
    rssiReceiptFlags[i]=0;
  }
}


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
    int8_t rssi;
    if (manager.recvfromAck(buf, &len, &from, &to))
    {
      
      if (to == RH_BROADCAST_ADDRESS)
      {
        //Serial.println("Received Mobile Beam");  
        data[0] = buf[0];
        data[1] = driver.lastRssi();//(int8_t)(-120 + ((driver.spiRead(RH_RF22_REG_26_RSSI) / 2)));
        //Serial.println(data[1],DEC);
        rssiReceiptFlags[0] = 1;
        
      }
      else 
      {
        switch (from) {
        case NODE_2_ADDRESS:
        //Serial.println("Received RSSI From A1");  
        data[2] = buf[1];
        rssiReceiptFlags [1] = 1;
        break;
        
        case NODE_3_ADDRESS:
        //Serial.println("Received RSSI From A2");  
        data[3] = buf[1];
        rssiReceiptFlags [2] = 1;
        break;
        
        case NODE_4_ADDRESS:
        //Serial.println("Received RSSI From A3");  
        data[4] = buf[1];
        rssiReceiptFlags [3] = 1;
        break;

        
        case NODE_5_ADDRESS:
        //Serial.println("Received RSSI From A4");  
        data[5] = buf[1];
        rssiReceiptFlags [4] = 1;
        break;


        case NODE_6_ADDRESS:
        //Serial.println("Received RSSI From A5");  
        data[6] = buf[1];
        rssiReceiptFlags [5] = 1;
        break;

        case NODE_7_ADDRESS:
        //Serial.println("Received RSSI From A6");  
        data[7] = buf[1];
        rssiReceiptFlags [6] = 1;
        break;

        case NODE_8_ADDRESS:
        //Serial.println("Received RSSI From A7");  
        data[8] = buf[1];
        rssiReceiptFlags [7] = 1;
        break;   
        
        }
      }

      uint8_t allDataReceived=1;
      for (int i=0; i<NUMBER_OF_NODES-1; i++){
        allDataReceived=allDataReceived && rssiReceiptFlags[i];
        }
      if(allDataReceived){  
      // Send a reply back to the originator client
        for (int i=0; i<NUMBER_OF_NODES-1; i++){
        rssiReceiptFlags[i] = 0;
        Serial.write(data,NUMBER_OF_NODES);
        }
      }
    }
  }
}

