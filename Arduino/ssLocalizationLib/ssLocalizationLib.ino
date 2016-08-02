#include <MatrixMath.h>
#include <math.h>

#define num_nodes 2
#define xsize 3
#define ysize 3

void setup() 
{
  Serial.begin(9600);
  
  float x[xsize] = {14, 5, 7};
  float y[ysize] = {9, 12, 11};
  float cal[xsize][num_nodes] = {
    {1, 2},
    {3, 4},
    {5, 6}
  };
  float xi = 2.0;
  float yi = 3.0;
  float A[xsize][num_nodes];
  float B[xsize];
  float C[xsize][1];
  generateMatrices(x, y, cal, xi, yi, A, B, C);

  Serial.println();
  int n = num_nodes;
  Serial.println("A");
  for (int i = 0; i < xsize; i++)
  {
    for (int j = 0; j < n; j++)
    {
      Serial.println(A[i][j]);
    }
  }

  Serial.println();
  Serial.println("B");
  for (int i = 0; i < xsize; i++)
  {
    Serial.println(B[i]);
  }

  Serial.println();
  Serial.println("C");
  for (int i = 0; i < xsize; i++)
  {
    Serial.println(C[i][0]);
  }
}

void loop() 
{
}

void generateMatrices(float ptrx[xsize], float ptry[ysize], float ptrcal[xsize][num_nodes], float xi, float yi, float A[xsize][num_nodes], float B[xsize],float C[xsize][1])
{
  int m = xsize;
  int n = 2;
  for(int i = 0; i < m; i++)
  {
    for(int j = 0; j < n; j++)
    {
      A[i][j] = 0.0;
    }

    B[i] = 0.0;
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
    B[i] = 1/ptrcal[i][0];
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
      Serial.println(finalarr[i][0]);
    }
  }
  
  for (int i = 0; i < m; i++)
  {
    C[i][0] = C[i][0] + finalarr[i][0];
  }
}



