#include <RHReliableDatagram.h>
#include <RH_RF22.h>
#include <SPI.h>
#include <Time.h>
#include <MatrixMath.h>
#include <math.h>

// Mobile_Node_Code.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF22 driver to control a RF22 radio.
// It is designed to work with the other example rf22_reliable_datagram_server
// Tested on Duemilanove, Uno with Sparkfun RFM22 wireless shield
// Tested on Flymaple with sparkfun RFM22 wireless shield
// Tested on ChiKit Uno32 with sparkfun RFM22 wireless shield

#define NODE_1_ADDRESS 0x01
#define NODE_2_ADDRESS 0x02
#define NODE_3_ADDRESS 0x03
#define NODE_4_ADDRESS 0x04
#define NODE_5_ADDRESS 0x05
#define NODE_6_ADDRESS 0x06
#define NODE_7_ADDRESS 0x07
#define NODE_8_ADDRESS 0x08
#define NUMBER_OF_NODES 4
RH_RF22 driver;

//This is the address of THIS node  
#define CLIENT_ADDRESS NODE_4_ADDRESS

//GENERATE MATRICES VALUES
#define xsize 3
#define ysize 3
float xi = 2.0;
float yi = 3.0;
float A[xsize][2];
float B[xsize][1];
float C[xsize][1];

int m = xsize;
int n = 2;

//CALIBRATION VALUES
float x[xsize] = {14, 5, 7};
float y[ysize] = {9, 12, 11};
float cal[xsize][NUMBER_OF_NODES - 1] = {
    {1, 2, 3},
    {3, 4, 5},
    {5, 6, 7}
  };

//Tells node whether it has received from all the other nodes
uint8_t myTurnToBroadcast = 0;

uint8_t data[NUMBER_OF_NODES+10];
uint8_t rssiReceiptFlags[NUMBER_OF_NODES-1];
uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

/**
 * This is the setup. The nodes are all setup here and then exit to the loop.
 */
void setup() 
{
  Serial.begin(9600);
  if (!manager.init())
    //Serial.println("init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36

  //Indicates the RSSI values from each node.
  for (int i=0; i<NUMBER_OF_NODES; i++)
  {
    data[i]=0;
  }
  
  // Flags indicating the receipt of rssi of each mobile node from each anchor.
  for (int i=0; i<NUMBER_OF_NODES-1; i++)
  {
    rssiReceiptFlags[i]=0;
  }

  //If this is the first node, broadcast and wait for an acknowledgment
  if(CLIENT_ADDRESS == 0x01)
  {
    uint8_t receiveSuccessful = 0;
    while(receiveSuccessful == 0)
    {
       //delay(1500);
       receiveSuccessful = receiveSetup();
    }

    generateMatrices(x, y, cal, xi, yi, A, B, C);

    for (int i=0; i<NUMBER_OF_NODES-1; i++)
    {
      rssiReceiptFlags[i] = 0;
    } 

    myTurnToBroadcast = 1;
    broadcast();
  }
  
  //if not the first node, wait for a broadcast and then acknowledge
  else
  {
    uint8_t broadcastSuccessful = 0;
    while(broadcastSuccessful == 0)
    {
      broadcastSuccessful = receiveAcknowledge();
    }

    //fills the rssiReceiptFlags for nodes that must broadcast before receiving from all
    if(CLIENT_ADDRESS != NUMBER_OF_NODES)
    {
      for(int i = CLIENT_ADDRESS - 1; i < NUMBER_OF_NODES - 1; i++)
      {
        rssiReceiptFlags[i] = 1;
      }
    }
  }
}

/**
 * This is the loop. Each node waits to receive from all the other nodes and
 * then exits the loop to broadcast.
 */
void loop()
{
  //Serial.read();
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
        data[0] = buf[0];
        
        switch (from) {
        case NODE_1_ADDRESS:
        //Serial.println("Received RSSI From M1");
        data[1] = driver.lastRssi();
        rssiReceiptFlags [0] = 1;
        if(CLIENT_ADDRESS == NODE_2_ADDRESS)
        {
          myTurnToBroadcast = 1;
        }
        break;
        
        case NODE_2_ADDRESS: 
        //Serial.println("Received RSSI From M2");
        data[2] = driver.lastRssi();
        rssiReceiptFlags [1] = 1;
        if(CLIENT_ADDRESS == NODE_3_ADDRESS)
        {
          myTurnToBroadcast = 1;
        }
        break;
      
        case NODE_3_ADDRESS:
        //Serial.println("Received RSSI From M3");
        data[3] = driver.lastRssi();
        rssiReceiptFlags [2] = 1;
        if(CLIENT_ADDRESS == NODE_4_ADDRESS)
        {
          myTurnToBroadcast = 1;
        }
        break;
        
        case NODE_4_ADDRESS:  
        //Serial.println("Received RSSI From M4");
        data[3] = driver.lastRssi();
        rssiReceiptFlags [2] = 1;
        if(CLIENT_ADDRESS == NODE_1_ADDRESS)
        {
          myTurnToBroadcast = 1;
        }
        break;
        
        /**case NODE_5_ADDRESS:
        Serial.println("Received RSSI From M5");  
        data[4] = buf[1];
        Serial.println(data[4]);
        rssiReceiptFlags [4] = 1;
        break;

        case NODE_6_ADDRESS:
        Serial.println("Received RSSI From M6");  
        data[5] = buf[1];
        Serial.println(data[5]);
        rssiReceiptFlags [5] = 1;
        break;

        case NODE_7_ADDRESS:
        Serial.println("Received RSSI From M7");  
        data[6] = buf[1];
        Serial.println(data[6]);
        rssiReceiptFlags [6] = 1;
        break;

        case NODE_8_ADDRESS:
        Serial.println("Received RSSI From M8");  
        data[7] = buf[1];
        Serial.println(data[7]);
        rssiReceiptFlags [7] = 1;
        break; **/
        
      }

      //Check if all the data is received
      uint8_t allDataReceived=1;
      for (int i=0; i<NUMBER_OF_NODES-1; i++)
      {
        allDataReceived=allDataReceived && rssiReceiptFlags[i];
      }

      //Generate Matrices Stuff
      if(CLIENT_ADDRESS == 0x01)
      {
        if(myTurnToBroadcast)
        {
          generateMatrices(x, y, cal, xi, yi, A, B, C);
          float rssiValues[xsize][1];
          for(int i = 0; i < NUMBER_OF_NODES - 1; i++)
          {
            rssiValues[i][0] = data[i];
          }

          //subtract B and C from the RSSI values
          for(int i = 0; i < m; i++)
          {
            rssiValues[i][0] = rssiValues[i][0] - B[i][0] - C[i][0];
          }

          float multiplyA[2][NUMBER_OF_NODES - 1];
          float XmAndYm[2][1];
          inverse(A, multiplyA);

          //multiply the inverse of A by rssiValues
          for(int i = 0; i < n; i++)
          {
            for(int j = 0; j < 1; j++)
            {
              XmAndYm[i][j]=0;
              for(int k = 0; k < m; k++)
              {
                XmAndYm[i][j]= XmAndYm[i][j] + (multiplyA[i][k] * rssiValues[k][j]);
              }
            }
          }
        }
      }

      //Checks to see if all the data is received. If so, sends out a broadcast signal
      if(allDataReceived)
      {   
        String stringData;
        for (int i=0; i < NUMBER_OF_NODES; i++)
        {
          int dataValue = data[i];
          stringData = stringData + data[i] + ",";
          rssiReceiptFlags[i] = 0;
        } 
        stringData = stringData + '\n';
        Serial.print(stringData);
        //Serial.print("my turn to broadcast");
        //Serial.print(myTurnToBroadcast);
      }
      if(myTurnToBroadcast == 1)
      {
        broadcast();
        //Serial.println("broadcasted");
      }
    }
  }
 }
}

/*****************************************************************BROADCAST********************************************************************/

/************************************************************************************************************
 * This is the broadcasting method for the loop.
 * It sends one broadcast and then exits the function.
 * There is a delay at the beginning so the other nodes have time to get into the receive
 * and don't miss the broadcast.
 ************************************************************************************************************/
void broadcast()
{
  //BROADCAST
  delay(750);
  myTurnToBroadcast = 0;
  //Serial.println();
  //Serial.println("Broadcasting ID to all receiving nodes.");
  //Broadcast the message to all other reachable nodes.    
  for(int i = 0; i < 5; i++)
  {
    if (manager.sendtoWait(data, sizeof(data), RH_BROADCAST_ADDRESS))
    {
      //Serial.println("Broadcast Successful");
      //Serial.print("My Address: ");
      //Serial.println(CLIENT_ADDRESS);
    }
    else
   {
     Serial.println("sendtoWait failed");  
   }  
  }
}


/*****************************************************************SETUP FUNCTIONS********************************************************************/
 
/*************************************************************
 * This is the method that is called by Node 1 in the setup
 * The first node receives from each active broadcasting node
 * and sends an acknowledgement.
 *************************************************************/
int receiveSetup()
{
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t sendtoWait = 0;
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t to;
    int8_t rssi;
    if (manager.recvfromAck(buf, &len, &from, &to))
    {
        switch (from) 
        {
          case NODE_2_ADDRESS:
          //Serial.println("Received From M2");
          rssiReceiptFlags [0] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_2_ADDRESS))
            {
              //Serial.println("2 success");
              sendtoWait = 1;
            }
            else
            {
              //Serial.println("2 not success");
              sendtoWait = 0;
            }
          }
          break;
          
          case NODE_3_ADDRESS:
          //Serial.println("Received From M3");  
          rssiReceiptFlags [1] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_3_ADDRESS))
            {
              //Serial.println("3 success");
              sendtoWait = 1;
            }
            else
            {
              //Serial.println("3 not success");
              sendtoWait = 0;
            }
          }
          break;
          
          case NODE_4_ADDRESS:
          //Serial.println("Received From M4");
          rssiReceiptFlags [2] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_4_ADDRESS))
            {
              //Serial.println("4 success");
              sendtoWait = 1;
            }
            else
            {
              //Serial.println("4 not success");
              sendtoWait = 0;
            }
          }
          break;
          
          /**case NODE_5_ADDRESS:
          Serial.println("Received From M5");  
          rssiReceiptFlags [3] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_5_ADDRESS))
            {
              Serial.println("5 success");
              sendtoWait = 1;
            }
            else
            {
              Serial.println("5 keep going");
              sendtoWait = 0;
            }
          }
          break;
  
          case NODE_6_ADDRESS:
          Serial.println("Received From M6");  
          rssiReceiptFlags [4] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_6_ADDRESS))
            {
              Serial.println("6 success");
              sendtoWait = 1;
            }
            else
            {
              Serial.println("6 keep going");
              sendtoWait = 0;
            }
          }
          break;
  
          case NODE_7_ADDRESS:
          Serial.println("Received From M7");  
          rssiReceiptFlags [5] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_7_ADDRESS))
            {
              Serial.println("7 success");
              sendtoWait = 1;
            }
            else
            {
              Serial.println("7 keep going");
              sendtoWait = 0;
            }
          }
          break;
  
          case NODE_8_ADDRESS:
          Serial.println("Received From M8");  
          rssiReceiptFlags [6] = 1;
          while(sendtoWait == 0)
          {
            if(manager.sendtoWait(data, sizeof(data), NODE_8_ADDRESS))
            {
              Serial.println("8 success");
              sendtoWait = 1;
            }
            else
            {
              Serial.println("8 keep going");
              sendtoWait = 0;
            }
          } 
          break;**/
        }
      }
      
      uint8_t allDataReceived=1;
      for (int i=0; i<NUMBER_OF_NODES-1; i++)
      {
        allDataReceived=allDataReceived && rssiReceiptFlags[i];
      }

      //Checks to see if all the data is received. If so, sends out a broadcast signal
      if(allDataReceived)
      {    
        return 1;
      }
      else
      {
         return 0;
      }
   }
}

/**************************************************************
 * This is the method called by all the nodes in the setup
 * to broadcast and then receive an acknowledgement.
 *************************************************************/
int receiveAcknowledge()
{
  //BROADCAST
  //Serial.println();
  //Serial.println("Broadcasting ID to NODE 1.");
  uint8_t waitToReceive = 0;

  //Broadcast the message to all other reachable nodes.
  if (manager.sendtoWait(data, sizeof(data), NODE_1_ADDRESS))
  {
    delay(1000);
    Serial.println("Broadcast Successful");
    while(waitToReceive == 0) 
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
          //Serial.println("RECEIVED AN ACKNOWLEDGEMENT");
          waitToReceive = 1;
          return 1;
        }
        else
        {
         // Serial.println("DID NOT RECEIVE AN ACKNOWLEGEMENT");
          waitToReceive = 0;
        }
     }
    }
  }
  else
   // Serial.println("sendtoWait failed");
    return 0;
}

/*****************************************************GENERATE MATRICES and OTHER MATRIX FUNCTIONS*********************************************************/

/**************************************************
 * This function ...
 *************************************************/
void generateMatrices(float ptrx[xsize], float ptry[ysize], float ptrcal[xsize][NUMBER_OF_NODES - 1], float xi, float yi, float A[xsize][2], float B[xsize][1],float C[xsize][1])
{
  int m = xsize;
  int n = 2;
  for(int i = 0; i < m; i++)
  {
    for(int j = 0; j < n; j++)
    {
      A[i][j] = 0.0;
    }

    B[i][0] = 0.0;
    C[i][0] = 0.0;
  }

  for(int i = 0; i < m; i++)
  {
    float linearM[] = {2*(xi-ptrx[i])/(pow(xi-ptrx[i],2)+pow(yi-ptry[i],2)), 2*(yi-ptry[i])/(pow(xi-ptrx[i],2)+pow(yi-ptry[i],2))};
    for(int j = 0; j < n; j++)
    {
      A[i][j] = (-10.0 /ptrcal[i][0]) * linearM[j];
    }
    C[i][0] = (-ptrcal[i][1]-25.2-20 * log10(sqrt(pow(xi-ptrx[i],2) + pow(yi-ptry[i],2))) / ptrcal[i][0]);
    B[i][0] = 1/ptrcal[i][0];
  }

 float arr[2][1] = {
    {-xi},
    {-yi}
  };

  float finalarr[m][1];
  
  for (int i = 0; i < m; i++)
  {
    finalarr[i][0] = 0.0;
    for (int k = 0; k < n; k++)
    {
      finalarr[i][0] = finalarr[i][0] + (A[i][k] * arr[k][0]);
    }
  }
  
  for (int i = 0; i < m; i++)
  {
    C[i][0] = C[i][0] + finalarr[i][0];
  }
  
}

/**************************************
 * Calculate the pseudoinverse of A
 *************************************/
void inverse(float A[][2], float multiplyA[2][NUMBER_OF_NODES - 1])
{
  //TRANSPOSE A
  float transposedA[n][m];
  for(int i = 0; i < m; i++)
  {
    for(int j = 0; j < n; j++)
    {
      transposedA[j][i] = A[i][j];
    }
  }

  //MULTIPLY A TRANSPOSE BY A
  float AByATranspose[2][2];
  for (int i = 0; i < n; i++)//2 rows in A transpose
  {
    for(int j = 0; j < n; j++)//3 columns in A transpose and 3 rows in A 
    {
      AByATranspose[i][j]=0;
      for(int k = 0; k < m; k++)//2 columns in A
      {
        AByATranspose[i][j]= AByATranspose[i][j] + (transposedA[i][k]*A[k][j]);
      }
    }
  }

    //INVERESE OF AByATranspose
    int pivrow;     // keeps track of current pivot row
    int k,i,j;      // k: overall index along diagonal; i: row index; j: col index
    int pivrows[n]; // keeps track of rows swaps to undo at end
    float tmp;      // used for finding max value and making column swaps
 
    for (k = 0; k < n; k++)
    {
        // find pivot row, the row with biggest entry in current column
        tmp = 0;
        for (i = k; i < n; i++)
        {
            if (abs(AByATranspose[i][k]) >= tmp)   // 'Avoid using other functions inside abs()?'
            {
                tmp = abs(AByATranspose[i][k]);
                pivrow = i;
            }
        }
 
        // check for singular matrix
        if (AByATranspose[pivrow][k] == 0.0f)
        {
            Serial.println("Inversion failed due to singular matrix");
        }
 
        // Execute pivot (row swap) if needed
        if (pivrow != k)
        {
            // swap row k with pivrow
            for (j = 0; j < n; j++)
            {
                tmp = AByATranspose[k][j];
                AByATranspose[k][j] = AByATranspose[pivrow][j];
                AByATranspose[pivrow][j] = tmp;
            }
        }
        pivrows[k] = pivrow;    // record row swap (even if no swap happened)
 
        tmp = 1.0f/AByATranspose[k][k];    // invert pivot element
        AByATranspose[k][k] = 1.0f;        // This element of input matrix becomes result matrix
 
        // Perform row reduction (divide every element by pivot)
        for (j = 0; j < n; j++)
        {
            AByATranspose[k][j] = AByATranspose[k][j]*tmp;
        }
 
        // Now eliminate all other entries in this column
        for (i = 0; i < n; i++)
        {
            if (i != k)
            {
                tmp = AByATranspose[i][k];
                AByATranspose[i][k] = 0.0f;  // The other place where in matrix becomes result mat
                for (j = 0; j < n; j++)
                {
                    AByATranspose[i][j] = AByATranspose[i][j] - AByATranspose[k][j]*tmp;
                }
            }
        }
    }
 
    // Done, now need to undo pivot row swaps by doing column swaps in reverse order
    for (k = n-1; k >= 0; k--)
    {
        if (pivrows[k] != k)
        {
            for (i = 0; i < n; i++)
            {
                tmp = AByATranspose[i][k];
                AByATranspose[i][k] = AByATranspose[i][pivrows[k]];
                AByATranspose[i][pivrows[k]] = tmp;
            }
        }
    }

  //MUTLIPLY A TRANSPOSE BY A INVERSE
  //A[3][2] --> multiplyA[2][3]
  //transposedA[2][3]
  //AByATranspose[2][2]
  for (int i = 0; i < n; i++)//rows in AByATranspose
  {
    for(int j = 0; j < m; j++)//rows in transposedA and columns in AByATranspose
    {
      multiplyA[i][j]=0;
      for(int k = 0; k < n; k++)//columns in transposedA
      {
        multiplyA[i][j]= multiplyA[i][j] + (AByATranspose[i][k] * transposedA[k][j]);  
      }
    }
  }
}

