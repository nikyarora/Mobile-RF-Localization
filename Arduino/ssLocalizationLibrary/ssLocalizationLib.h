#ifndef ssLocalizationLib_h
#define ssLocalizationLib_h

#include "Arduino.h"

#define NUMBER_OF_NODES 4
#define xsize NUMBER_OF_NODES - 1
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

 int m = NUMBER_OF_NODES - 1;
 int n = 2;

class ssLocalizationLib
{
  public:
    void generateMatrices(float ptrx[xsize], float ptry[ysize], float ptrcal[xsize][NUMBER_OF_NODES], float xi, float yi, float A[xsize][NUMBER_OF_NODES], float B[xsize][1],float C[xsize][1]);
    void add(float *arr1[][1], float *arr2[][1], int m, float *finalarr[][1]);
    void subtract(float *arr1[][1], float *arr2[][1], int m, float *finalarr[][1]);
    void multiply(float *arr1[][2], float *arr2[][1], int m, int n, float *multiplyarr[][1]);
};

#endif