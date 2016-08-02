#ifndef ssLocalizationLib_h
#define ssLocalizationLib_h

#include "Arduino.h"

#define NUMBER_OF_NODES 4
#define xsize 3
#define ysize 3
float xi = 2.0;
float yi = 3.0;
float A[xsize][2];
float B[xsize][1];
float C[xsize][1];

float x[xsize] = {14, 5, 7};
float y[ysize] = {9, 12, 11};
float cal[xsize][NUMBER_OF_NODES - 1] = {
    {1, 2},
    {3, 4},
    {5, 6}
  };

class ssLocalizationLib
{
  public:
    //ssLocalizationLib();
    void generateMatrices(float ptrx[xsize], float ptry[ysize], float ptrcal[xsize][num_nodes], float xi, float yi, float A[xsize][num_nodes], float B[xsize][1],float C[xsize][1]);
    void add(float arr1[m][1], float arr2[m][1], int m, float finalarr[m][1]);
    void subtract(float arr[m][1], float arr2[m][1], int m, float finalarr[m][1]);
    void multiply(float arr1[m][n], float arr2[n][1], int m, int n, float finalarr[m][1]);
  //private:
};

#endif