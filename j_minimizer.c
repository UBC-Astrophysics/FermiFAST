#include <math.h>
#include "j_minimizer.h"

#define GOLD 1.618034
#define GLIMIT 100.0
#define TINY 1.0e-20
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define SIGN(a,b) ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

void j_mnbrak(double *ax,double *bx,double *cx,double *fa,double *fb,double *fc,
	      double (*func)(double a, void *localdata),void *localdata)
{
	double ulim,u,r,q,fu,dum;

	*fa=(*func)(*ax,localdata);
	*fb=(*func)(*bx,localdata);
	if (*fb > *fa) {
		SHFT(dum,*ax,*bx,dum)
		SHFT(dum,*fb,*fa,dum)
	}
	*cx=(*bx)+GOLD*(*bx-*ax);
	*fc=(*func)(*cx,localdata);
	while (*fb > *fc) {
		r=(*bx-*ax)*(*fb-*fc);
		q=(*bx-*cx)*(*fb-*fa);
		u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
			(2.0*SIGN(MAX(fabs(q-r),TINY),q-r));
		ulim=(*bx)+GLIMIT*(*cx-*bx);
		if ((*bx-u)*(u-*cx) > 0.0) {
			fu=(*func)(u,localdata);
			if (fu < *fc) {
				*ax=(*bx);
				*bx=u;
				*fa=(*fb);
				*fb=fu;
				return;
			} else if (fu > *fb) {
				*cx=u;
				*fc=fu;
				return;
			}
			u=(*cx)+GOLD*(*cx-*bx);
			fu=(*func)(u,localdata);
		} else if ((*cx-u)*(u-ulim) > 0.0) {
			fu=(*func)(u,localdata);
			if (fu < *fc) {
				SHFT(*bx,*cx,u,*cx+GOLD*(*cx-*bx))
				SHFT(*fb,*fc,fu,(*func)(u,localdata))
			}
		} else if ((u-ulim)*(ulim-*cx) >= 0.0) {
			u=ulim;
			fu=(*func)(u,localdata);
		} else {
			u=(*cx)+GOLD*(*cx-*bx);
			fu=(*func)(u,localdata);
		}
		SHFT(*ax,*bx,*cx,u)
		SHFT(*fa,*fb,*fc,fu)
	}
}

#undef GOLD
#undef GLIMIT
#undef TINY
#undef MAX
#undef SIGN
#undef SHFT

#define ITMAX 100
#define ZEPS 1.0e-10
#define SIGN(a,b) ((b) > 0.0 ? fabs(a) : -fabs(a))
#define MOV3(a,b,c, d,e,f) (a)=(d);(b)=(e);(c)=(f);

double j_dbrent(double ax,double bx,double cx,double (*f)(double, double *, void *localdata),double tol,double *xmin, void *localdata)
{
  int iter,ok1,ok2;
  double a,b,d,d1,d2,du,dv,dw,dx,e=0.0;
  double fu,fv,fw,fx,olde,tol1,tol2,u,u1,u2,v,w,x,xm;
  void nrerror();
  
  a=(ax < cx ? ax : cx);
  b=(ax > cx ? ax : cx);
  x=w=v=bx;
  fw=fv=fx=(*f)(x,&dx,localdata);
  dw=dv=dx;
  for (iter=1;iter<=ITMAX;iter++) {
    xm=0.5*(a+b);
    tol1=tol*fabs(x)+ZEPS;
    tol2=2.0*tol1;
    if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
      *xmin=x;
      return fx;
    }
    if (fabs(e) > tol1) {
      d1=2.0*(b-a);
      d2=d1;
      if (dw != dx)  d1=(w-x)*dx/(dx-dw);
      if (dv != dx)  d2=(v-x)*dx/(dx-dv);
      u1=x+d1;
      u2=x+d2;
      ok1 = (a-u1)*(u1-b) > 0.0 && dx*d1 <= 0.0;
      ok2 = (a-u2)*(u2-b) > 0.0 && dx*d2 <= 0.0;
      olde=e;
      e=d;
      if (ok1 || ok2) {
	if (ok1 && ok2)
	  d=(fabs(d1) < fabs(d2) ? d1 : d2);
	else if (ok1)
	  d=d1;
	else
	  d=d2;
	if (fabs(d) <= fabs(0.5*olde)) {
	  u=x+d;
	  if (u-a < tol2 || b-u < tol2)
	    d=SIGN(tol1,xm-x);
	} else {
	  d=0.5*(e=(dx >= 0.0 ? a-x : b-x));
	}
      } else {
	d=0.5*(e=(dx >= 0.0 ? a-x : b-x));
      }
    } else {
      d=0.5*(e=(dx >= 0.0 ? a-x : b-x));
    }
    if (fabs(d) >= tol1) {
      u=x+d;
      fu=(*f)(u,&du,localdata);
    } else {
      u=x+SIGN(tol1,d);
      fu=(*f)(u,&du,localdata);
      if (fu > fx) {
	*xmin=x;
	return fx;
      }
    }
    if (fu <= fx) {
      if (u >= x) a=x; else b=x;
      MOV3(v,fv,dv, w,fw,dw)
	MOV3(w,fw,dw, x,fx,dx)
	MOV3(x,fx,dx, u,fu,du)
	} else {
      if (u < x) a=u; else b=u;
      if (fu <= fw || w == x) {
	MOV3(v,fv,dv, w,fw,dw)
	  MOV3(w,fw,dw, u,fu,du)
	  } else if (fu < fv || v == x || v == w) {
	MOV3(v,fv,dv, u,fu,du)
	  }
    }
  }
  // nrerror("Too many iterations in routine JDBRENT");
  return 0.0;
}

#undef ITMAX
#undef ZEPS
#undef SIGN
#undef MOV3

#if 0
typedef
struct j_linmin_struct {
  int ncom;	
  double *pcom,*xicom,(*nrfunc)();
  void *localdata;
} J_LINMIN_STRUCT;

double f1dim(x, void *localdata)
double x;
{
	int j;
	double f,*xt,*vector();
	void free_vector();

	xt=vector(1,ncom);
	for (j=1;j<=ncom;j++) xt[j]=( (J_LINMIN_STRUCT *) localdata)->pcom[j]+x*( (J_LINMIN_STRUCT *) localdata)->xicom[j];
	f=(*nrfunc)(xt);
	free_vector(xt,1,ncom);
	return f;
}

#define TOL 2.0e-4

int ncom=0;	/* defining declarations */
double *pcom=0,*xicom=0,(*nrfunc)();

void j_linmin(p,xi,n,fret,func,localdata)
double p[],xi[],*fret,(*func)();
void *localdata;
int n;
{
	int j;
	double xx,xmin,fx,fb,fa,bx,ax;
	double brent(),f1dim(),*vector();
	void mnbrak(),free_vector();

	ncom=n;
	pcom=vector(1,n);
	xicom=vector(1,n);
	nrfunc=func;
	for (j=1;j<=n;j++) {
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}
	ax=0.0;
	xx=1.0;
	bx=2.0;
	j_mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f1dim,localdata);
	*fret=j_brent(ax,xx,bx,f1dim,TOL,&xmin,localdata);
	for (j=1;j<=n;j++) {
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free_vector(xicom,1,n);
	free_vector(pcom,1,n);
}

#undef TOL

#define ITMAX 200
static double sqrarg;
#define SQR(a) (sqrarg=(a),sqrarg*sqrarg)

void j_powell(p,xi,n,ftol,iter,fret,func,localdata)
double p[],**xi,ftol,*fret,(*func)();
int n,*iter;
void *localdata;
{
	int i,ibig,j;
	double t,fptt,fp,del;
	double *pt,*ptt,*xit,*vector();
	void nrerror(),free_vector();

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
		if (*iter == ITMAX) nrerror("Too many iterations in routine POWELL");
		for (j=1;j<=n;j++) {
			ptt[j]=2.0*p[j]-pt[j];
			xit[j]=p[j]-pt[j];
			pt[j]=p[j];
		}
		fptt=(*func)(ptt,localdata);
		if (fptt < fp) {
			t=2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR(fp-fptt);
			if (t < 0.0) {
			        j_linmin(p,xit,n,fret,func,localdata);
				for (j=1;j<=n;j++) xi[j][ibig]=xit[j];
			}
		}
	}
}

#undef ITMAX
#undef SQR


#endif
