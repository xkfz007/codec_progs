#ifdef _VLC
#include <stdio.h>
int fast_log2(int x){
    if(x<=0)
        return 0;
    float fx;
    unsigned long ix,exp;
    fx=(float)x;
    ix=*(unsigned long*)&fx;
    exp=(ix>>23)&0xff;
    return exp-127;
}
void ue_v(int codeNum,int *M,int *INFO){
    int i=0;
    int tmp=codeNum+1;//(codeNum+1)>>1;
    for(i=0;i<33&&tmp>1;i++)
        tmp>>=1;
    *M=i;//fast_log2(codeNum+1);
    *INFO=codeNum+1-(1<<i);
}
void print_s(int M,int INFO){
    int i;
    char s[32]={'0'};
    for(i=0;i<M;i++)
        printf("0");
    printf("1");
    for(i=M-1;i>=0;i--)
    {
        int t=INFO&0x1;
        s[i]=t+'0';
        INFO>>=1;
    }
    for(i=0;i<M;i++)
        printf("%c",s[i]);
}
#if 1
int main(){
    int i,M,INFO;
    for(i=1;i<=20;i++){
        ue_v(i,&M,&INFO);
        printf("i=%3d M=%3d INFO=%3d : ",i,M,INFO);
        print_s(M,INFO);
        printf("\n");
    }
}
#endif
#if 0
int main(){
    int i;
    for(i=-32;i<33;i++)
        printf("i=%d log=%d\n",i,fast_log2(i));
}

#endif
#endif
