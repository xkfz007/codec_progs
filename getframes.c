/*
 * This program is used to get some continuous frames from the yuv sequences.
 */
#ifdef _GETFRAMES
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char byte;

int main(int argc,char*argv[])
{
    char *yuvpath;
    char outpath[100];
    int from; int to;
    FILE *p_in; FILE *p_out;
    int height; int width;
    int framebytes;
    byte *buf;
    if(argc<2) {
        printf("Usage:getframes <xxx.yuv> <from> <to> <height> <width>\n");
        exit(-1);
    }
    yuvpath=argv[1];
    if(argc<3) 
        from=0;
    else {
        int tmp=atoi(argv[2]);
        from=tmp>=0?tmp:0;
    }
    if(argc<4) 
        to=130;
    else {
        int tmp=atoi(argv[3]);
        to=tmp>0?tmp:130;
    }
    if(argc<5) {
        height=144;
        width=176;
    }
    else {
        int tmph=atoi(argv[4]);
        int tmpw=atoi(argv[5]);
        height=tmph>0?tmph:144;
        width=tmpw>0?tmpw:176;
    }
    framebytes=height*width+height/2*width/2*2;
    if(NULL==(buf=(byte*)calloc(framebytes,sizeof(byte))))
    {
        printf("Merrory allocate error!\n");
        exit(-1);
    }
    sprintf(outpath,"%s_%d_%d",yuvpath,from,to);
    
    if(NULL==(p_in=fopen(yuvpath,"r"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_out=fopen(outpath,"w"))) {
        printf("open file error\n");
        exit(-4);
    }

    {
        for(int i=from,j=0;i<to;i++,j++)
        {
          if(-1==fseek(p_in,framebytes*i,SEEK_SET))
          {
            printf("read yuv file error\n");
            exit(10);
          }
         if(framebytes!=fread(buf,sizeof(byte),framebytes,p_in))
         {
           printf("read file error\n");
           exit(-3);
          }
          if(-1==fseek(p_out,framebytes*j,SEEK_SET))
          {
            printf("read yuv file error\n");
            exit(10);
          }
         if(framebytes!=fwrite(buf,sizeof(byte),framebytes,p_out))
         {
           printf("write file error\n");
           exit(-7);
          }

        }

    }
    printf("%s done!\n",outpath);
        fclose(p_in);
        fclose(p_out);
        free(buf);

    return 0;

}

#endif