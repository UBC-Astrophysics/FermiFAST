#include <math.h>
#include <stdio.h>
#include "loadeffarea.h"
#include "../astrometry.net-0.46/util/fitstable.h"

char aeff_colname[NAEFF_DATA][12]={"ENERG_LO","ENERG_HI",
				   "CTHETA_LO","CTHETA_HI","EFFAREA"};

extern int debug;

void
freeeffarea(AEFF_DATA_STRUCT *aeffds) {
  int i,k;
  for (k=0;k<2;k++) {
    for (i=0;i<NAEFF_DATA;i++) {
      free((void *) aeffds->aeffdata[k][i]);
    }
    free((void *) aeffds->aeffdata[k]);
  }
  free((void *) aeffds->aeffdata);
}

int
loadeffarea(char *passfile, AEFF_DATA_STRUCT *aeffds) {
  char filename[2][255]={"front.fits","back.fits"};
  char buffer[255];
  int i, j, f, ncurr;
  fitstable_t* tab;
  qfits_header* hdr;
  tfits_type flt = fitscolumn_float_type();
  float ***aeffdata;
  int nE, nMu;

  if ( !(aeffdata=(float ***) calloc(2,sizeof(float **))) ) {
    printf("# Cannot allocate aeffdata in loadeffarea() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }

  if ( !(aeffdata[0]=(float **) calloc(NAEFF_DATA,sizeof(float *))) ) {
    printf("# Cannot allocate aeffdata[0] in loadeffarea() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }

  if ( !(aeffdata[1]=(float **) calloc(NAEFF_DATA,sizeof(float *))) ) {
    printf("# Cannot allocate aeffdata[1] in loadeffarea() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }

  aeffds->aeffdata=aeffdata;

  if (debug>2) {
    printf("      ");
    for (i=0;i<NAEFF_DATA;i++) {
      printf(" %10s",aeff_colname[i]);
    }
    printf("\n");
  }

  for (f=0;f<2;f++) {
    sprintf(buffer,"aeff_%s_%s",passfile,filename[f]);
    printf("# loading AEFF file: %s\n",buffer);
    if ( (tab = fitstable_open(buffer))==NULL) {
      printf("# Cannot open %s in loadeffarea() %s:%d\n",buffer,__FILE__,__LINE__);
      return -1;
    }
    hdr = fitstable_get_primary_header(tab);


    ncurr = fitstable_nrows(tab);
    for (i=AEFF_ENERG_LO;i<=AEFF_EFF_AREA;i++) {
      if ( (aeffdata[f][i]= (float *) fitstable_read_column_array(tab, aeff_colname[i], flt))==NULL) {
	printf("# cannot load column %s from %s in loadeffarea() %s:%d\n",aeff_colname[i],buffer,__FILE__,__LINE__);
	fitstable_close(tab);
	return -1;
      }
    }
    aeffds->nE = nE = fitstable_get_array_size(tab, aeff_colname[AEFF_ENERG_LO]);
    aeffds->nMu = nMu = fitstable_get_array_size(tab, aeff_colname[AEFF_CTHETA_LO]);
    aeffds->lemin=log10(aeffdata[f][AEFF_ENERG_LO][0]);
    aeffds->lestep=log10(aeffdata[f][AEFF_ENERG_HI][0])-log10(aeffdata[f][AEFF_ENERG_LO][0]);
    aeffds->mumin=aeffdata[f][AEFF_CTHETA_LO][0];
    aeffds->mustep=aeffdata[f][AEFF_CTHETA_HI][0]-aeffdata[f][AEFF_CTHETA_LO][0];

#if 1
    if (f==0) {
      hdr = fitstable_get_header(tab);
      strncpy(aeffds->units,qfits_header_getstr(hdr, "TUNIT5"),sizeof(aeffds->units));

      printf("# Eff area units: %s\n", aeffds->units);
    }
#endif


    if (debug>2) {
      int D = fitstable_get_array_size(tab, aeff_colname[AEFF_EFF_AREA]);
      for (j=0;j<D;j++) {
        printf("[%2d] %2d",f,j);
	for (i=0;i<2;i++) {
	  printf(" %10g",log10(aeffdata[f][i][j%nE]));
	}
	for (i=2;i<4;i++) {
	  printf(" %10g",aeffdata[f][i][(j/nE)%nMu]);
	}
	for (i=4;i<NAEFF_DATA;i++) {
	  printf(" %10g",aeffdata[f][i][j]);
	}
	printf("\n");
      }
    }
  }
  return 0;
}
