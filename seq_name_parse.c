/*
 * this program uses strchr to parse the sequence name to get some information
 */
#ifdef _SEQ_NAME_PARSE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(){
//    char *in_file="e:\\sequences\\BlowingBubbles_416x240_50.yuv";
//    char *in_file="e:/sequences/BlowingBubbles_416x240_50.yuv";
    char *in_file="BlowingBubbles_416x240_50.yuv";
   char *pos,*pos2;
   char ss[50];
   pos=strrchr(in_file,'\\');
   if(pos==NULL)
       pos=strrchr(in_file,'/');
   if(pos==NULL)
       pos=in_file;
   else
       pos++;
   printf("pos=%s\n",pos);
   pos2=strrchr(in_file,'.');
   if(pos2==NULL)
       return -1;
   printf("pos2=%s\n",pos2);
   strncpy(ss,pos,pos2-pos);
   strcpy(ss+(pos2-pos),".bin");
   printf("%s\n",ss);
   strcpy(ss+(pos2-pos),".yuv");
   printf("%s\n",ss);
   
   ss[pos2-pos]='\0';
   printf("%s\n",ss);
   int width,height,fps;
   sscanf(ss,"%*[A-Za-z_]%dx%d_%d",&width,&height,&fps);
   printf("width=%d height=%d fps=%d\n",width,height,fps);


   
}
#endif
