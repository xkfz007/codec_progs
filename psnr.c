/*
 * 本程序主要用于计算两个原始和重建yuv文件之间的psnr值
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef unsigned char byte;
double calc_psnr(byte* orig,byte* rec,int N){
    int i;
    int err_sum=0;
    int e;
    double mse;
    for(i=0;i<N;i++){
        e=*(orig+i)-*(rec+i);
        err_sum+=e*e;
    }
    mse=(err_sum*1.0/N);
    return 10*log10(255*255/mse);
}
void calc_all(FILE* p_orig,FILE* p_rec,int from,int to,int width,int height){
    double psnr[3],psnr_sum[3];
    int pic_num;
    int framebytes;
    int luma_sz,chroma_sz;
    byte *orig_frm,*rec_frm;
    framebytes=height*width+height/2*width/2*2;
    luma_sz=height*width;
    chroma_sz=height/2*width/2;
    if(NULL==(orig_frm=(byte*)calloc(framebytes,sizeof(byte)))
            ||NULL==(rec_frm=(byte*)calloc(framebytes,sizeof(byte)))) {
        printf("Merrory allocate error!\n");
        exit(-2);
    }
    pic_num=from;
    psnr_sum[0]=psnr_sum[1]=psnr_sum[2]=0.0;
    fseek(p_orig,from*framebytes, SEEK_SET);
    fseek(p_rec,from*framebytes, SEEK_SET);
    while(framebytes==(fread(orig_frm,sizeof(byte),framebytes,p_orig))
            && framebytes==(fread(rec_frm,sizeof(byte),framebytes,p_rec))
            && pic_num<=to)
    {
        psnr[0]=calc_psnr(orig_frm,rec_frm,luma_sz);
        psnr[1]=calc_psnr(orig_frm+luma_sz,rec_frm+luma_sz,chroma_sz);
        psnr[2]=calc_psnr(orig_frm+luma_sz+chroma_sz,rec_frm+luma_sz+chroma_sz,chroma_sz);
        psnr_sum[0]+=psnr[0];
        psnr_sum[1]+=psnr[1];
        psnr_sum[2]+=psnr[2];
        printf("%4d %7.4f %7.4f %7.4f\n",pic_num++,psnr[0],psnr[1],psnr[2]);
    }
    printf("avg: %7.4f %7.4f %7.4f\n",psnr_sum[0]/(pic_num-from),psnr_sum[1]/(pic_num-from),psnr_sum[2]/(pic_num-from));
    free(orig_frm);
}
void help(){
        printf("Usage:psnr <width> <height> <orig.yuv> <rec.yuv> from to\n"
                "Example:psnr 176 144 foreman_qcif.yuv foreman_qcif_rec.yuv 5 100\n");
}

#if 0
int main(int argc,char*argv[])
{
    FILE *p_orig, *p_rec;
    int height,width;
    int from,to;
    if(argc<5) {
        help();
        exit(-1);
    }
    width=atoi(argv[1]);
    height=atoi(argv[2]);
    if(width<=0||height<=0){
        printf("width or height can't be 0 or negative\n");
        exit(-1);
    }
    if(NULL==(p_orig=fopen(argv[3],"r"))) {
        printf("can't open the original yuv file\n");
        exit(-4);
    }
    if(NULL==(p_rec=fopen(argv[4],"r"))) {
        printf("can't open the reconstruct yuv file\n");
        exit(-4);
    }
    if(argc<6){
        from=0;
        to=1<<16;
        calc_all(p_orig,p_rec,from,to,width,height);
    }
    else if(argc<7){
        from=atoi(argv[5]);
        if(from<0){
            printf("invalid argument of <from> \n");
            exit(-1);
        }
        to=1<<16;
        calc_all(p_orig,p_rec,from,to,width,height);
    }
    else{
        from=atoi(argv[5]);
        to=atoi(argv[6]);
        if(from<0||to<0||from>to){
            printf("invalid arguments of from or to\n");
            exit(-1);
        }
        calc_all(p_orig,p_rec,from,to,width,height);
    }

    fclose(p_orig);
    fclose(p_rec);

    return 0;
}

#endif