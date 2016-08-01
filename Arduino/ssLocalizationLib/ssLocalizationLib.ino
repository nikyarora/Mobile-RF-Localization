#include <MatrixMath.h>
#include <math.h>

#define num_nodes 2
#define xsize 3
#define ysize 3

void setup() 
{
  Serial.begin(9600);
  
  char x[1][xsize] = {14, 3, 7};
  char y[1][ysize] = {9, 2, 11};
  double cal[2][2] = {
    {1, 2},
    {3, 4}
  };
  double xi = 2.0;
  double yi = 3.0;
  double A[xsize][2];
  double B[xsize];
  double C[xsize][1];
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
    for (int j = 0; j < n; j++)
    {
      Serial.println(C[i][j]);
    }
  }
}

void loop() 
{
}

void generateMatrices(char ptrx[1][xsize], char ptry[1][ysize], double ptrcal[][num_nodes], double xi, double yi, double A[xsize][2], double B[xsize],double C[xsize][1])
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
  
  char xnew[m][1];
  char ynew[m][1];
  for (int i = 0; i < m; i++)
  {
      for(int j = 0; j < 1; j++)
      {
        xnew[m*j + i] = ptrx[n*i + j];
        ynew[m*j + i] = ptry[n*i + j];
      }
  }

  for(int i = 0; i < n; i++)
  {
    double linearM[] = {2*(xi-xnew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2)), 2*(yi-ynew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2))};
    for(int j = 0; j < m; j++)
    {
      A[i][j] = (-10.0 /ptrcal[i][0]) * linearM[j];
    }
    C[i][0] = (-ptrcal[i][1]-25.2-20 * log10(sqrt(pow(xi-xnew[i],2) + pow(yi-ynew[i],2))) / ptrcal[i][0]);
    B[i] = 1/ptrcal[i][0];
  }

 double arr[2][1] = {
    {-xi},
    {-yi}
  };

  double finalarr[m][n];
  
  for (int i = 0; i < m; i++)
  {
    for(int j = 0; j < 1; j++)
    {
      finalarr[1*i+j][0] = 0.0;
      for (int k = 0; k < n; k++)
      {
        finalarr[1*i+j][k] = finalarr[1*i+j][k] + A[n*i+k][k] * arr[1*k+j][0];
      }
    }
  }
  
  for (int i = 0; i < m; i++)
  {
     for(int j = 0; j < n; j++)
     {
       C[n*i+j][j] = C[n*i+j][j] + finalarr[n*i+j][j];
     }
  }
}



