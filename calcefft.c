#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "healpix.h"
#include "loadeffarea.h"
#include "loadltcube.h"
#include "calcefft.h"

double calcefft(double ra, double dec, double energy, AEFF_DATA_STRUCT *aeffds, LTCUBE_DATA_STRUCT *ltcds) {
  float *lfcosbins, *aeffarr,nside=ltcds->nside;
  int *aeffmubin, i, k, ebin, nMu=ltcds->nMu,healpixval;
  double res;

  aeffmubin=ltcds->aeffmubin;
  if (aeffmubin[0]==-99)  {
    for (i=0;i<nMu;i++) {
      float ctheta=(ltcds->cthetamin[i]+ltcds->cthetamax[i])*0.5;
      aeffmubin[i]=(int) ((ctheta-aeffds->mumin)/aeffds->mustep)*aeffds->nE;
    }
  }

  ebin= (int) ((log10(energy)-aeffds->lemin)/aeffds->lestep);
  healpixval=healpix_xy_to_nested(radecdegtohealpix(ra, dec, nside), nside);

  lfcosbins=ltcds->cosbins+healpixval*ltcds->nMu;

  res=0;
  for (k=0;k<2;k++) {
    aeffarr=aeffds->aeffdata[k][AEFF_EFF_AREA]+ebin;
    for (i=0;i<nMu;i++) {
      if (aeffmubin[i]>0 && lfcosbins[i]>0) {
	res+=aeffarr[aeffmubin[i]]*lfcosbins[i];
      }
    }
  }

  return res;
}
