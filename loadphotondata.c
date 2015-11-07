#include "fitstable.h"
#include <stdio.h>
#include <stdlib.h>
#include "loadpsffile.h"
#include "loadeffarea.h"
#include "loadltcube.h"
#include "calcefft.h"
#include "loadphotondata.h"
#include <math.h>

extern int ntot, d, debug;
extern double *data, e0, e1, energy_min;
extern float *photondata[NPHOTON_DATA];
extern double *photontime;
extern PSF_DATA_STRUCT psfds;
extern AEFF_DATA_STRUCT aeffds;
extern LTCUBE_DATA_STRUCT ltcubeds;

char photon_colname[NPHOTON_LOADDATA][12]={"ENERGY","THETA","RA","DEC"};

#ifndef __AVAILABILITY__
void
__sincospi(double ang, double *sinval, double *cosval) {
  ang*=M_PI;
  *sinval=sin(ang);
  *cosval=cos(ang);
}
#endif

int
alloc_globals(int nfinal) {
  int i;
  if (ntot==0) {
    if  ( ( data=(double *) calloc(
				   nfinal*d,sizeof(double))) == NULL) {
      printf("# data is NULL at calloc ntot=%d ncurr=%d d=%d in alloc_globals() %s:%d\n",
	     ntot,nfinal-ntot,d,__FILE__,__LINE__);
      return -1;
    }
    if  ( ( photontime=(double *) calloc(
				   nfinal,sizeof(double))) == NULL) {
      printf("# photontime is NULL at calloc ntot=%d ncurr=%d d=%d in alloc_globals() %s:%d\n",
	     ntot,nfinal-ntot,d,__FILE__,__LINE__);
      return -1;
    }
    for (i=0;i<NPHOTON_DATA;i++) {
      if  ( ( photondata[i]=(float *) calloc(
					     nfinal,sizeof(float))) == NULL) {
	printf("# photondata is NULL at calloc ntot=%d ncurr=%d i=%d in alloc_globals() %s:%d\n",
	       ntot,nfinal-ntot,i,__FILE__,__LINE__);
	return -1;
      }
    }
  } else {
    if  ( ( data=(double *) realloc((void *) data,
				    nfinal*d*sizeof(double))) == NULL) {
      printf("# data is NULL at realloc ntot=%d ncurr=%d d=%d in alloc_globals() %s:%d\n",
	     ntot,nfinal-ntot,d,__FILE__,__LINE__);
      return -1;
    }
    if  ( ( photontime=(double *) realloc((void *) photontime,
				    nfinal*sizeof(double))) == NULL) {
      printf("# photontime is NULL at realloc ntot=%d ncurr=%d d=%d in alloc_globals() %s:%d\n",
	     ntot,nfinal-ntot,d,__FILE__,__LINE__);
      return -1;
    }
    for (i=0;i<NPHOTON_DATA;i++) {
      if  ( ( photondata[i]=(float *) realloc((void *) photondata[i],
				    nfinal*sizeof(float))) == NULL) {
	printf("# photondata is NULL at realloc ntot=%d ncurr=%d i=%d in alloc_globals() %s:%d\n",
	       ntot,nfinal-ntot,i,__FILE__,__LINE__);
	return -1;
      }
    }
  }
  return 0;
}

int
loadphotondata(char *filename, char *passfile) {
  fitstable_t* tab;
  qfits_header* hdr;
  tfits_type flt = fitscolumn_float_type(), chtype = fitscolumn_char_type(), dbltype=fitscolumn_double_type();
  float *localphotondata[NPHOTON_LOADDATA], *galdisrsp=NULL, *isodisrsp=NULL;
  double *localphotontime;
  char *conversion_type=NULL;
  char buffer[255];
  int i, j, ncurr, ndiff, nresp=0, conv_type, psf_cnt, psf_ind;
  double rmax2, ehold, rad, r2hold;

  if ( (tab = fitstable_open(filename))==NULL) {
    printf("# Cannot open %s in loadphotondata() %s:%d\n",filename,__FILE__,__LINE__);
    return -1;
  }
  hdr = fitstable_get_primary_header(tab);
  hdr = fitstable_get_header(tab);
  ncurr = fitstable_nrows(tab);
  if (debug) printf("# filename= %s ncurr= %d\n",filename,ncurr);
  /* load in the energy, theta, ra and dec */
  for (i=0;i<NPHOTON_LOADDATA;i++) {
    if ( (localphotondata[i]= 
	  (float *) fitstable_read_column_array(tab, 
						photon_colname[i], flt))==NULL) {
      printf("# vector for %s is NULL in loadphotondata() %s:%d\n",photon_colname[i],__FILE__,__LINE__);
	for (j=0;j<i;j++) {
	  SAFEFREE( localphotondata[i]);
	}
	fitstable_close(tab);
	return -1;
      }
  }

  /* load in conversion type */
  if ( (conversion_type= (char *) fitstable_read_column(tab, "CONVERSION_TYPE", chtype))==NULL) {
    printf("# array for conversion_type is NULL in loadphotondata() %s:%d\n",__FILE__,__LINE__);
    for (i=0;i<NPHOTON_LOADDATA;i++) {
      SAFEFREE( localphotondata[i]);
    }
    fitstable_close(tab);
    return -1;
  }

  /* load in photon arrival time */
  if ( (localphotontime= (double *) fitstable_read_column(tab, "TIME", dbltype))==NULL) {
    printf("# array for localphotontime is NULL in loadphotondata() %s:%d\n",__FILE__,__LINE__);
    for (i=0;i<NPHOTON_LOADDATA;i++) {
      SAFEFREE( localphotondata[i]);
    }
    SAFEFREE( conversion_type);
    fitstable_close(tab);
    return -1;
  }

  /* load in the diffuse response functions that match the passfile */
  ndiff = qfits_header_getint(hdr, "NDIFRSP",-1);
  for (i=0;i<ndiff;i++) {
    sprintf(buffer,"DIFRSP%d",i);
    char *respname = qfits_header_getstr(hdr, buffer);
    if (debug) printf("# %d %s = %s\n",i,buffer,respname);
    if (strcasestr(respname,passfile)) {
      if (strcasestr(respname,"gll")) {
	if (debug) printf("# galactic is %s\n",buffer);
	if ( (galdisrsp= 
	  (float *) fitstable_read_column_array(tab, 
						buffer, flt))==NULL) {
	  printf("# array for %s is NULL in loadphotondata() %s:%d\n",buffer,__FILE__,__LINE__);
	  for (j=0;j<i;j++) {
	    SAFEFREE( localphotondata[i]);
	  }
	  SAFEFREE( conversion_type);
	  SAFEFREE(localphotontime);
	  SAFEFREE( galdisrsp);
	  SAFEFREE( isodisrsp);
	  fitstable_close(tab);
	  return -1;
	}
	nresp++;
      }
      if (strcasestr(respname,"iso")) {
	if (debug) printf("# iso is %s\n",buffer);
	if ( (isodisrsp= 
	  (float *) fitstable_read_column_array(tab, 
						buffer, flt))==NULL) {
	  printf("# array for %s is NULL in loadphotondata() %s:%d\n",buffer,__FILE__,__LINE__);
	  for (j=0;j<i;j++) {
	    SAFEFREE( localphotondata[i]);
	  }
	  SAFEFREE( conversion_type);
	  SAFEFREE(localphotontime);
	  SAFEFREE( galdisrsp);
	  SAFEFREE( isodisrsp);
	  fitstable_close(tab);
	  return -1;
	}
	nresp++;
      }
    }
  }

  if (nresp<2) {
    printf("Could not find the matching response functions in loadphotondata() %s:%d.\n",__FILE__,__LINE__);
    return -1;
  }

  if (alloc_globals(ncurr+ntot)) {
      SAFEFREE(localphotontime);
      SAFEFREE(conversion_type);
      SAFEFREE(data);
      for (i=0;i<NPHOTON_LOADDATA;i++) {
	SAFEFREE(localphotondata[i]);
      }
    return -1;
  }
  fitstable_close(tab);

  j=0;
  for (i=0;i<ncurr;i++) {
    ehold=localphotondata[ENERGY][i];
    if (ehold>=e0 && ehold<=e1) {
      double ra, dec;
      __sincospi((ra=localphotondata[RA][i])/180.0,data+d*(j+ntot)+1,data+d*(j+ntot));
      __sincospi((dec=localphotondata[DEC][i])/180.0,data+d*(j+ntot)+2,&rad);
      data[d*(ntot+j)]*=rad;
      data[d*(ntot+j)+1]*=rad;
      if (ehold<energy_min) energy_min=ehold;
      /* calculate RMAX2 for this photon */
      conv_type=conversion_type[i];
      rmax2=photondata[COSTHETA][j+ntot]=cos(localphotondata[THETA][i]*M_PI/180.0);
      if (debug>3) printf("# costheta= %g\n",rmax2);
#if 1
      /* fast way --- evenly spaced in log energy and costheta -- no interpolation */
      //      rmax2=psfds.psfdata[conv_type][RMAX2][(int) ((rmax2-psfds.mumin)/psfds.mustep)*psfds.nE + (int) ((log10(ehold)-psfds.lemin)/psfds.lestep)];
      rmax2=rmax2_funk(psfds,conv_type,rmax2,ehold);
#else      
      /* slow way --- find the right bin -- no interpolation */
      for (psf_cnt=0;psf_cnt<psfds.nE;psf_cnt++) {
	if (ehold>psfds.psfdata[conv_type][ENERG_LO][psf_cnt] && ehold<=psfds.psfdata[conv_type][ENERG_HI][psf_cnt]) {
	  psf_ind=psf_cnt;
	  break;
	}
      }
      for (psf_cnt=0;psf_cnt<psfds.nMu;psf_cnt++) {
	if (rmax2>psfds.psfdata[conv_type][CTHETA_LO][psf_cnt] && rmax2<=psfds.psfdata[conv_type][CTHETA_HI][psf_cnt]) {
	  psf_ind+=psf_cnt*psfds.nE;
	  break;
	}
      }
      rmax2=psfds.psfdata[conv_type][RMAX2][psf_ind];
#endif
      if (debug>3) printf("# rmax2= %g\n",rmax2);
      if (conv_type) {  
	/* back conversion */
	r2hold=SPE_squared_back(ehold)*rmax2;
	data[d*(ntot+j)+3]=-(r2hold>4 ? 4 : r2hold);
      } else {
	/* front conversion */
	r2hold=SPE_squared_front(ehold)*rmax2;
	data[d*(ntot+j)+3]=(r2hold>4 ? 4 : r2hold);
      }
      if (debug>3) printf("# %g rmax2= %g %g %g\n",ehold,data[d*(ntot+j)+3],SPE_squared_front(ehold),localphotondata[THETA][i]);

      photondata[ENERGY][j+ntot]=ehold;
      photondata[DIFRSP_GAL][j+ntot]=galdisrsp[i];
      photondata[DIFRSP_ISO][j+ntot]=isodisrsp[i];
      photondata[EFFAREA][j+ntot]=aeffds.aeffdata[conv_type][AEFF_EFF_AREA]
	[(int) ((photondata[COSTHETA][j+ntot]-aeffds.mumin)/aeffds.mustep)*aeffds.nE + (int) ((log10(ehold)-aeffds.lemin)/aeffds.lestep)];
      /* calculate the effective area integral */
      photondata[EFFAREAT][j+ntot]=calcefft(ra,dec,ehold,&aeffds,&ltcubeds);
      photontime[j+ntot]=localphotontime[i];
      j++;
    }
  }

  for (i=0;i<NPHOTON_LOADDATA;i++) {
    SAFEFREE(localphotondata[i]);
  }
  SAFEFREE( galdisrsp);
  SAFEFREE( isodisrsp);
  SAFEFREE(conversion_type);
  SAFEFREE(localphotontime);

  ntot+=j;
  if (debug) printf("# ntot= %d j= %d\n",ntot,j);
  if (j!=ncurr) {
    if (alloc_globals(ncurr+ntot)) {
      SAFEFREE(data);
      for (i=0;i<NPHOTON_LOADDATA;i++) {
	SAFEFREE(localphotondata[i]);
      }
      return -1;
    }
  }    

  return 0;
}
