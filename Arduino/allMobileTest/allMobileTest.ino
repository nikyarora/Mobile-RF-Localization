#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>

// Mobile_Node_Code.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF22 driver to control a RF22 radio.
// It is designed to work with the other example rf22_reliable_datagram_server
// Tested on Duemilanove, Uno with Sparkfun RFM22 wireless shield
// Tested on Flymaple with sparkfun RFM22 wireless shield
// Tested on ChiKit Uno32 with sparkfun RFM22 wireless shield

#define CLIENT_ADDRESS 0x02
// Singleton instance of the radio driver
//#define MAIN_SERVER_ADDRESS 0xA0
#define NODE_2_ADDRESS 0x01  // 
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
//uint8_t data[] = {CLIENT_ADDRESS};

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36

  for (int i=0; i<NUMBER_OF_NODES; i++)
  {
    data[i]=0;
  }
  
  // Flags indicating the receipt of rssi of each mobile node from each anchor.
  for (int i=0; i<NUMBER_OF_NODES-1; i++)
  {
    rssiReceiptFlags[i]=0;
  }

  if(CLIENT_ADDRESS == 0x01)
  {
    uint8_t receivedFromAll = 0;
    while(receivedFromAll == 0)
    {
      receivedFromAll = receiveSetup();   
    }
  }
  else if(CLIENT_ADDRESS == 0x02)
  {
    uint8_t recvAck = 0;
    while(recvAck == 0)
    {
       broadcast();
       recvAck = receiveAcknowledge();
       Serial.println(recvAck);
    }
  }
  else if(CLIENT_ADDRESS == 0x03)
  {
    uint8_t recvAck = 0;
    while(recvAck == 0)
    {
       broadcast();
       recvAck = receiveAcknowledge();
       //broadcast then receive
    }
  }
  else if(CLIENT_ADDRESS == 0x04)
  {
    uint8_t recvAck = 0;
    while(recvAck == 0)
    {
       broadcast();
       recvAck = receiveAcknowledge();
    }
  }
  else if(CLIENT_ADDRESS == 0x05)
  {
    uint8_t recvAck = 0;
    while(recvAck == 0)
    {
       broadcast();
       recvAck = receiveAcknowledge();
    }
  }
}

void loop()
{
  //RECEIVE
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
        Serial.println();
        Serial.println("Received Mobile Beam(s)");  

        switch (from) {
        case NODE_2_ADDRESS:
        Serial.println("Received RSSI From M2");  
        data[0] = buf[1];
        Serial.println(data[0]);
        rssiReceiptFlags [0] = 1;
        break;
        
        case NODE_3_ADDRESS:
        Serial.println("Received RSSI From M3");  
        data[1] = buf[1];
        Serial.println(data[1]);
        rssiReceiptFlags [1] = 1;
        break;
        
        case NODE_4_ADDRESS:
        Serial.println("Received RSSI From M4");  
        data[2] = buf[1];
        Serial.println(data[2]);
        rssiReceiptFlags [2] = 1;
        break;
        
        case NODE_5_ADDRESS:
        Serial.println("Received RSSI From M5");  
        data[3] = buf[1];
        Serial.println(data[3]);
        rssiReceiptFlags [3] = 1;
        break;

        case NODE_6_ADDRESS:
        Serial.println("Received RSSI From M6");  
        data[4] = buf[1];
        Serial.println(data[4]);
        rssiReceiptFlags [4] = 1;
        break;

        case NODE_7_ADDRESS:
        Serial.println("Received RSSI From M7");  
        data[5] = buf[1];
        Serial.println(data[5]);
        rssiReceiptFlags [5] = 1;
        break;

        case NODE_8_ADDRESS:
        Serial.println("Received RSSI From M8");  
        data[6] = buf[1];
        Serial.println(data[6]);
        rssiReceiptFlags [6] = 1;
        break; 
        
      }
      
      uint8_t allDataReceived=1;
      for (int i=0; i<NUMBER_OF_NODES-1; i++)
      {
        allDataReceived=allDataReceived && rssiReceiptFlags[i];
      }

      //Checks to see if all the data is received. If so, sends out a broadcast signal
      if(allDataReceived)
      {    
         broadcast();
        for (int i=0; i<NUMBER_OF_NODES-1; i++)
        {
          data[i] = 0;
          rssiReceiptFlags[i] = 0;
        }
      }
    }
  }
 }
  
  delay(1000);
}

int receiveSetup()
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
        Serial.println();
        Serial.println("Received Mobile Beam(s)"); 
        Serial.println(from); 

        switch (from) {
        case NODE_2_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_2_ADDRESS))
        {
          Serial.println("Received From M2");
          rssiReceiptFlags [0] = 1;
        }
        else
        {
          Serial.println("SendtoWait Failed");
        }
        break;
        
        case NODE_3_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_3_ADDRESS))
        {
           Serial.println("Received From M3");  
           rssiReceiptFlags [1] = 1;
        }
        break;
        
        case NODE_4_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_4_ADDRESS))
        {
          Serial.println("Received From M4");
          rssiReceiptFlags [2] = 1;
        }
        break;
        
        case NODE_5_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_5_ADDRESS))
        {
          Serial.println("Received From M5");  
          rssiReceiptFlags [3] = 1;
        }
        break;

        case NODE_6_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_5_ADDRESS))
        {
          Serial.println("Received From M6");  
          rssiReceiptFlags [4] = 1;
        }
        break;

        case NODE_7_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_7_ADDRESS))
        {
          Serial.println("Received From M7");  
          rssiReceiptFlags [5] = 1;
        }
        break;

        case NODE_8_ADDRESS:
        if (manager.sendtoWait(data, sizeof(data), NODE_8_ADDRESS))
        {
          Serial.println("Received From M8");  
          rssiReceiptFlags [6] = 1;
        }
        break; 
      }
      
      uint8_t receivedFromAll = 1;
      for (int i=0; i<NUMBER_OF_NODES-1; i++)
      {
        receivedFromAll=receivedFromAll && rssiReceiptFlags[i];
      }

      //Checks to see if all the data is received. If so, sends out a broadcast signal
      if(receivedFromAll)
      {  
        return 1;  
      }
      else
      {
        return 0;
      }
      }
    }
  }
}

int receiveAcknowledge()
{
  if(manager.available())
  {
    Serial.println("in receiveAcknowledge");
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t to;
    int8_t rssi;
    if (manager.recvfromAck(buf, &len, &from, &to))
    {
      Serial.println("message received");
        return 1;
    }
    else
    {
      Serial.println("in here");
      return 0;
    }
  }
}

void broadcast()
{  
  //BROADCAST
  Serial.println();
  Serial.println("Broadcasting ID to reachable mobile nodes.");

    //Broadcast the message to all other reachable nodes.
  if (manager.sendtoWait(data, sizeof(data), RH_BROADCAST_ADDRESS))
  {
    Serial.println("Broadcast Successful");
    Serial.print("My Address: ");
    Serial.println(CLIENT_ADDRESS);
  }
  else
    Serial.println("sendtoWait failed");
}


