#include "Arduino.h"
#include "ssLocalizationLib.h"

/*Morse::Morse(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}*/

void ssLocalizationLib::generateMatrices(float ptrx[xsize], float ptry[ysize], float ptrcal[xsize][num_nodes], float xi, float yi, float A[xsize][num_nodes], float B[xsize][1],float C[xsize][1])
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
  
  C = add(C, finalarr, m, addArray);
}

float ssLocalizationLib::add(float arr1, float arr2, int m, float finalarr)
{
  for (int i = 0; i < m; i++)
  {
    finalarr[i][0] = arr1[i][0] + arr2[i][0];
  }  

  return finalarr;
}

void ssLocalizationLib::subtract(float arr1, float arr2, int m, float finalarr)
{
  for (int i = 0; i < m; i++)
  {
    finalarr[i][0] = arr1[i][0] - arr2[i][0];
  }  

  return finalarr;
}

void ssLocalizationLib::multiply()
{
}

