#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "healpix.h"
#include "kdtree_fits_io.h"
#include "loadpsffile.h"
#include "loadphotondata.h"
#include "j_minimizer.h"
#include "calcpixeldata.h"
#ifdef USE_CHEALPIX
#include "chealpix.h"
#endif

typedef
struct calcpixel_struct {
  u32 *indexarray;
  double *data;
  double *photontime;
  float **photondata;
  double oneminusfcutoff;
  double bfactor;
  PSF_DATA_STRUCT *psfds;
  int nres;
  double *pt;
  double maxsqr; 
  int healpixlevel,i, d, debug, calcmode;
} CALCPIXEL_STRUCT;


typedef 
struct loglike_funk_struct {
  int nres;
  float *ratioarray;
} LOGLIKE_FUNK_STRUCT;

/* just calculate the PSF negloglike */
/* for mnbrak */
double negloglike_psf_funk(double a, void *localdata) {
  double sum, oneminusa=1-a;
  float *ptr, *pdone;
  int nres, ratioarray;
  ptr=((LOGLIKE_FUNK_STRUCT *) localdata)->ratioarray;
  pdone=ptr+((LOGLIKE_FUNK_STRUCT *) localdata)->nres;
  
  sum=0;
  for (;ptr<pdone;ptr++) {
    sum-=log(a*(*ptr)+oneminusa);
  }
  return(sum);
}

/* calculate the PSF negloglike and its derivative too, not much more expensive! */
double
negloglike_psf_funkdfunk(double a, double *dloglike, void *localdata) {
  double f, sum, dsum, dum,oneminusa=1-a;
  float *ptr, *pdone;
  ptr=((LOGLIKE_FUNK_STRUCT *) localdata)->ratioarray;
  pdone=ptr+((LOGLIKE_FUNK_STRUCT *) localdata)->nres;
  dsum=sum=0;
  for (;ptr<pdone;ptr++) {
    sum-=log(dum=a*(f=*ptr)+oneminusa);
    dsum-=(f-1.0)/dum;
  }
  *dloglike=dsum;
  return(sum);
}

int
compindex(void *data, const void *i1, const void *i2) {
  float *photonenergy_g=(float *) data;
  if ( photonenergy_g[*((int *) i1)]>  photonenergy_g[*((int *) i2)] ) {
    return(1);
  } else { 
    return(-1);
  }
}

void 
printpixelheader() {
  printf("\
#    1     2      3       4        5         6         7         8        9         10         11       12       13       14       15        16       17        18        19        20        21        22       23       24      25\n\
# Healpix  N     TS     MEANR2   MEANFRAC  SIGR2    SIGFRAC    AFRAC     RA         DEC      BVALUE   NBACK  BACK_OFF ln(PTSNew) DeltaSig csource  cesource   cback    ceback     sumL0     sumL1  2(DeltaSum)  TSPSF    APSF  ln(ProbPSF)F\n");
}

void printinfocolumns() {
  printf("\
### Spectral Information Columns\n\
###   Column  1 - j          number of photon from lowest to highest energy\n \
###   Column  2 - photon_num number of photon in the event list\n\
###   Column  3 - energy     energy of photon in MeV\n\
###   Column  4 - psf        value of the PSF in (sterradians)^{-1}\n\
###   Column  5 - effarea    effective area in m^2\n\
###   Column  6 - back       background flux density in photons/MeV/cm^2/s/sterradian\n\
###   Column  7 - weight     photon weighting (for integration) in MeV\n\
###   Column  8 - r2         distance squared in radians of photon from point source position\n\
###   Column  9 - fi         spectral density of the source in photons/MeV/m^2/s\n\
###   Column 10 - ci         total cumulative spectra density photons/m^2/s\n\
###   Column 11 - cei        total cumulative spectra energy density MeV/m^2/s\n\
###   Column 12 - cbi        background cumulative spectra energy density MeV/m^2/s\n\
###   Column 13 - cbei       background cumulative spectra energy density MeV/m^2/s\n\
###   Column 14 - time       mission elapsed time\n");
}


#if 0
CALCPIXEL_STRUCT minfunk_data;

double
minfunk(double pos[]) {
#ifndef __AVAILABILITY__
    void __sincospi(double ang, double *sinval, double *cosval);
#endif
  double pt[4], rad;
  __sincospi(pos[1]/180.0,pt+1,pt);
  __sincospi(pos[2]/180.0,pt+2,&rad);
  pt[0]*=rad;
  pt[1]*=rad;
  pt[3]=0;
  return calcpixeldata(minfunk_data.indexarray,	
		       minfunk_data.data,
		       minfunk.photondata_g,
		       minfunk_data.photontime,
		       minfunk_data.oneminusfcutoff,
		       minfunk_data.bfactor,
		       minfunk_data.psfds,
		       minfunk_data.nres,
		       pt,
		       minfunk_data.maxsqr,
		       minfunk_data.healpixlevel,
		       -1,
		       minfunk_data.d,
		       minfunk_data.debug,
		       pos[1],pos[2]);

}

double
minpixeldata(u32 *indexarray, double *data, float *photondata[], 
	     double *photontime, double oneminusfcutoff, 
	      double bfactor,
	      PSF_DATA_STRUCT *psfds, 
	      int nres, double *pt, double maxsqr, int healpixlevel, 
	      int i, int d, int debug, double *ra, double *dec) {
  double pos[2], **xi;
  void powell( double p[],double **xi, int n,double ftol,int *iter,
	       double *fret,double (*func)(double []));
  double **matrix(int nrl,int nrh,int ncl,int nch);
  void  free_matrix(double **m, int nrl,int nrh,int ncl,int nch);
  int iter;
  double fret;


  minfunk_data.photondata=photondata;
  minfunk_data.indexarray=indexarray;
  minfunk_data.data=data;
  minfunk_data.photontime=photontime;
  minfunk_data.oneminusfcutoff=oneminusfcutoff;
  minfunk_data.bfactor=bfactor;
  minfunk_data.psfds=psfds;
  minfunk_data.nres=nres;
  minfunk_data.pt=pt;
  minfunk_data.maxsqr=maxsqr;
  minfunk_data.healpixlevel=healpixlevel;
  minfunk_data.i=i;
  minfunk_data.d=d;
  minfunk_data.debug=-1;

  pos[0]=*ra; pos[1]=*dec;
  xi=matrix(1,2,1,2);
  xi[1][1]=0.1; xi[1][2]=0; xi[2][1]=0; xi[2][2]=0.1;

  powell(pos-1,xi,2,1e-3,&iter,&fret,minfunk);
  *ra=pos[0]; *dec=pos[1];
  return fret;
	
  free_matrix(xi,1,2,1,2);

}
#else
double
minfunk(double pos[], void *localdata) {
#ifndef __AVAILABILITY__
    void __sincospi(double ang, double *sinval, double *cosval);
#endif
  double pt[4], rad;
  CALCPIXEL_STRUCT *minfunk_data=(CALCPIXEL_STRUCT *) localdata;
  
  __sincospi(pos[1]/180.0,pt+1,pt);
  __sincospi(pos[2]/180.0,pt+2,&rad);
  pt[0]*=rad;
  pt[1]*=rad;
  pt[3]=0;
  return calcpixeldata(minfunk_data->indexarray,	
		       minfunk_data->data,
		       minfunk_data->photondata,
		       minfunk_data->photontime,
		       minfunk_data->oneminusfcutoff,
		       minfunk_data->bfactor,
		       minfunk_data->psfds,
		       minfunk_data->nres,
		       pt,
		       minfunk_data->maxsqr,
		       minfunk_data->healpixlevel,
		       -1,
		       minfunk_data->d,
		       minfunk_data->debug, 
		       minfunk_data->calcmode,
		       pos[1],pos[2],NULL);

}

double
minpixeldata(u32 *indexarray, double *data, float *photondata[], 
	     double *photontime, double oneminusfcutoff, 
	      double bfactor,
	      PSF_DATA_STRUCT *psfds, 
	      int nres, double *pt, double maxsqr, int healpixlevel, 
	      int i, int d, int debug, int calcmode, double *ra, double *dec) {
  double pos[2], **xi;
  void powell( double p[],double **xi, int n,double ftol,int *iter,
	       double *fret,double (*func)(double []));
  int iter;
  double fret;
  CALCPIXEL_STRUCT minfunk_data;

  minfunk_data.photondata=photondata;
  minfunk_data.indexarray=indexarray;
  minfunk_data.data=data;
  minfunk_data.photontime=photontime;
  minfunk_data.oneminusfcutoff=oneminusfcutoff;
  minfunk_data.bfactor=bfactor;
  minfunk_data.psfds=psfds;
  minfunk_data.nres=nres;
  minfunk_data.pt=pt;
  minfunk_data.maxsqr=maxsqr;
  minfunk_data.healpixlevel=healpixlevel;
  minfunk_data.i=i;
  minfunk_data.d=d;
  minfunk_data.debug=debug;
  minfunk_data.calcmode=calcmode;

  pos[0]=*ra; pos[1]=*dec;
  xi=matrix(1,2,1,2);
  xi[1][1]=0.1; xi[1][2]=0; xi[2][1]=0; xi[2][2]=0.1;

  j_powell(pos-1,xi,2,1e-3,&iter,&fret,minfunk,(void *) &minfunk_data);
  *ra=pos[0]; *dec=pos[1];
  return fret;
	
  free_matrix(xi,1,2,1,2);

}
#endif

/*****************************************************************************************/
/*                                                                                       */
/* at a minimum calcpixeldata will calculate the mean value of r2 and the best fitting   */
/* combination of PSF and a flat background to the data returning the null probability   */
/* changing the value of calcmode from                                                   */
/*                                                                                       */
/* CALC_SPECTRUM | CALC_PERCENTILE | CALC_POWFIT                                         */
/*                                                                                       */
/* to zero yields this minimal functionality for a 15% speed up                          */ 
/*                                                                                       */
/*****************************************************************************************/

double
calcpixeldata(u32 *indexarray, double *data, float *photondata[],
	      double *photontime, double oneminusfcutoff, 
	      double bfactor,
	      PSF_DATA_STRUCT *psfds, 
	      int nres, double *pt, double maxsqr, int healpixlevel, 
	      int i, int d, int debug, int calcmode, 
	      double ra, double dec, 
	      char *extrastring) {

  double lnprob(double dof, double chi2), resTS, resTSPSF;
  int dimcnt, photpos, j;
  double r2,dum, weight, sumfrac, frac, sumr2, 
    sumTS, sumL1, sumL0, sumnback,
    sumspec, sumbspec, sumespec, sumbespec, 
    psfi_val, fi, psfieaifi, psfieaifiodi, diffrspi,
    r2max, effareat, afrac, bval, shold, ehold, xmin, psflike;
  float *ratioarray, *photonenergy_g;
  char *outputbuffer=NULL;
  int totallength;
#define LINELENGTH 170

  photonenergy_g=photondata[ENERGY];
  if (calcmode & CALC_SPECTRUM)  {
#ifdef NEED_QSORT_R
    void j_qsort_r(void *a, size_t n, size_t es, void *thunk, 
		   int (*cmp)(void *, const void *, const void *));

  /* Sort the indices by the energy of photons */
    j_qsort_r((void *) indexarray,nres,sizeof(u32),(void *) photonenergy_g,compindex);
#else
    qsort_r((void *) indexarray,nres,sizeof(u32),(void *) photonenergy_g,compindex);
#endif

  }

  sumnback=sumespec=sumspec=sumbespec=sumbspec=sumr2=sumfrac=sumL0=sumL1=sumTS=0;

  /* allocate the ratioarray */
  ratioarray=(float *) calloc(nres,sizeof(float));

  if ((debug>0) && (calcmode & CALC_SPECTRUM)) {
    outputbuffer=(char *) calloc((nres+1)*LINELENGTH,sizeof(char));
    if (outputbuffer) 
      sprintf(outputbuffer,"###     j photon_num   energy    psf effarea    back        weight     r2         fi         ci         cei     cbi     cebi    time\n");

  }

  for (j=nres-1;j>=0;j--) {
    r2=0;
    photpos=indexarray[j];
    ehold=photonenergy_g[photpos];
    for (dimcnt=0;dimcnt<3;dimcnt++) {
      dum=data[d*photpos+dimcnt]-pt[dimcnt];
      r2+=dum*dum;
    }
    shold=data[d*photpos+3];

    /* calculate sum of area/areamax */
    sumr2+=r2/(r2max=(maxsqr-shold*shold));

    psfi_val=psfi(r2,ehold,photondata[COSTHETA][photpos],(data[d*photpos+3] > 0 ? 0 : 1),psfds,&frac);
    if (ratioarray) ratioarray[j]=psfi_val*r2max*M_PI/oneminusfcutoff;

    if (calcmode & CALC_PERCENTILE) {
      /* calculate sum of percentiles in the PSF */
      sumfrac+=(1-frac);
    }

    if (calcmode & CALC_SPECTRUM) {
      /* calculate the weights of the photons using trapezoidal integration */
      if (j==0) {
	weight=0.5*(photonenergy_g[indexarray[1]]-ehold);
      } else if (j==nres-1) {
	weight=0.5*(ehold-photonenergy_g[indexarray[j-1]]);
      } else {
	weight=(photonenergy_g[indexarray[j+1]]-photonenergy_g[indexarray[j-1]])*0.5;
      }

      if (weight==0) weight=1e-3;
      
      /* calculate basic flux spectral density --- for zero background */
      /* units per area per time per MeV */
      fi=1/(photondata[EFFAREAT][photpos]*weight*oneminusfcutoff);
      
      /* calculate number of photons expected per unit solid angle per unit time per energy --- for zero background */
      /* same units as the diffuse response */ 
      /* units of psfi are 1/(solid angle) */
      psfieaifi=psfi_val*photondata[EFFAREA][photpos]*fi;
      sumL0+=log(psfieaifi);
      
      psfieaifiodi=psfieaifi/(diffrspi=bfactor*photondata[DIFRSP_GAL][photpos]);
      sumTS+=log(psfieaifiodi);
      
      sumnback+=1/psfieaifiodi;

      sumL1+=log(diffrspi);
      
      sumspec+=(dum=fi*weight);
      sumespec+=dum*ehold;
      sumbspec+=(dum=fi/psfieaifiodi*weight);
      sumbespec+=dum*ehold;

      if (outputbuffer) {
	sprintf(outputbuffer,
		 "%s## %9d %10d %9.2f %9.2f %6.2f %10.2e %10.2e %10.2e %10.2e %10.2e %10.2e %10.2e %10.2e %.17e\n",outputbuffer,
		 j,indexarray[j],
		 ehold,psfi_val,photondata[EFFAREAT][photpos],dum/weight,weight,r2,
		 fi,sumspec,sumespec,sumbspec,sumbespec,photontime[photpos]);
      }
    }
  }

  if (calcmode & CALC_SPECTRUM) {
    /* calculate TS statistic -- twice Delta log L */
    sumTS=2*(sumTS-nres*log(nres/sumnback)); 

    /* calculate logL0 */
    sumL0=sumL0+nres*(log(nres/sumnback)-1);

    /* calculate logL1 */
    sumL1=sumL1-nres;
  }

  /* calculate mean of r2/r2max */
  sumr2/=nres; 

  if (calcmode & CALC_PERCENTILE) {
    /* calculate mean of percentile/percentilmax */
    sumfrac/=nres*(oneminusfcutoff);

    /* calculate the fraction of photons from a point source by setting the mean percentile to 0.5 */
    afrac=(0.5-sumr2)/(sumfrac-sumr2);

    /* calculate the mean value of r^2 for the source model */
    bval=sumr2-sumfrac+0.5;
  }

  if (i>=0) {
    /* find the ra and dec of the pixel */
#ifdef USE_CHEALPIX
    pix2ang_ring(healpixlevel,i,&dec,&ra);
    dec=90-dec*180.0/M_PI;
    ra=ra*180.0/M_PI;
#else
    healpix_to_radecdeg(i,healpixlevel,0.0,0.0,&ra,&dec);
#endif
  }

  if (calcmode & CALC_SPECTRUM) {
    /* calculate the coefficient of the diff response B */
    dum=nres/sumnback*(1-afrac);
    resTS=lnprob(2*nres,sumTS);
  } else { 
    dum=0; 
  }

  /* did we successfully allocate the array? */
  if (ratioarray) {
    double ax, bx, cx, fa, fb, fc; 
    LOGLIKE_FUNK_STRUCT likefunkdata;
    bx=0.5;
    ax=0.9*bx;  cx=1.1*bx;
    likefunkdata.nres=nres;
    likefunkdata.ratioarray=ratioarray;
    j_mnbrak(&ax,&bx,&cx,&fa,&fb,&fc,negloglike_psf_funk,(void *) &likefunkdata);
    psflike=-j_dbrent(ax,bx,cx,negloglike_psf_funkdfunk,1e-3,&xmin,(void *) &likefunkdata);
    free((void *) ratioarray);
  } else {
    xmin=psflike=0;
  }

  resTSPSF=lnprob(1.0,psflike);

  if (debug>=0) {
    if (extrastring) {
      if (outputbuffer) {
	printf("%8d %4d %8.3f %8.6f %8.6f %8.6f %8.6f %8.6f %9.5f %9.5f %9.5f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %10.2e %10.2e %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %s\n%s",
	   i,nres,sumTS,sumr2,
	   sumfrac,
	 /* calculate number of standard deviations from the mean */
	   (sumr2-0.5)*sqrt(12*nres),
	   (sumfrac-0.5)*sqrt(12*nres),afrac,
	 ra,dec,bval,sumnback,dum,resTS,(sumTS-nres)/sqrt(2*nres),
	 sumspec,sumespec,sumbspec,sumbespec,
	       sumL0,sumL1,2*(sumL1-sumL0),psflike,xmin,resTSPSF,extrastring,outputbuffer);
      } else {
	printf("%8d %4d %8.3f %8.6f %8.6f %8.6f %8.6f %8.6f %9.5f %9.5f %9.5f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %10.2e %10.2e %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %s\n",
	   i,nres,sumTS,sumr2,
	   sumfrac,
	 /* calculate number of standard deviations from the mean */
	   (sumr2-0.5)*sqrt(12*nres),
	   (sumfrac-0.5)*sqrt(12*nres),afrac,
	 ra,dec,bval,sumnback,dum,resTS,(sumTS-nres)/sqrt(2*nres),
	 sumspec,sumespec,sumbspec,sumbespec,
	     sumL0,sumL1,2*(sumL1-sumL0),psflike,xmin,resTSPSF,extrastring);
      }
    } else {
      if (outputbuffer) {
	printf("%8d %4d %8.3f %8.6f %8.6f %8.6f %8.6f %8.6f %9.5f %9.5f %9.5f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %10.2e %10.2e %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n%s",
	   i,nres,sumTS,sumr2,
	   sumfrac,
	 /* calculate number of standard deviations from the mean */
	   (sumr2-0.5)*sqrt(12*nres),
	   (sumfrac-0.5)*sqrt(12*nres),afrac,
	 ra,dec,bval,sumnback,dum,resTS,(sumTS-nres)/sqrt(2*nres),
	 sumspec,sumespec,sumbspec,sumbespec,
	       sumL0,sumL1,2*(sumL1-sumL0),psflike,xmin,resTSPSF,outputbuffer);
      } else {
	printf("%8d %4d %8.3f %8.6f %8.6f %8.6f %8.6f %8.6f %9.5f %9.5f %9.5f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %10.2e %10.2e %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n",
	       i,nres,sumTS,sumr2,
	       sumfrac,
	       /* calculate number of standard deviations from the mean */
	       (sumr2-0.5)*sqrt(12*nres),
	       (sumfrac-0.5)*sqrt(12*nres),afrac,
	       ra,dec,bval,sumnback,dum,resTS,(sumTS-nres)/sqrt(2*nres),
	       sumspec,sumespec,sumbspec,sumbespec,
	       sumL0,sumL1,2*(sumL1-sumL0),psflike,xmin,resTSPSF);
      }
    }
  }
  if (outputbuffer) free((void *) outputbuffer);

  if (calcmode & OUTPUT_RADIAL) {
    return (sumr2-0.5)*sqrt(12*nres);
  } else {
    return resTSPSF;
  }

}
