/*
 * this program is used to calculate the dct value of a 4x4 matrix.
 * It is from the JM software. Three kinds of function forward4x4 is listed
 * here and the main reason is to demenstrate the difference between 2-dim
 * array and 2-dim pointer.
 */
#ifdef _DCT
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define M 16    
#define BLOCK_SIZE 4
static int tmp[M];
void forward4x4(int (*block)[M], int (*tblock)[M], int pos_y, int pos_x) {
  int i, ii;  
  int *pTmp = tmp, *pblock;
  static int p0,p1,p2,p3;
  static int t0,t1,t2,t3;
  // Horizontal
  for (i=pos_y; i < pos_y + BLOCK_SIZE; i++) {
    pblock = &block[i][pos_x];
    p0 = *(pblock++);
    p1 = *(pblock++);
    p2 = *(pblock++);
    p3 = *(pblock  );
    t0 = p0 + p3;
    t1 = p1 + p2;
    t2 = p1 - p2;
    t3 = p0 - p3;
    *(pTmp++) =  t0 + t1;
    *(pTmp++) = (t3 << 1) + t2;
    *(pTmp++) =  t0 - t1;    
    *(pTmp++) =  t3 - (t2 << 1);
  }
  // Vertical 
  for (i=0; i < BLOCK_SIZE; i++) {
    pTmp = tmp + i;
    p0 = *pTmp;
    p1 = *(pTmp += BLOCK_SIZE);
    p2 = *(pTmp += BLOCK_SIZE);
    p3 = *(pTmp += BLOCK_SIZE);
    t0 = p0 + p3;
    t1 = p1 + p2;
    t2 = p1 - p2;
    t3 = p0 - p3;
    ii = pos_x + i;
    tblock[pos_y    ][ii] = t0 +  t1;
    tblock[pos_y + 1][ii] = t2 + (t3 << 1);
    tblock[pos_y + 2][ii] = t0 -  t1;
    tblock[pos_y + 3][ii] = t3 - (t2 << 1);
  }
}
void print_arr(int arr[][M],int n) {
    int j,i;
    for(j=0;j<n;j++) {
        for(i=0;i<M;i++)
            printf("%8d",arr[j][i]);
        printf("\n");
    }
}
#if 1
int main()
{
   int orig[M][M];
   int dct[M][M];
   int j,i;
//   srand((int)time(NULL));
   for(j=0;j<M;j++)
       for(i=0;i<M;i++)
           orig[j][i]=rand()%100;
   print_arr(orig,M);
   for(j=0;j<M;j+=4) {
       for(i=0;i<M;i+=4)
           forward4x4(orig,dct,j,i);
   }
   print_arr(dct,M);


}
#endif

#if 0
#define BLOCK_SIZE 4
static int tmp[16];
void forward4x4(int **block, int **tblock, int pos_y, int pos_x) {
  int i, ii;  
  int *pTmp = tmp, *pblock;
  static int p0,p1,p2,p3;
  static int t0,t1,t2,t3;
  // Horizontal
  for (i=pos_y; i < pos_y + BLOCK_SIZE; i++) {
    pblock = &block[i][pos_x];
    p0 = *(pblock++);
    p1 = *(pblock++);
    p2 = *(pblock++);
    p3 = *(pblock  );
    t0 = p0 + p3;
    t1 = p1 + p2;
    t2 = p1 - p2;
    t3 = p0 - p3;
    *(pTmp++) =  t0 + t1;
    *(pTmp++) = (t3 << 1) + t2;
    *(pTmp++) =  t0 - t1;    
    *(pTmp++) =  t3 - (t2 << 1);
  }
  // Vertical 
  for (i=0; i < BLOCK_SIZE; i++) {
    pTmp = tmp + i;
    p0 = *pTmp;
    p1 = *(pTmp += BLOCK_SIZE);
    p2 = *(pTmp += BLOCK_SIZE);
    p3 = *(pTmp += BLOCK_SIZE);
    t0 = p0 + p3;
    t1 = p1 + p2;
    t2 = p1 - p2;
    t3 = p0 - p3;
    ii = pos_x + i;
    tblock[pos_y    ][ii] = t0 +  t1;
    tblock[pos_y + 1][ii] = t2 + (t3 << 1);
    tblock[pos_y + 2][ii] = t0 -  t1;
    tblock[pos_y + 3][ii] = t3 - (t2 << 1);
  }
}
void print_arr(int **arr,int n,int m)
{
    int j,i;
    for(j=0;j<n;j++) {
        for(i=0;i<m;i++)
            printf("%8d",arr[j][i]);
        printf("\n");
    }
}
int main()
{
   int **orig;
   int **dct;
   int j,i;
//   srand((int)time(NULL));
orig=malloc(sizeof(int*)*M);
dct=malloc(sizeof(int*)*M);
for(j=0;j<M;j++)
{
    orig[j]=malloc(sizeof(int)*M);
    dct[j]=malloc(sizeof(int)*M);
}
   for(j=0;j<M;j++)
       for(i=0;i<M;i++)
           orig[j][i]=rand()%100;
   print_arr(orig,M,M);
   for(j=0;j<M;j+=4)
   {
       for(i=0;i<M;i+=4)
           forward4x4(orig,dct,j,i);
   }
   print_arr(dct,M,M);
   for(j=0;j<M;j++)
{
    free(orig[j]);
    free(dct[j]);
}
free(orig);
free(dct);
}
#endif


#if 0
void forward4x4(int (*block)[M], int (*tblock)[M], int pos_y, int pos_x)
{
  int i, ii;  
  int *pTmp = tmp, *pblock;
  static int p0,p1,p2,p3;
  static int t0,t1,t2,t3;
  printf("ok");
  // Horizontal
  for (i=pos_y; i < pos_y + BLOCK_SIZE; i++)
  {
    pblock = &block[i][pos_x];
    p0 = *(pblock++);
    p1 = *(pblock++);
    p2 = *(pblock++);
    p3 = *(pblock  );
    t0 = p0 + p3;
    t1 = p1 + p2;
    t2 = p1 - p2;
    t3 = p0 - p3;
    *(pTmp++) =  t0 + t1;
    *(pTmp++) = (t3 << 1) + t2;
    *(pTmp++) =  t0 - t1;    
    *(pTmp++) =  t3 - (t2 << 1);
  }
  // Vertical 
  for (i=0; i < BLOCK_SIZE; i++)
  {
    pTmp = tmp + i;
    p0 = *pTmp;
    p1 = *(pTmp += BLOCK_SIZE);
    p2 = *(pTmp += BLOCK_SIZE);
    p3 = *(pTmp += BLOCK_SIZE);
    t0 = p0 + p3;
    t1 = p1 + p2;
    t2 = p1 - p2;
    t3 = p0 - p3;
    ii = pos_x + i;
    tblock[pos_y    ][ii] = t0 +  t1;
    tblock[pos_y + 1][ii] = t2 + (t3 << 1);
    tblock[pos_y + 2][ii] = t0 -  t1;
    tblock[pos_y + 3][ii] = t3 - (t2 << 1);
  }
}
void print_arr(int **arr,int n,int m)
{
    int j,i;
    for(j=0;j<n;j++)
    {
        for(i=0;i<m;i++)
            printf("%8d",arr[j][i]);
        printf("\n");
    }
}
int main()
{
   int **orig;
   int **dct;
   int j,i;
//   srand((int)time(NULL));
orig=malloc(sizeof(int*)*M);
dct=malloc(sizeof(int*)*M);
for(j=0;j<M;j++)
{
    orig[j]=malloc(sizeof(int)*M);
    dct[j]=malloc(sizeof(int)*M);
}
   for(j=0;j<M;j++)
       for(i=0;i<M;i++)
           orig[j][i]=rand()%100;
   print_arr(orig,M,M);
   printf("pk\n");
   for(j=0;j<M;j+=4)
   {
       for(i=0;i<M;i+=4)
           forward4x4(orig,dct,j,i);
   }
   print_arr(dct,M,M);
   for(j=0;j<M;j++)
{
    free(orig[j]);
    free(dct[j]);
}
free(orig);
free(dct);
}
#endif
#endif