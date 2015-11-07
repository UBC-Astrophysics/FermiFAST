#include "j_minimizer.h"

double j_f1dim(double x, void *localdata)
{
        int j, ncom=((J_LINMIN_STRUCT *) localdata)->ncom;
	double *pcom=((J_LINMIN_STRUCT *) localdata)->pcom, *xicom=((J_LINMIN_STRUCT *) localdata)->xicom;
	double f,*xt;
	
	xt=vector(1,ncom);
	for (j=1;j<=ncom;j++) xt[j]=pcom[j]+x*xicom[j];
	f=(*(((J_LINMIN_STRUCT *) localdata)->nrfunc))(xt,((J_LINMIN_STRUCT *) localdata)->localdata);
	free_vector(xt,1,ncom);
	return f;
}

#define TOL 2.0e-4

void j_linmin( double p[],double xi[],int n,double *fret,double (*func)(double x[], void *localdata),void *localdata)
{
	int j;
	double xx,xmin,fx,fb,fa,bx,ax;
	J_LINMIN_STRUCT jls;
	jls.ncom=n;
	jls.pcom=vector(1,n);
	jls.xicom=vector(1,n);
	jls.nrfunc=func;
	jls.localdata=localdata;
	for (j=1;j<=n;j++) {
		jls.pcom[j]=p[j];
		jls.xicom[j]=xi[j];
	}
	ax=0.0;
	xx=1.0;
	bx=2.0;
	j_mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,j_f1dim,(void *) &jls);
	*fret=j_brent(ax,xx,bx,j_f1dim,TOL,&xmin,(void *) &jls);
	for (j=1;j<=n;j++) {
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free_vector(jls.xicom,1,n);
	free_vector(jls.pcom,1,n);
}

#undef TOL
