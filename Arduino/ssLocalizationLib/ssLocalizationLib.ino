#include <MatrixMath.h>
#include <math.h>

//GENERATE MATRICES VALUES
#define xsize 3
#define ysize 3
#define NUMBER_OF_NODES 4
float xi = 0;
float yi = 2.67;
float A[xsize][2];
float B[xsize][1];
float C[xsize][1];

int m = xsize;
int n = 2;

//CALIBRATION VALUES
float x[xsize] = {-4.875,1.188,2.792};
float y[ysize] = {0, 7.917, 0};
float cal[xsize][NUMBER_OF_NODES - 1] = {
    {1.596036021269376104e-01,-7.545786433384164127e+01},
    {7.202668552816129943e-02,-5.509336898971369578e+01},
    {1.604968531492740880e-01,-7.598877203038927064e+01}
  };

void setup() 
{
  Serial.begin(9600);
  
  generateMatrices(x, y, cal, xi, yi, A, B, C);

  float data[NUMBER_OF_NODES - 1] = {242, 230, 240};
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

  //mutliply A is the array that will be multiplied by the rssiValues to get xm and ym
  float multiplyA[2][NUMBER_OF_NODES - 1];
  for(int i = 0; i < 2; i++)
  {
    for(int j = 0; j < NUMBER_OF_NODES - 1; j++)
    {
      multiplyA[i][j] = 0;
    }
  }
  float XmAndYm[2][1];

  //this sets mutliplyA with the pseudoinverse of A
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
  
  for(int i = 0; i < 2; i++)
  {
    Serial.println(XmAndYm[i][0]);
  }
}

void loop() 
{
}

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
    Serial.print("linear M: ");
    Serial.print(linearM[0]);
    Serial.print("   ");
    Serial.println(linearM[1]);
    for(int j = 0; j < n; j++)
    {
      A[i][j] = (-10.0 /ptrcal[i][0]) * linearM[j];
     // Serial.println(A[i][j]);
    }
    float num = ((-ptrcal[i][1]-25.2-20) * log10(sqrt(pow(xi-ptrx[i],2) + pow(yi-ptry[i],2))));
    C[i][0] = num / (ptrcal[i][0]);
    B[i][0] = 1/ptrcal[i][0];
    //Serial.print("B: ");
    //Serial.println(B[i][0]);
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



