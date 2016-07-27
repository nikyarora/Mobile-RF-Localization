#include <MatrixMath.h>
#include <math.h>

void setup() 
{
}

void loop() 
{
}

int generateMatrices(int m, char *ptr, char *ptr2, char **ptr3cal, double xi, double yi)
{
  int n = 2;
  float A[m][n];
  float B[m];
  float C[m];
  for(int i = 0; i < m; i++)
  {
    for(int j = 0; j < n; j++)
    {
      A[i][j] = 0;
    }

    B[i] = 0;
    C[i] = 0;
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

  for(int i = 0; i < n; i++)
  {
    double linearM[] = {2*(xi-xnew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2)), 2*(yi-ynew[i])/(pow(xi-xnew[i],2)+pow(yi-ynew[i],2))};
    //A[i];//do something
    C[i] = (-ptr3cal[i][1]-25.2-20 * log(sqrt(pow(xi-xnew[i],2) + (pow(yi-ynew[i],2))),10)) / ptr3cal[i][0];
    B[i] = 1/ptr3cal[i][0];
  }

 // C = C; // + something

  return 0;
}



