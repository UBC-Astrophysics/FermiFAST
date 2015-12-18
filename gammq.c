#include <math.h>

double gammln(xx)
double xx;
{
	double x,tmp,ser;
	static double cof[6]={76.18009173,-86.50532033,24.01409822,
		-1.231739516,0.120858003e-2,-0.536382e-5};
	int j;

	x=xx-1.0;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.0;
	for (j=0;j<=5;j++) {
		x += 1.0;
		ser += cof[j]/x;
	}
	return -tmp+log(2.50662827465*ser);
}

#define ITMAX 100
#define EPS 3.0e-7

void lgcf(lgammcf,a,x,gln)
double a,x,*lgammcf,*gln;
{
	int n;
	double gold=0.0,g,fac=1.0,b1=1.0;
	double b0=0.0,anf,ana,an,a1,a0=1.0;
	double gammln();

	*gln=gammln(a);
	a1=x;
	for (n=1;n<=ITMAX;n++) {
		an=(double) n;
		ana=an-a;
		a0=(a1+a0*ana)*fac;
		b0=(b1+b0*ana)*fac;
		anf=an*fac;
		a1=x*a0+anf*a1;
		b1=x*b0+anf*b1;
		if (a1) {
			fac=1.0/a1;
			g=b1*fac;
			if (fabs((g-gold)/g) < EPS) {
			        *lgammcf=-x+a*log(x)-(*gln)+log(g);
				return;
			}
			gold=g;
		}
	}
}

#undef ITMAX
#undef EPS

#define ITMAX 100
#define EPS 3.0e-7

void gser(gamser,a,x,gln)
double a,x,*gamser,*gln;
{
	int n;
	double sum,del,ap;
	double gammln();

	*gln=gammln(a);
	if (x <= 0.0) {
	        if (x < 0.0) return;
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			ap += 1.0;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
		return;
	}
}

#undef ITMAX
#undef EPS



double lnprob(double dof, double chi2) {
        double gamser,lgammcf,gln, a, x;
	void lgcf(),gser();
	
	a=dof*0.5;  x=chi2*0.5;
	if (x < 0.0 || a <= 0.0) return -1;
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return log1p(-gamser);
	} else {
		lgcf(&lgammcf,a,x,&gln);
		return lgammcf;
	}

}
