/*
 * this program is used to obtain the specific frame from the yuv sequence.
 */
#ifdef _READIMG
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char byte;
byte**y;
byte**u;
byte**v;
void getimgdata(FILE*p_in,int fno,int height,int width) {
    int bytes_y=height*width;
    int bytes_uv=height/2*width/2;
    int frame_bytes=bytes_y+bytes_uv*2;
    byte*buf;
    byte*buf_y;
    byte*buf_u;
    byte*buf_v;

    if(-1==fseek(p_in,frame_bytes*fno,SEEK_SET)) {
        printf("read yuv file error\n");
        exit(10);
    }
    if(NULL==y||NULL==u||NULL==v) {
        printf("y/u/v:no memory allocation\n");
        exit(-2);
    }
    if(NULL==(buf=(byte*)calloc(frame_bytes,sizeof(byte)))) {
        printf("memorry allocation error\n");
        exit(-1);
    }
    if(NULL==(buf_y=(byte*)calloc(bytes_y,sizeof(byte)))) {
        printf("memorry allocation error\n");
        exit(-1);
    }
    if(NULL==(buf_u=(byte*)calloc(bytes_uv,sizeof(byte)))) {
        printf("memorry allocation error\n");
        exit(-1);
    }
    if(NULL==(buf_v=(byte*)calloc(bytes_uv,sizeof(byte)))) {
        printf("memorry allocation error\n");
        exit(-1);
    }
    if(frame_bytes!=fread(buf,sizeof(byte),frame_bytes,p_in)) {
        printf("read file error\n");
        exit(-3);
    }
    memcpy(buf_y,buf,bytes_y);
    memcpy(buf_u,buf+bytes_y,bytes_uv);
    memcpy(buf_v,buf+bytes_y+bytes_uv,bytes_uv);
    {
        int j;
        for(j=0;j<height;j++)
            memcpy(y[j],buf_y+j*width,width);
        
        for(j=0;j<height/2;j++)
        {
            memcpy(u[j],buf_u+j*width/2,width/2);
            memcpy(v[j],buf_v+j*width/2,width/2);
        }
    }
    free(buf);
    free(buf_y);
    free(buf_u);
    free(buf_v);
}
void memalloc(byte***src,int row,int col) {
    int i;
    *src=(byte**)calloc(row,sizeof(byte*));
    for(i=0;i<row;i++)
        (*src)[i]=(byte*)calloc(col,sizeof(byte));

}
void freemem(byte**src,int row) {
    int i;
    for(i=0;i<row;i++)
        free(src[i]);
    free(src);
}
int main(int argc,char*argv[]) {
    char *yuvpath;
    int framenumber;
    FILE *p_in;
    int height;
    int width;
    if(argc<2) {
        printf("Usage:readimg <xxx.yuv> <fno> <height> <width>\n");
        exit(-1);
    }
    yuvpath=argv[1];
    if(argc<3) {
        framenumber=0;
    }
    else {
        framenumber=atoi(argv[2]);
    }
    if(argc<5) {
        height=144;
        width=176;
    }
    else {
        height=atoi(argv[3]);
        width=atoi(argv[4]);
    }
    memalloc(&y,height,width);
    memalloc(&u,height/2,width/2);
    memalloc(&v,height/2,width/2);
    if(NULL==(p_in=fopen(yuvpath,"r"))) {
        printf("open file error\n");
        exit(-4);
    }

    getimgdata(p_in,framenumber,height,width);
    {
        FILE *p_y,*p_u,*p_v;
        FILE *p_y2,*p_u2,*p_v2;
        int i,j;
        char yname[20]="y";
        char yname2[20]="y";
        char uname[20]="u";
        char uname2[20]="u";
        char vname[20]="v";
        char vname2[20]="v";

        sprintf(yname,"%s_%d.txt",yname,framenumber);
        sprintf(yname2,"%s_%d.bin",yname2,framenumber);
        sprintf(uname,"%s_%d.txt",uname,framenumber);
        sprintf(uname2,"%s_%d.bin",uname2,framenumber);
        sprintf(vname,"%s_%d.txt",vname,framenumber);
        sprintf(vname2,"%s_%d.bin",vname2,framenumber);


    if(NULL==(p_y=fopen(yname,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_u=fopen(uname,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_v=fopen(vname,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_y2=fopen(yname2,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_u2=fopen(uname2,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    if(NULL==(p_v2=fopen(vname2,"w"))) {
        printf("open file error\n");
        exit(-4);
    }
    for(j=0;j<height;j++) {
        for(i=0;i<width;i++) {
            fprintf(p_y,"%d ",y[j][i]);
        }
        fprintf(p_y,"\n");
    fwrite(y[j],sizeof(byte),width,p_y2);
    }
    for(j=0;j<height/2;j++) {
        for(i=0;i<width/2;i++) {
            fprintf(p_u,"%d ",u[j][i]);
            fprintf(p_v,"%d ",v[j][i]);
        }
        fprintf(p_u,"\n");
        fprintf(p_v,"\n");
    fwrite(u[j],sizeof(byte),width/4,p_u2);
    fwrite(v[j],sizeof(byte),width/4,p_v2);
    }


    fclose(p_y);
    fclose(p_u);
    fclose(p_v);
    fclose(p_y2);
    fclose(p_u2);
    fclose(p_v2);
    }

    freemem(y,height);
    freemem(u,height/2);
    freemem(v,height/2);

    return 0;
}

#endif