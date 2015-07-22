#ifdef _QP_BITRATE
#include <stdio.h>
#include <math.h>
double qp2qscale(double qp)
{
        return 0.85 * pow(2.0, ( qp - 12.0 ) / 6.0);
}
double qscale2qp(double qscale)
{
        return 12.0 + 6.0 * log(qscale/0.85) / log(2.0);
}
int clip3( int v, int i_min, int i_max )
{
        return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}
double get_qscale(int width,int height,double sad,double brate,double fps){
    double last_satd=sad,
           bitrate=brate*1000;
    double short_term_cplxsum=last_satd,
           short_term_count=1,
           blurred_complexity=short_term_cplxsum/short_term_count;
    double f_qcompress=0.6;
    int r1=width%64,r2=height%64;
    int numberOfLCU=(width/64+r1)*(height/64+r2);
    double wanted_bits_window=bitrate/fps,
           cplxr_sum=0.01*pow(7.0e5,f_qcompress)*pow(numberOfLCU,0.5);
    double ratefactor=wanted_bits_window/cplxr_sum;
    double overflow=1.00;
    double q;


    q=pow(blurred_complexity,1-f_qcompress);

    q/=ratefactor;

    q*=overflow;

    return q;
}
int main(){
    int width=416,height=240;
    double sad=1728133;
    double fps=30;
    int i;
    for(i=0;i<1000;i++){
        double brate=(i+1)*10;
        double q=get_qscale(width,height,sad,brate,fps);
        int qp=(int)qscale2qp(q);
        int qp2=clip3(qp,0,51);
        printf("%f\t%f\t%d\t%d\n",brate,q,qp,qp2);
    }
}
#endif