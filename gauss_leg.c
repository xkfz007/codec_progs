#ifdef _GGD
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
void gauleg(double x1, double x2, double *x, double *w,int n,double EPS)
{
	double z1,z,xm,xl,pp,p3,p2,p1;
	int m=(n+1)/2;
    int i,j;
	xm=0.5*(x2+x1);
	xl=0.5*(x2-x1);
	for (i=0;i<m;i++) {
		z=cos(3.141592654*(i+0.75)/(n+0.5));
		do {
			p1=1.0;
			p2=0.0;
			for (j=0;j<n;j++) {
				p3=p2;
				p2=p1;
				p1=((2.0*j+1.0)*z*p2-j*p3)/(j+1);
			}
			pp=n*(z*p1-p2)/(z*z-1.0);
			z1=z;
			z=z1-p1/pp;
		} while (abs(z-z1) > EPS);
		x[i]=xm-xl*z;
		x[n-1-i]=xm+xl*z;
		w[i]=2.0*xl/((1.0-z*z)*pp*pp);
		w[n-1-i]=w[i];
	}
}

double gslr(double a,double b,double eps,double (*f)(double))
{ 
    int m,i,j;
  double s,p,ep,h,aa,bb,w,x,g;
  static double t[5]={-0.9061798459,-0.5384693101,0.0,
                       0.5384693101,0.9061798459};
  static double c[5]={0.2369268851,0.4786286705,0.5688888889,
                      0.4786286705,0.2369268851};
  m=1;
  h=b-a; s=fabs(0.001*h);
  p=1.0e+35; ep=eps+1.0;
  while ((ep>=eps)&&(fabs(h)>s))
    { g=0.0;
      for (i=1;i<=m;i++)
        { aa=a+(i-1.0)*h; bb=a+i*h;
          w=0.0;
          for (j=0;j<=4;j++)
            { x=((bb-aa)*t[j]+(bb+aa))/2.0;
              w=w+(*f)(x)*c[j];
            }
          g=g+w;
        }
      g=g*h/2.0;
      ep=fabs(g-p)/(1.0+fabs(g));
      p=g; m=m+1; h=(b-a)/m;
    }
  return(g);
}
double t[5]={-0.9061798459,-0.5384693101,0.0, 0.5384693101,0.9061798459};
double c[5]={0.2369268851,0.4786286705,0.5688888889, 0.4786286705,0.2369268851};
double gau_leg(double a,double b,double *t,double *c,int n,double eps,double f(double)){
    int m,i,j;
  double s,p,ep,h,aa,bb,w,x,g;
  m=1;
  h=b-a; s=fabs(0.001*h);
  p=1.0e+35; ep=eps+1.0;
  while ((ep>=eps)&&(fabs(h)>s))
    { g=0.0;
      for (i=1;i<=m;i++)
        { aa=a+(i-1.0)*h; bb=a+i*h;
          w=0.0;
          for (j=0;j<=4;j++)
            { x=((bb-aa)*t[j]+(bb+aa))/2.0;
              w=w+(*f)(x)*c[j];
            }
          g=g+w;
        }
      g=g*h/2.0;
      ep=fabs(g-p)/(1.0+fabs(g));
      p=g; m=m+1; h=(b-a)/m;
    }
  return(g);

}
#define NPOINT 5
#define X1 0.0
#define X2 1.0
#define X3 10.0

double func(double x)
{
	return x*exp(-x);
}

#if 0
int main(void)
{
	int i;
	double xx=0.0;
	double x[NPOINT],w[NPOINT];
    double xx2;
	const double EPS=1.0e-14;

	gauleg(X1,X2,x,w,NPOINT,EPS);
	printf("\n%2s %10s %12s\n","#","x[i]","w[i]");
	for (i=1;i<=NPOINT;i++)
		printf("%2d %12.6f %12.6f\n",i,x[i],w[i]);
	/* Demonstrate the use of gauleg for integration */
	gauleg(X1,X3,x,w,NPOINT,EPS);
	for (i=1;i<=NPOINT;i++)
		xx += (w[i]*func(x[i]));
	printf("\nIntegral from GAULEG: %12.6f\n",xx);
	printf("Actual value: %12.6f\n",
		(1.0+X1)*exp(-X1)-(1.0+X3)*exp(-X3));

    xx2=gslr(X1,X3,EPS,func);
	printf("Actual value: %12.6f\n",xx2);


	return 0;
}

#endif
#endif