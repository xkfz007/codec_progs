/*
 * this program is used to calculate the value of generalized gaussian distribution.
 */
#ifdef _GGD
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>
#define TIMEB timeb
#define PI 3.1415926
#define TIC ftime(&tstruct1);\
    time(&ltime1);
#define TOC time(&ltime2);\
    ftime(&tstruct2);
#define TIME (ltime2 * 1000 + tstruct2.millitm) - (ltime1 * 1000 + tstruct1.millitm)

const int g = 7;
const double p[9] = {0.99999999999980993, 676.5203681218851, -1259.1392167224028, 
                    771.32342877765313, -176.61502916214059, 12.507343278686905, 
                    -0.13857109526572012, 9.9843695780195716e-6, 1.5056327351493116e-7};

time_t ltime1, ltime2, tmp_time;
struct TIMEB tstruct1, tstruct2;

double gamma(double z)
{
//TIC
    int i;
    double x,t;
    double y;
    if(z<0.5)
        y=PI/(sin(z*PI)*gamma(1-z));
    else
    {
        z--;
        x=p[0];
        for(i=1;i<g+2;i++)
            x+=p[i]/(z+(double)i);
        t=z+g+0.5;
        y=sqrt(2*PI) * pow(t,z+0.5) * exp(-t) * x;
    }
//    TOC;
//printf("%s time:%d\n",__func__,TIME);
    return y;
}

double generalized_gauss(double sigma,double eta,double x){
//    TIC
    double g1,g2;
    double alpha;
    double f;
    g1=gamma(3.0/eta);
    g2=gamma(1.0/eta);
    alpha=sqrt(g1/g2);
    f=0.5*eta*alpha/(sigma*g2)*exp(-pow(alpha*abs(x)/sigma,eta));
//    TOC
//printf("%s time:%d\n",__func__,TIME);
    return f;
}

double gauss_leg(double a,double b,double *t,double *c,int n,double eps,double f(double)){
    int m,i,j;
  double s,p,ep,h,aa,bb,w,x,g;
  m=1;
  h=b-a; s=fabs(0.001*h);
  p=1.0e+35; ep=eps+1.0;
  int cnt=0;
  while ((ep>=eps)&&(fabs(h)>s))
    { 
    TIC
        g=0.0;
      for (i=1;i<=m;i++)
        { aa=a+(i-1.0)*h; bb=a+i*h;
          w=0.0;
          for (j=0;j<n;j++)
            { x=((bb-aa)*t[j]+(bb+aa))/2.0;
              w=w+(*f)(x)*c[j];
            }
          g=g+w;
        }
      g=g*h/2.0;
      ep=fabs(g-p)/(1.0+fabs(g));
      p=g; m=m+1; h=(b-a)/m;
      cnt++;
    TOC
printf("%s time:%d %d\n",__func__,TIME,cnt);
    }
  return(g);

}
const int GAU_LEG=10;
double xk[10]={
-0.9739065285171717200779640,
-0.8650633666889845107320967,
-0.6794095682990244062343274,
-0.4333953941292471907992659,
-0.1488743389816312108848260,
+0.1488743389816312108848260,
+0.4333953941292471907992659,
+0.6794095682990244062343274,
+0.8650633666889845107320967,
+0.9739065285171717200779640,
};
double Ak[10]={
0.0666713443086881375935688,
0.1494513491505805931457763,
0.2190863625159820439955349,
0.2692667193099963550912269,
0.2955242247147528701738930,
0.2955242247147528701738930,
0.2692667193099963550912269,
0.2190863625159820439955349,
0.1494513491505805931457763,
0.0666713443086881375935688,
};
double sigma=2;
double eta=3;
double f(double x){
    return generalized_gauss(sigma,eta,x);
}
#if 0
int main(){
	int i;
	double xx=0.0;
	const double EPS=1.0e-14;
    double a=-5.0,b=5.0;
    xx=gauss_leg(a,b,xk,Ak,GAU_LEG,EPS,f);
	printf("\nIntegral from GAULEG: %f\n",xx);

}
#endif
#endif