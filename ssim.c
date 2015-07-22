/*
 * this program is used to calculate the SSIM value between two images
 */
#ifdef _SSIM
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned char imgpel;
typedef unsigned char byte;

void getmem2D(imgpel***img,int dim0,int dim1)
{
    int i;
    if(NULL==(*img=(imgpel**)calloc(dim0,sizeof(imgpel*)))) {
        printf("memorry allocate errror\n");
        exit(-2);
    }
    for(i=0;i<dim0;++i) {
        if(NULL==((*img)[i]=(imgpel*)calloc(dim1,sizeof(imgpel)))) {
        printf("memorry allocate errror\n");
        exit(-2);
        }
    }
}

void freemem2D(imgpel**img,int dim0) {
    int i;
    for(i=0;i<dim0;++i)
        free(img[i]);
    free(img);
}
void getImageData(char*filename,imgpel**img,int height,int width) {
    byte *buf;
    FILE *pImg;
    int frame_bytes=height*width;
    if(NULL==(buf=(byte*)calloc(frame_bytes,sizeof(byte)))) {
        printf("Memorry allocate error\n");
        exit(-2);
    }

    if(NULL==(pImg=fopen(filename,"rb"))) {
        printf("Can't open the file:%s\n",filename);
        exit(-1);
    }
    if(-1==fseek(pImg,0,SEEK_SET)) {
        printf("read image file error\n");
        exit(-1);
    }
    if(frame_bytes!=fread(buf,sizeof(byte),frame_bytes,pImg)) {
        printf("read file error -3\n");
        exit(-3);
    }
    fclose(pImg);
    {
        int j;
        for(j=0;j<height;++j) {
            memcpy(img[j],buf+j*width,sizeof(byte)*width);
        }
    }
    free(buf);
}

float compute_ssim (imgpel **refImg, imgpel **encImg, int height, int width, int win_height, int win_width)
{
  static const float K1 = 0.01f, K2 = 0.03f;
  static float max_pix_value_sqd;
  float C1, C2;
  float win_pixels = (float) (win_width * win_height);
  float mb_ssim, meanOrg, meanEnc;
  float varOrg, varEnc, covOrgEnc;
  int imeanOrg, imeanEnc, ivarOrg, ivarEnc, icovOrgEnc;
  float cur_distortion = 0.0;
  int i, j, n, m, win_cnt = 0;
  int overlapSize = 1;

  max_pix_value_sqd = 255*255.0f;
  C1 = K1 * K1 * max_pix_value_sqd;
  C2 = K2 * K2 * max_pix_value_sqd;

  for (j = 0; j <= height - win_height; j += overlapSize)
  {
    for (i = 0; i <= width - win_width; i += overlapSize)
    {
      imeanOrg = 0;
      imeanEnc = 0; 
      ivarOrg  = 0;
      ivarEnc  = 0;
      icovOrgEnc = 0;

      for ( n = j;n < j + win_height;n ++)
      {
        for (m = i;m < i + win_width;m ++)
        {
          imeanOrg   += refImg[n][m];
          imeanEnc   += encImg[n][m];
          ivarOrg    += refImg[n][m] * refImg[n][m];
          ivarEnc    += encImg[n][m] * encImg[n][m];
          icovOrgEnc += refImg[n][m] * encImg[n][m];
        }
      }

      meanOrg = (float) imeanOrg / win_pixels;
      meanEnc = (float) imeanEnc / win_pixels;

      varOrg    = ((float) ivarOrg - ((float) imeanOrg) * meanOrg) / win_pixels;
      varEnc    = ((float) ivarEnc - ((float) imeanEnc) * meanEnc) / win_pixels;
      covOrgEnc = ((float) icovOrgEnc - ((float) imeanOrg) * meanEnc) / win_pixels;

      mb_ssim  = (float) ((2.0 * meanOrg * meanEnc + C1) * (2.0 * covOrgEnc + C2));
      mb_ssim /= (float) (meanOrg * meanOrg + meanEnc * meanEnc + C1) * (varOrg + varEnc + C2);

      cur_distortion += mb_ssim;
      win_cnt++;
    }
  }

  cur_distortion /= (float) win_cnt;

  if (cur_distortion >= 1.0 && cur_distortion < 1.01) // avoid float accuracy problem at very low QP(e.g.2)
    cur_distortion = 1.0;

  return cur_distortion;
}

#if 0
int main(int argc,char*argv[])
{
    char*refname,*encname;
    imgpel **refImg,**encImg;
    int height,width;
    int win_height,win_width;
    float ssim_score;
    
    if(argc<2) {
        printf("Usage:ssim <refImage> <encImage> <height> <width> <win_height> <win_width>\n");
        exit(0);
    }
    refname=argv[1];
    encname=argv[2];

    height=argc<4?144:atoi(argv[3]);
    width=argc<4?176:atoi(argv[4]);
    
    win_height=argc<6?8:atoi(argv[5]);
    win_width=argc<6?8:atoi(argv[6]);

    getmem2D(&refImg,height,width);
    getmem2D(&encImg,height,width);

    getImageData(refname,refImg,height,width);
    getImageData(encname,encImg,height,width);
    {
        int i;
        FILE *pssim=fopen("ssim.txt","w");
        //        for(i=1;i<=16;++i) {
        //           win_height=i;
        //          win_width=i;
        ssim_score=compute_ssim(refImg,encImg,height,width,win_height,win_width);
        fprintf(pssim,"%f ",ssim_score);
        //   }
        fclose(pssim);
    }

    freemem2D(refImg,height);
    freemem2D(encImg,height);

    return 0;
}
#endif
#endif