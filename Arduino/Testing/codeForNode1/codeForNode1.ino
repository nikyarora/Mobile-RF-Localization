
#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>

#define CLIENT_ADDRESS 0x01
// Singleton instance of the radio driver
//#define MAIN_SERVER_ADDRESS 0xA0
#define NODE_2_ADDRESS 0x02  // 
#define NODE_3_ADDRESS 0x03  // 
#define NODE_4_ADDRESS 0x04  // 
#define NODE_5_ADDRESS 0x05  // 
#define NODE_6_ADDRESS 0x06  // 
#define NODE_7_ADDRESS 0x07  // 
#define NODE_8_ADDRESS 0x08  // 
#define NUMBER_OF_NODES 2
RH_RF22 driver;

uint8_t data[NUMBER_OF_NODES+10];
uint8_t rssiReceiptFlags[NUMBER_OF_NODES-1];
uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];

RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
   Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
}

void loop() 
{
  //BROADCAST
  Serial.println();
  Serial.println("Broadcasting ID to NODE 2.");
  uint8_t waitToReceive = 0;

    //Broadcast the message to all other reachable nodes.
  if (manager.sendtoWait(data, sizeof(data), NODE_2_ADDRESS))
  {
    delay(1000);
    Serial.println("Broadcast Successful");
    Serial.print("My Address: ");
    Serial.println(CLIENT_ADDRESS);
    while(waitToReceive == 0)
    {
     if (manager.available())
     {
        Serial.println("manager is available");
        // Wait for a message addressed to us from the client
        uint8_t len = sizeof(buf);
        uint8_t from;
        uint8_t to;
        int8_t rssi;
        if (manager.recvfromAck(buf, &len, &from, &to))
        {
          Serial.println(from);
          Serial.println("RECEIVED AN ACKNOWLEDGEMENT");
          waitToReceive = 1;
        }
        else
        {
          Serial.println("DID NOT RECEIVE AN ACKNOWLEGEMENT");
          waitToReceive = 0;
        }
     }
    }
  }
  else
    Serial.println("sendtoWait failed");

  delay(1000);
}

