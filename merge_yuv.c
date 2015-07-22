/*
create a merged yuv sequence
*/
#ifdef _MERGE_YUV
#include <stdio.h>
#include <stdlib.h>

int main3()
{
	int width=416;
	int height=240;
	int fps=50;
	int size=width*height*3/2;
	int num=300;
	int i;
	char*frame=malloc(size*sizeof(char));
	FILE* fp= fopen("E:\\sequences\\BlowingBubbles_416x240_50.yuv", "rb");
	FILE* fp2= fopen("E:\\sequences\\BlowingBubbles2_416x240_50.yuv", "wb");
	if(fp==NULL)
		fprintf(stderr,"Can't open input file!\n"),exit(-1);
	if(fp2==NULL)
		fprintf(stderr,"Can't open output file!\n"),exit(-1);
	fseek(fp,num*size,SEEK_SET);
	while(fread(frame,sizeof(char),size,fp)==size)
	{
		fwrite(frame,sizeof(char),size,fp2);
		fprintf(stdout,".");
	}
	fseek(fp2,-size,SEEK_END);
	fseek(fp,0,SEEK_SET);
	i=0;
	while(i<num)
	{
		fread(frame,sizeof(char),size,fp);
		fwrite(frame,sizeof(char),size,fp2);
		i++;
		fprintf(stdout,".");
	}
	free(frame);
	fclose(fp);
	fclose(fp2);
}

void merge(int width, int height, int  framebegin, int framenum, FILE* input, FILE* output)
{
	int size = height*width*3/2;
	char*  buf = (char*)malloc(size);
        int i=0;
	fseek(input, SEEK_SET, framebegin*size);
        
        while(i<framenum&&fread(buf,sizeof(char),size,input)==size){
            fwrite(buf,sizeof(char),size,output);
            i++;
            fprintf(stdout,".");
            fflush(stdout);
        }
        fprintf(stdout,"\n");
        fflush(stdout);
	free(buf);
}
#define MERGE_FILE(FILENAME,frm_start,frm_end) do{\
        filein= fopen(FILENAME, "rb");\
        fprintf(stdout,"FILE:%s",FILENAME);\
        merge(width,height,frm_start,frm_end,filein,fileout);\
        fclose(filein);\
}while(0)

int main(int argc, char* argv[]) {
	//\\\\172.21.60.128\\hfz\\sequences
	FILE* fileout = fopen("e:\\sequences\\Mergewvga_416x240_30.yuv", "wb");
	FILE* filein ;
        int width=416,
            height=240;

        MERGE_FILE("e:\\sequences\\RaceHorses_416x240_30.yuv",0,250);
        MERGE_FILE("e:\\sequences\\Flowervase_416x240_30.yuv",0,250);
        MERGE_FILE("e:\\sequences\\Keiba_416x240_30.yuv",0,250);
        MERGE_FILE("e:\\sequences\\Mobisode2_416x240_30.yuv",0,250);
        MERGE_FILE("e:\\sequences\\BasketballPass_416x240_50.yuv",0,250);
        MERGE_FILE("e:\\sequences\\BlowingBubbles_416x240_50.yuv",0,250);
        MERGE_FILE("e:\\sequences\\BQSquare_416x240_60.yuv",0,250);
        MERGE_FILE("e:\\sequences\\RaceHorses_416x240_30.yuv",0,250);
        MERGE_FILE("e:\\sequences\\Keiba_416x240_30.yuv",0,250);
        MERGE_FILE("e:\\sequences\\BasketballPass_416x240_50.yuv",0,250);

	fclose(fileout);
}

#endif