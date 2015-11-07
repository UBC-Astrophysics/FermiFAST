#include <math.h>
#include <stdio.h>
#include "loadpsffile.h"
#include "fitstable.h"

#define PRECISION 1e-4    /* solution of integrated PSF */

/* Definitions to calculate the Fermi PSF */
/* Units are radians squared */

double minus2beta, c0front2, c1front2, c0back2, c1back2;

char psf_colname[NPSF_DATA][12]={"ENERG_LO","ENERG_HI",
				 "CTHETA_LO","CTHETA_HI",
				 "NCORE","NTAIL","SCORE",
				 "STAIL","GCORE","GTAIL",
				 "FCORE",
				 "NORMCORE","NORMTAIL",
				 "FACTORCORE","FACTORTAIL",
				 "RMAX2"};


extern int debug;

/* variable for finding radius with enclosed energy */

double ntail_g, score_g, gcore_g, stail_g, gtail_g, fcore_g, 
  factorcore_g, factortail_g, value_g;

double
SPE_squared_front(double X) {
  return (pow(((X)/100),minus2beta)*c0front2+c1front2);
}

double 
SPE_squared_back(double X) {
  return (pow(((X)/100),minus2beta)*c0back2+c1back2);
}


double
ickingfunk(double x2) {
  return(fcore_g*pow(1.0+x2/factorcore_g,1.0-gcore_g)+(1-fcore_g)*pow(1.0+x2/factortail_g,1.0-gtail_g));
}

double
ikingfunk(double x2) {
  return(1-ickingfunk(x2));
}

double f(double x2) {
  return(ickingfunk(x2)-value_g);
}


double psfi(double r2, double energy, double ctheta, int conv_type, PSF_DATA_STRUCT *psfds, double *fraction) {
  double res, dumc, dumt;
  float ***psfdata=psfds->psfdata;
  int index=(int) ((ctheta-psfds->mumin)/psfds->mustep)*psfds->nE + (int) ((log10(energy)-psfds->lemin)/psfds->lestep);
  double x2, area, hold;

  x2=r2/(area=(conv_type ? SPE_squared_back(energy) : SPE_squared_front(energy)));

  res=((pow((dumc=1.0+x2/psfdata[conv_type][FACTORCORE][index]),
	      -psfdata[conv_type][GCORE][index])*
	  psfdata[conv_type][NORMCORE][index]+
	  pow(dumt=(1.0+x2/psfdata[conv_type][FACTORTAIL][index]),
	      -psfdata[conv_type][GTAIL][index])*
	  psfdata[conv_type][NORMTAIL][index])/area);

  if (fraction) {
    fcore_g=psfdata[conv_type][FCORE][index];
    *fraction=fcore_g*pow(dumc,1-psfdata[conv_type][GCORE][index])+(1-fcore_g)*pow(dumt,1-psfdata[conv_type][GTAIL][index]);
  }
  return(res);

}

double secant( double xA, double xB, double(*f)(double) )
{
    double e = 1.0e-12;
    double fA, fB;
    double d;
    int i;
    int limit = 50;
 
    fA=(*f)(xA);
    for (i=0; i<limit; i++) {
        fB=(*f)(xB);
        d = (xB - xA) / (fB - fA) * fB;
        if (fabs(d) < e) 
            break;
        xA = xB;
        fA = fB;
        xB -= d;
    }
    if (i==limit) {
        printf("Function is not converging near (%7.4f,%7.4f).\n", xA,xB);
        return -99.0;
    }
    return xB;
}    
 
double root(double fitvalue) {
    double step = 1.1;
    double e = 1.0e-12;
    double x = 0.1;		// just so we use secant method
    double xx, value;
 
    int s = (f(x)> 0.0);

    
    if (debug>3)  printf("# gcore= %g gtail= %g factorcore= %g factortail= %g\n",gcore_g,gtail_g,factorcore_g,factortail_g);
    value_g=fitvalue;

 
    while (x < 1e9) {
        value = f(x);
	if (debug>3) printf("%g %g %g\n",x,value,ickingfunk(x));
        if (fabs(value) < e) {
	  if (debug >3) printf("Root found at x= %12.9f\n", x);
	    return x;
        }
        else if ((value > 0.0) != s) {
            xx = secant(x/step, x,&f);
            if (xx != -99.0)   // -99 meaning secant method failed
	      return xx;
            else
	      return xx;
        }
        x *= step;
    }
    return x;
 }

void
freepsdfdata(PSF_DATA_STRUCT *psfds) {
  int i,k;
  for (k=0;k<2;k++) {
    for (i=0;i<NPSF_DATA;i++) {
      free((void *) psfds->psfdata[k][i]);
    }
    free((void *) psfds->psfdata[k]);
  }
  free((void *) psfds->psfdata);
}

int
loadpsffile(char *passfile, PSF_DATA_STRUCT *psfds, float f_cutoff) {
  char filename[2][255]={"front.fits","back.fits"};
  char buffer[255];
  int i, j, f, ncurr;
  float *psfscale;
  fitstable_t* tab;
  qfits_header* hdr;
  tfits_type flt = fitscolumn_float_type();
  float ***psfdata;
  int nE, nMu;

  if ( !(psfdata=(float ***) calloc(2,sizeof(float **))) ) {
    printf("# Cannot allocate psfdata in loadpsffile() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }

  if ( !(psfdata[0]=(float **) calloc(NPSF_DATA,sizeof(float *))) ) {
    printf("# Cannot allocate psfdata[0] in loadpsffile() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }

  if ( !(psfdata[1]=(float **) calloc(NPSF_DATA,sizeof(float *))) ) {
    printf("# Cannot allocate psfdata[1] in loadpsffile() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }


  psfds->psfdata=psfdata;
  psfds->f_cutoff=f_cutoff;

  if (debug>2) {
    printf("      ");
    for (i=0;i<NPSF_DATA;i++) {
      printf(" %10s",psf_colname[i]);
    }
    printf("\n");
  }

  for (f=0;f<2;f++) {
    sprintf(buffer,"psf_%s_%s",passfile,filename[f]);
    printf("# loading PSF file: %s\n",buffer);
    if ( (tab = fitstable_open(buffer))==NULL) {
      printf("# Cannot open %s in loadpsffile() %s:%d\n",buffer,__FILE__,__LINE__);
      return -1;
    }
    hdr = fitstable_get_primary_header(tab);
    ncurr = fitstable_nrows(tab);
    for (i=ENERG_LO;i<=GTAIL;i++) {
      if ( (psfdata[f][i]= (float *) fitstable_read_column_array(tab, psf_colname[i], flt))==NULL) {
	printf("# Cannot file column %s (file=%s) in loadpsffile() %s:%d\n",psf_colname[i],buffer,__FILE__,__LINE__);
	fitstable_close(tab);
	return -1;
      }
    }
    int D = fitstable_get_array_size(tab, psf_colname[NTAIL]);
    psfds->nE = nE = fitstable_get_array_size(tab, psf_colname[ENERG_LO]);
    psfds->nMu = nMu = fitstable_get_array_size(tab, psf_colname[CTHETA_LO]);
    for (i=FCORE;i<=RMAX2;i++) {
      if (!(psfdata[f][i]=(float *) calloc(D,sizeof(float)))) {
	printf("# Cannot allocate psfdata[%d][%d] in loadpsffile() %s:%d\n",f,i,__FILE__,__LINE__);
	return -1;
      }
    }

    for (j=0;j<D;j++) {
      if (debug>2) printf("[%2d] %2d",f,j);
      score_g=psfdata[f][SCORE][j];
      gcore_g=psfdata[f][GCORE][j];
      stail_g=psfdata[f][STAIL][j];
      gtail_g=psfdata[f][GTAIL][j];
      psfdata[f][FACTORTAIL][j]=factortail_g=gtail_g*stail_g*stail_g*2;
      psfdata[f][FACTORCORE][j]=factorcore_g=gcore_g*score_g*score_g*2;
      fcore_g=stail_g/score_g;
      psfdata[f][FCORE][j]=fcore_g=1/(1+psfdata[f][NTAIL][j]*fcore_g*fcore_g);
      psfdata[f][NORMCORE][j]=fcore_g/(2*M_PI*score_g*score_g)*(1-1/gcore_g);
      psfdata[f][NORMTAIL][j]=(1-fcore_g)/(2*M_PI*stail_g*stail_g)*(1-1/gtail_g); ;
      psfdata[f][RMAX2][j]=root(f_cutoff);
      if (debug>2) {
	for (i=0;i<2;i++) {
	  printf(" %10g",log10(psfdata[f][i][j%nE]));
	}
	for (i=2;i<4;i++) {
	  printf(" %10g",psfdata[f][i][(j/nE)%nMu]);
	}
	for (i=4;i<NPSF_DATA;i++) {
	  printf(" %10g",psfdata[f][i][j]);
	}
	printf(" %10g\n",ickingfunk(psfdata[f][RMAX2][j]));
      }
    }
    if (f==0) {
      psfds->lemin=log10(psfdata[f][ENERG_LO][0]);
      psfds->lestep=log10(psfdata[f][ENERG_HI][0])-log10(psfdata[f][ENERG_LO][0]);
      psfds->mumin=psfdata[f][CTHETA_LO][0];
      psfds->mustep=psfdata[f][CTHETA_HI][0]-psfdata[f][CTHETA_LO][0];
      fitstable_open_next_extension(tab);
      hdr = fitstable_get_primary_header(tab);
      ncurr = fitstable_nrows(tab);
      if ( (psfscale= (float *) fitstable_read_column_array(tab, "PSFSCALE", flt))==NULL) {
	printf("# Cannot read PSFSCALE (file=%s) in loadpsffile() %s:%d\n",buffer,__FILE__,__LINE__);
	fitstable_close(tab);
	return -1;
      }
      c0front2=psfscale[0]*psfscale[0];
      c1front2=psfscale[1]*psfscale[1];
      c0back2=psfscale[2]*psfscale[2];
      c1back2=psfscale[3]*psfscale[3];
      minus2beta=2*psfscale[4];
      if (debug>1) { printf("# %g %g %g %g %g\n",c0front2,c1front2,c0back2,c1back2,minus2beta); }
    }
    fitstable_close(tab);
  }
  free((void *) psfscale);
  return 0;
}
