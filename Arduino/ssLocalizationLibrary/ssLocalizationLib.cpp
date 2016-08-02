#include "Arduino.h"
#include "ssLocalizationLib.h"

void ssLocalizationLib::generateMatrices(float ptrx[xsize], float ptry[ysize], float ptrcal[xsize][NUMBER_OF_NODES], float xi, float yi, float A[xsize][NUMBER_OF_NODES], float B[xsize][1],float C[xsize][1])
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

  float multiplyarr[m][1];

  multiply(A, arr, m, n, multiplyarr);

  float finalarr[m][1];

  add(finalarr, multiplyarr, m, finalarr);
  
  add(C, finalarr, m, C);
}

void ssLocalizationLib::add(float **arr1[][1], float **arr2[][1], int m, float **finalarr[][1])
{
  for (int i = 0; i < m; i++)
  {
    finalarr[i][0] = arr1[i][0] + arr2[i][0];
  }  
}

void ssLocalizationLib::subtract(float **arr1[][1], float **arr2[][1], int m, float **finalarr[][1])
{
  for (int i = 0; i < m; i++)
  {
    finalarr[i][0] = arr1[i][0] - arr2[i][0];
  }  
}

void ssLocalizationLib::multiply(float **arr1[][2], float **arr2[][1], int m, int n, float **multiplyarr[][1])
{
  for (int i = 0; i < m; i++)
  {
    multiplyarr[i][0] = 0.0;
    for (int k = 0; k < n; k++)
    {
      multiplyarr[i][0] = multiplyarr[i][0] + (arr1[i][k] * arr2[k][0]);
    }
  }
}

