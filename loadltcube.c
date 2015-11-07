#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../astrometry.net-0.46/util/fitstable.h"
#include "loadltcube.h"

extern int debug;

void freeltcube(LTCUBE_DATA_STRUCT *lfcubeds) {
  free((void *) lfcubeds->cosbins);
  free((void *) lfcubeds->cthetamin);
  free((void *) lfcubeds->cthetamax);
  free((void *) lfcubeds->aeffmubin);
}

int loadltcube(char *filename, LTCUBE_DATA_STRUCT *ltcubeds) {
  fitstable_t* tab;
  qfits_header* hdr;
  tfits_type flt = fitscolumn_float_type();
  
  if ( (tab = fitstable_open(filename))==NULL) {
    printf("# Cannot open %s in loadltcube() %s:%d\n",filename,__FILE__,__LINE__);
    return -1;
  }
  fitstable_open_next_extension(tab);
  hdr = fitstable_get_header(tab);
  strncpy(ltcubeds->ordering,qfits_header_getstr(hdr, "ORDERING"),sizeof(ltcubeds->ordering));
  strncpy(ltcubeds->coordsys,qfits_header_getstr(hdr, "COORDSYS"),sizeof(ltcubeds->coordsys));
  strncpy(ltcubeds->thetabin,qfits_header_getstr(hdr, "THETABIN"),sizeof(ltcubeds->thetabin));

  ltcubeds->nside=qfits_header_getint(hdr,"NSIDE",-1);
  ltcubeds->firstpix=qfits_header_getint(hdr,"FIRSTPIX",-1);
  ltcubeds->lastpix=qfits_header_getint(hdr,"LASTPIX",-1);
  if (debug) { printf("ORDERING= %s; COORDSYS= %s; THETABIN= %s; NSIDE= %d FIRSTPIX= %d LASTPIX= %d\n",
		      ltcubeds->ordering,
		      ltcubeds->coordsys,
		      ltcubeds->thetabin,
		      ltcubeds->nside,
		      ltcubeds->firstpix,
		      ltcubeds->lastpix);
  }


  /* load in livetime cube data */
  ltcubeds->nMu = fitstable_get_array_size(tab, "COSBINS");
  ltcubeds->ncosbins = fitstable_nrows(tab);
  printf("# nrows= %d\n",ltcubeds->ncosbins);
  if ( (ltcubeds->cosbins= (float *) fitstable_read_column_array(tab, "COSBINS", flt))==NULL) {
    printf("# cannot load vector COSBINS from %s in loadltcude() %s:%d\n",filename,__FILE__,__LINE__);
    fitstable_close(tab);
    return -1;
  }

  fitstable_open_next_extension(tab);

  if ( (ltcubeds->cthetamin= (float *) fitstable_read_column(tab, "CTHETA_MIN", flt))==NULL) {
    printf("# cannot load vector CTHETA_MIN from %s in loadltcude() %s:%d\n",filename,__FILE__,__LINE__);
    fitstable_close(tab);
    return -1;
  }

  if ( (ltcubeds->cthetamax= (float *) fitstable_read_column(tab, "CTHETA_MAX", flt))==NULL) {
    printf("# cannot load vector CTHETA_MAX from %s in loadltcude() %s:%d\n",filename,__FILE__,__LINE__);
    fitstable_close(tab);
    return -1;
  }

  if ( (ltcubeds->aeffmubin = (int *) calloc(ltcubeds->nMu,sizeof(int)))==NULL) {
    printf("# cannot allocate vector AEFFMUBIN in loadltcude() %s:%d\n",__FILE__,__LINE__);
    fitstable_close(tab);
    return -1;
  }

  ltcubeds->aeffmubin[0]=-99;

  if (debug>2) {
    int i, j;
    printf("# nMu= %d\n",ltcubeds->nMu);
    for (i=0;i<ltcubeds->nMu;i++) {
      printf("%3d %10g %10g\n",i,ltcubeds->cthetamin[i],ltcubeds->cthetamax[i]);
    }
    for (j=0;j<10;j++) {
      printf("j= %d",j);
      for (i=0;i<ltcubeds->nMu;i++) {
	printf(" %10g",ltcubeds->cosbins[j*ltcubeds->nMu+i]);
      }
      printf("\n");
    }
  }

  printf("# loaded %s\n",filename);
  return 0;
}
