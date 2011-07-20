//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>


// Blablalbalbalbalbal

int listdir(const char *path) {
  struct dirent *entry;
  DIR *dp;
  
  dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    return -1;
  }
  
  while((entry = readdir(dp)))
    puts(entry->d_name);
  
  closedir(dp);
  return 0;
}



bool is_CSV_file (const char * fileName) {
  char *strptr = strstr(fileName, ".csv");
  //printf("testin filename %s\n", fileName);
  if (strptr == NULL) {
    //printf("False");
    return false;
  }
  else {
    //printf("True");
    return true;
  }
}


void extractXYZdataFromFile (const char * fileName, double * xData, double * yData, double * zData, const int rows, const int offset) {
  
  double x;
  double y;
  double z;
  
  FILE *file = fopen(fileName, "rt");
  for (int i = 0; i < offset; ++i) {
   //
    // Move the file seeker forward
   fscanf(file, "%lf, %lf, %lf", &x, &y, &z);
//    //printf("%.15e\n", x);
//    xData[i] = x;
//    yData[i] = y;
//    zData[i] = z;
  }
  
  
  for (int i = 0; i < (rows - offset); ++i) {
    
    fscanf(file, "%lf, %lf, %lf", &x, &y, &z);
    //printf("%.15e\n", x);
    xData[i] = x;
    yData[i] = y;
    zData[i] = z;
    
  }
  
  
  fclose(file);
}




void calcCF (const double * data1, const double * data2,
             double * cf, const int rows) {

  int tau = 0;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < (rows - tau); ++j) {
      cf[i] = cf[i] + data1[j + tau] * data2[j];
    }
    
    // Calculting the average for this tau
    cf[i] = cf[i] / ((double)(rows - tau));
    
    // Increment the correlation step 'tau'.
    ++tau;
  }
}



void calc_all_CF_measure (const double * xData, const double * yData, const double * zData,
                          double * cfXX_temp, double * cfXY_temp, double * cfXZ_temp,
                          double * cfYX_temp, double * cfYY_temp, double * cfYZ_temp,
                          double * cfZX_temp, double * cfZY_temp, double * cfZZ_temp,
                          const int rows) {
  
  for (int i = 0; i < rows; ++i) {
    cfXX_temp[i] = 0.0; cfXY_temp[i] = 0.0; cfXZ_temp[i] = 0.0;
    cfYX_temp[i] = 0.0; cfYY_temp[i] = 0.0; cfYZ_temp[i] = 0.0;
    cfZX_temp[i] = 0.0; cfZY_temp[i] = 0.0; cfZZ_temp[i] = 0.0;
  }
  
  calcCF(xData, xData, cfXX_temp, rows); calcCF(xData, yData, cfXY_temp, rows); calcCF(xData, zData, cfXZ_temp, rows);
  calcCF(yData, xData, cfYX_temp, rows); calcCF(yData, yData, cfYY_temp, rows); calcCF(yData, zData, cfYZ_temp, rows);
  calcCF(zData, xData, cfZX_temp, rows); calcCF(zData, yData, cfZY_temp, rows); calcCF(zData, zData, cfZZ_temp, rows);
}






// usage: ./VACF inputdirectory numOfRows offset





int main(int argc, char **argv) {
  
  const int tmp1 = chdir (argv[1]);
  if (tmp1 == -1) {
    perror("Error! Could not change the current working directory to the given path:");
    exit(EXIT_FAILURE);
  }
  
  const int rows = atoi (argv[2]);
  
  const int offset = atoi (argv[3]);
  
  
  double *xData = malloc((rows - offset) * sizeof(double));
  double *yData = malloc((rows - offset) * sizeof(double));
  double *zData = malloc((rows - offset) * sizeof(double));
  
  // Allocation of memory to hold the different correlation fucntions
  // for the 3x3 correlation tensor.
  double *cfXX = malloc((rows - offset) * sizeof(double));
  double *cfXY = malloc((rows - offset) * sizeof(double));
  double *cfXZ = malloc((rows - offset) * sizeof(double));
    
  double *cfYX = malloc((rows - offset) * sizeof(double));
  double *cfYY = malloc((rows - offset) * sizeof(double));
  double *cfYZ = malloc((rows - offset) * sizeof(double));
  
  
  double *cfZX = malloc((rows - offset) * sizeof(double));
  double *cfZY = malloc((rows - offset) * sizeof(double));
  double *cfZZ = malloc((rows - offset) * sizeof(double));
  
  
  double *cfXX_temp = malloc((rows - offset) * sizeof(double));
  double *cfXY_temp = malloc((rows - offset) * sizeof(double));
  double *cfXZ_temp = malloc((rows - offset) * sizeof(double));
  
  double *cfYX_temp = malloc((rows - offset) * sizeof(double));
  double *cfYY_temp = malloc((rows - offset) * sizeof(double));
  double *cfYZ_temp = malloc((rows - offset) * sizeof(double));
  
  double *cfZX_temp = malloc((rows - offset) * sizeof(double));
  double *cfZY_temp = malloc((rows - offset) * sizeof(double));
  double *cfZZ_temp = malloc((rows - offset) * sizeof(double));
  
  
  
  int files = 0;
 

  for (int i = 0; i < (rows - offset); ++i) {
    cfXX[i] = 0.0; cfXY[i] = 0.0; cfXZ[i] = 0.0;
    cfYX[i] = 0.0; cfYY[i] = 0.0; cfYZ[i] = 0.0;
    cfZX[i] = 0.0; cfZY[i] = 0.0; cfZZ[i] = 0.0;
  }

  
  
  printf("Current working directory is changed to: %s\n", argv[1]);
  
  DIR           *cwd;
  struct dirent *entry;
  cwd = opendir(".");
  
  if (cwd)
  { 
    while ( (entry = readdir(cwd)) != NULL )
      {
        bool test = is_CSV_file (entry->d_name);
        if (test == true)
          {
            printf("Filename %s is a csv file\n",entry->d_name);
            extractXYZdataFromFile (entry->d_name, xData, yData, zData, rows, offset);
        
            calc_all_CF_measure(xData, yData, zData,
                                cfXX_temp, cfXY_temp, cfXZ_temp,
                                cfYX_temp, cfYY_temp, cfYZ_temp,
                                cfZX_temp, cfZY_temp, cfZZ_temp, rows - offset);
            
        
        
            for (int i = 0; i < (rows - offset); ++i) {
              cfXX[i] += cfXX_temp[i]; cfXY[i] += cfXY_temp[i]; cfXZ[i] += cfXZ_temp[i];
              cfYX[i] += cfYX_temp[i]; cfYY[i] += cfYY_temp[i]; cfYZ[i] += cfYZ_temp[i];
              cfZX[i] += cfZX_temp[i]; cfZY[i] += cfZY_temp[i]; cfZZ[i] += cfZZ_temp[i];
        }
        
        ++files;
      }
  }
  closedir(cwd);
  }
  
  // Averaging over the number of measurements, i.e. the number of .csv files in
  // the directory.
  for (int i = 0; i < (rows - offset); ++i) {
    cfXX[i] /= ((double) files); cfXY[i] /= ((double) files); cfXZ[i] /= ((double) files);
    cfYX[i] /= ((double) files); cfYY[i] /= ((double) files); cfYZ[i] /= ((double) files);
    cfZX[i] /= ((double) files); cfZY[i] /= ((double) files); cfZZ[i] /= ((double) files);
  }
  
  mkdir("VACF", S_IRWXU);
  FILE * cfXX_File = fopen ("VACF/cfXX.csv", "w"); FILE * cfXY_File = fopen ("VACF/cfXY.csv", "w"); FILE * cfXZ_File = fopen ("VACF/cfXZ.csv", "w");
  FILE * cfYX_File = fopen ("VACF/cfYX.csv", "w"); FILE * cfYY_File = fopen ("VACF/cfYY.csv", "w"); FILE * cfYZ_File = fopen ("VACF/cfYZ.csv", "w");
  FILE * cfZX_File = fopen ("VACF/cfZX.csv", "w"); FILE * cfZY_File = fopen ("VACF/cfZY.csv", "w"); FILE * cfZZ_File = fopen ("VACF/cfZZ.csv", "w");
  
  for (int i = 0; i < (rows - offset); ++i) {
    fprintf(cfXX_File, "%.15e\n", cfXX[i]); fprintf(cfXY_File, "%.15e\n", cfXY[i]); fprintf(cfXZ_File, "%.15e\n", cfXZ[i]);
    fprintf(cfYX_File, "%.15e\n", cfYX[i]); fprintf(cfYY_File, "%.15e\n", cfYY[i]); fprintf(cfYZ_File, "%.15e\n", cfYZ[i]);
    fprintf(cfZX_File, "%.15e\n", cfZX[i]); fprintf(cfZY_File, "%.15e\n", cfZY[i]); fprintf(cfZZ_File, "%.15e\n", cfZZ[i]);
  }
  
  fclose(cfXX_File); fclose(cfXY_File); fclose(cfXZ_File);
  fclose(cfYX_File); fclose(cfYY_File); fclose(cfYZ_File);
  fclose(cfZX_File); fclose(cfZY_File); fclose(cfZZ_File);
  
  return 0;
}
  
  
 // 
//  
//  FILE *f = fopen(argv[1], "rt");
//  
//  
//
//int main(void)  {
//  DIR           *d;
//  struct dirent *dir;
//  d = opendir(".");
//  if (d)
//  {
//    while ((dir = readdir(d)) != NULL)
//    {
//      printf("%s\n", dir->d_name);
//    }
//    
//    closedir(d);
//  }
//  
//  return(0);
//}
//




  
  
//  
//  int counter = 1;
//  
//  if (argc == 1)
//    listdir(".");
//  
//  while (++counter <= argc) {
//    printf("\nListing %s...\n", argv[counter-1]);
//    listdir(argv[counter-1]);
//  }
//  
//  return 0;
//}
//

//int main (int argc, const char * argv[]) {
//  
//  if (argc < 2)
//  {
//    fprintf(stderr,"Usage: %s csv_file\n",argv[0]);
//    return(1);
//  }
//
//  FILE *f = fopen(argv[1], "rt");
//  char Line[256];
//  unsigned int AllocSize = 0, Size = 0, n;
//  char *L_text;
//  
//    printf("Hello, World!\n");
//  
//  
//  
//  
//  
//  
//  
//  
//  
//    return 0;
//}
//
//
//
//
//
//
//int main(int argc, char* argv[])
//{
//    
//  FILE *f = fopen(argv[1], "rt");
//  char Line[256];
//  unsigned int AllocSize = 0, Size = 0, n;
//  char *L_text;
//  
//  while(fgets(Line, sizeof(Line), f))
//  {
//    
//    printf("Item = %s \n",strtok(Line, ", "));
//    printf("Class = %s \n",strtok(NULL, ", "));
//    printf("Supplier = %s \n",strtok(NULL, ", "));
//    
//  }
//  return(0);
//}
//</string.h></stdlib.h></stdio.h>
