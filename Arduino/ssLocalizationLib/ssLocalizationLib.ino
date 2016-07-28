#include <MatrixMath.h>
#include <math.h>

#define num_nodes 2; 

void setup() 
{
}

void loop() 
{
}

void generateMatrices(int m, char *ptr, char *ptr2, char **ptr3cal, double xi, double yi, char A[num_nodes][2],char B[num_nodes][1],char C[num_nodes][1])
{
  int m = num_nodes;
  int n = 2;
 // double A[m][n];
  //double B[m];
  //double C[m][1];
  for(int i = 0; i < m; i++)
  {
    for(int j = 0; j < n; j++)
    {
      A[i][j] = 0.0;
    }

    B[i] = 0.0;
    C[i][0] = 0.0;
  }
  
  char xnew[n];
  char ynew[n];
  for (int i = 0; i < m; i++)
  {
      for(int j = 0; j < n; j++)
      {
        xnew[m*j + i] = ptr[n*i + j];
        ynew[m*j + i] = ptr2[n*i + j];
      }
  }

  for(int i = 0; i < m; i++)
  {
    double linearM[] = {2*(xi-xnew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2)), 2*(yi-ynew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2))};
    for(int j = 0; j < n; j++)
    {
      A[i][j] = -10.0 /ptr3cal[i][0]* linearM[i];
    }
    C[i][0] = (-ptr3cal[i][1]-25.2-20 * log10(sqrt(pow(xi-xnew[i],2) + pow(yi-ynew[i],2))) / ptr3cal[i][0]);
    B[i] = 1/ptr3cal[i][0];
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

  double Cnew[m][n];

  for (int i = 0; i < m; i++)
  {
     for(int j = 0; j < n; j++)
     {
       Cnew[n*i+j][j] = C[n*i+j][j] + finalarr[n*i+j][j];
     }
  }
}



