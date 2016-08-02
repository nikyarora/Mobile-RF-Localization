#ifndef ssLocalizationLib_h
#define ssLocalizationLib_h

#include "Arduino.h"

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