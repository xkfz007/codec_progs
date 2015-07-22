/*
 * This program is used to calculate the fourier transform of an original image (format like yuv). 
 * Two methods are used here: 
 * * calft1 is the basic * method that calculate the value according to the formulate (2-dim),
 * * calft2 is a faster method which converts the 2-dim calculation to two 1-dim calculation
 * The results are stored in the files:real1.tex image1.txt real2.txt image2.txt
 * 
 */
#ifdef _FOURIER
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#if WIN32
#include <time.h>
#else
#include <unistd.h>
#include <sys/timeb.h>
#endif

#define PI 3.1415926
typedef unsigned char byte;
void memalloc_byte(byte***src,int row,int col) {
    int i;
    *src=(byte**)calloc(row,sizeof(byte*));
    for(i=0;i<row;i++)
        (*src)[i]=(byte*)calloc(col,sizeof(byte));

}
void freemem_byte(byte**src,int row) {
    int i;
    for(i=0;i<row;i++)
        free(src[i]);
    free(src);
}
void memalloc_double(double***src,int row,int col) {
    int i;
    *src=(double**)calloc(row,sizeof(double*));
    for(i=0;i<row;i++)
        (*src)[i]=(double*)calloc(col,sizeof(double));

}
void freemem_double(double**src,int row) {
    int i;
    for(i=0;i<row;i++)
        free(src[i]);
    free(src);
}

void getMatrix(FILE*p_in,double**origMatrix,int height,int width) {
    int bytes=height*width;
    byte*buf;
    byte**tmp;
    if(-1==fseek(p_in,0,SEEK_SET)) {
        printf("read yuv file error\n");
        exit(10);
    } if(NULL==(buf=(byte*)calloc(bytes,sizeof(byte))))
    {
        printf("memorry allocation error\n");
        exit(-1);
    }

    if(bytes!=fread(buf,sizeof(byte),bytes,p_in)) {
        printf("read file error\n");
        exit(-3);
    }
    memalloc_byte(&tmp,height,width); 
    {
        int j;
        for(j=0;j<height;j++)
        {
    //        printf("%d ",buf[j]);
     //       fflush(stdout);
            memcpy(tmp[j],buf+j*width,width);

        }
    }
    {
        int i,j;
        for(j=0;j<height;j++)
            for(i=0;i<width;i++) {
   //             printf("%d ",tmp[j][i]);
                origMatrix[j][i]=(double)tmp[j][i];
            }
    }
    freemem_byte(tmp,height);
    free(buf);
}

void calft1(double**origMatrix,double **ftRealMatrix,double **ftImagMatrix,int M,int N) {
    int x,y,u,v;
    double fxy;
    double elemReal; double elemImag;
    double sumReal; double sumImag;
    double tmpcos; double tmpsin;
    double theta;
    for(u=0;u<M;u++) {
        for(v=0;v<N;v++) {
            sumReal=0.0;
            sumImag=0.0;
            for(x=0;x<M;x++) {
                for(y=0;y<N;y++) {

                    fxy=origMatrix[x][y];
  //                  printf("%7.4f ",fxy);
                    theta=2*PI*(u*x*1.0/M+v*y*1.0/N);
                    tmpcos=cos(theta);
                    tmpsin=-sin(theta);
                    elemReal=fxy*tmpcos;
                    elemImag=fxy*tmpsin;

                    sumReal+=elemReal;
                    sumImag+=elemImag;
                }
            }
        // printf("%7.4f %7.4f",sumReal,sumImag);
            ftRealMatrix[u][v]=sumReal/(M*N);
            ftImagMatrix[u][v]=sumImag/(M*N);
         // printf("%7.4f %7.4f",ftRealMatrix[u][v],ftImagMatrix[u][v]);
        }
    }

}
void calft2(double**origMatrix,double **ftRealMatrix,double **ftImagMatrix,int M,int N)
{
   int x,y,u,v;
   double fxy;
   double elemReal; double elemImag;
   double** FxvReal; double** FxvImag;
   double sumReal; double sumImag;
   double tmpcos; double tmpsin;
   double theta;

   double a,b,c,d;

   memalloc_double(&FxvReal,M,N);
   memalloc_double(&FxvImag,M,N);

   for(x=0;x<M;x++) {
       for(v=0;v<N;v++) {
           sumReal=0.0;
           sumImag=0.0;
           for(y=0;y<N;y++) {
              fxy=origMatrix[x][y];
              theta=2*PI*v*y*1.0/N;
              tmpcos=cos(theta);
              tmpsin=-sin(theta);
              elemReal=fxy*tmpcos;
              elemImag=fxy*tmpsin;

              sumReal+=elemReal;
              sumImag+=elemImag;
           }
           FxvReal[x][v]=sumReal/N;
           FxvImag[x][v]=sumImag/N;
       }
   }

    for(u=0;u<M;u++) {
        for(v=0;v<N;v++) {
            sumReal=0.0;
            sumImag=0.0;
            for(x=0;x<M;x++) {
               theta=2*PI*u*x*1.0/M;
               tmpcos=cos(theta);
               tmpsin=-sin(theta);
               a=FxvReal[x][v];
               b=FxvImag[x][v];
               c=tmpcos;
               d=tmpsin;

               elemReal=a*c-b*d;
               elemImag=a*d+b*c;

               sumReal+=elemReal;
               sumImag+=elemImag;
            }
            ftRealMatrix[u][v]=sumReal/M;
            ftImagMatrix[u][v]=sumImag/M;
        }
    }

    freemem_double(FxvReal,M);
    freemem_double(FxvImag,M);
}
void outputMatrix(char*real,char*imag,double**ftRealMatrix,double**ftImagMatrix,int height,int width) {
        int i,j;
        FILE*p_real,*p_imag;
    if(NULL==(p_real=fopen(real,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_imag=fopen(imag,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    for(j=0;j<height;j++) {
        for(i=0;i<width;i++) {
            fprintf(p_real,"%8.4f ",ftRealMatrix[j][i]);
            fprintf(p_imag,"%8.4f ",ftImagMatrix[j][i]);
        }
        fprintf(p_real,"\n");
        fprintf(p_imag,"\n");
    }
        
    fclose(p_real);
    fclose(p_imag);
}
#if 1
int main(int argc,char*argv[])
{
    time_t ltime1,ltime2,ltime3,ltime4,tmp_time;
    struct timeb tstruct1,tstruct2,tstruct3,tstruct4;
    int height,width;
    char *path;
    FILE *p_in;
    double **origMatrix;
    double **ftRealMatrix;
    double **ftImagMatrix;
    if(argc<2) {
        printf("Usage:fourier <image.yuv> <height> <width>\n"
                "Example:fourier foreman.yuv 176 144\n"
                "Output: calft1:86667ms\n"
                "        calft2:1034ms\n"
                );
        exit(-1);
    }
    path=argv[1];
    if(argc<4) {
        height=144;
        width=176;
    }
    else {
        height=atoi(argv[2]);
        width=atoi(argv[3]);
    }

    if(NULL==(p_in=fopen(path,"r"))) {
        printf("open file error\n");
        exit(-4);
    }

    memalloc_double(&origMatrix,height,width);
    memalloc_double(&ftRealMatrix,height,width);
    memalloc_double(&ftImagMatrix,height,width);

    getMatrix(p_in,origMatrix,height,width);

    ftime(&tstruct1);
    time(&ltime1);
    
    calft1(origMatrix,ftRealMatrix,ftImagMatrix,height,width);

    time(&ltime2);
    ftime(&tstruct2);
    tmp_time=(ltime2*1000+tstruct2.millitm)-(ltime1*1000+tstruct1.millitm);
    printf("calft1:%dms\n",(int)tmp_time);

    outputMatrix("real1.txt","image1.txt",ftRealMatrix,ftImagMatrix,height,width);

    ftime(&tstruct3);
    time(&ltime3);
    
    calft2(origMatrix,ftRealMatrix,ftImagMatrix,height,width);

    time(&ltime4);
    ftime(&tstruct4);
    tmp_time=(ltime4*1000+tstruct4.millitm)-(ltime2*1000+tstruct2.millitm);
    printf("calft2:%dms\n",(int)tmp_time);

    outputMatrix("real2.txt","image2.txt",ftRealMatrix,ftImagMatrix,height,width);

    freemem_double(origMatrix,height);
    freemem_double(ftRealMatrix,height);
    freemem_double(ftImagMatrix,height);

    fclose(p_in);
    return 0;

}
#endif

#endif