#include "j_minimizer.h"
#include <math.h>
#include <stdio.h>

#define ITMAX 200

void j_powell( double p[],double **xi, int n,double ftol,int *iter,
	       double *fret,double (*func)(double [],void *),void *localdata)
{
        double sqrarg1, sqrarg2;
	int i,ibig,j;
	double t,fptt,fp,del;
	double *pt,*ptt,*xit;

	pt=vector(1,n);
	ptt=vector(1,n);
	xit=vector(1,n);
	*fret=(*func)(p,localdata);
	for (j=1;j<=n;j++) pt[j]=p[j];
	for (*iter=1;;(*iter)++) {
		fp=(*fret);
		ibig=0;
		del=0.0;
		for (i=1;i<=n;i++) {
			for (j=1;j<=n;j++) xit[j]=xi[j][i];
			fptt=(*fret);
			j_linmin(p,xit,n,fret,func,localdata);
			if (fabs(fptt-(*fret)) > del) {
				del=fabs(fptt-(*fret));
				ibig=i;
			}
		}
		if (2.0*fabs(fp-(*fret)) <= ftol*(fabs(fp)+fabs(*fret))) {
			free_vector(xit,1,n);
			free_vector(ptt,1,n);
			free_vector(pt,1,n);
			return;
		}
		if (*iter == ITMAX) { printf("Too many iterations in routine J_POWELL"); return; }
		for (j=1;j<=n;j++) {
			ptt[j]=2.0*p[j]-pt[j];
			xit[j]=p[j]-pt[j];
			pt[j]=p[j];
		}
		fptt=(*func)(ptt,localdata);
		if (fptt < fp) {
		        sqrarg1=fp-(*fret)-del;
                        sqrarg2=fp-fptt;
			t=2.0*(fp-2.0*(*fret)+fptt)*sqrarg1*sqrarg1-del*sqrarg2*sqrarg2;
			if (t < 0.0) {
			        j_linmin(p,xit,n,fret,func,localdata);
				for (j=1;j<=n;j++) xi[j][ibig]=xit[j];
			}
		}
	}
}

#undef ITMAX
