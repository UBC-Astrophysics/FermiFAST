/* array values for livetime cube */


typedef 
struct ltcube_data_struct {
  float *cosbins, *cthetamin, *cthetamax;
  char ordering[80], coordsys[80],thetabin[80];
  int nMu, nside, firstpix, lastpix, ncosbins, *aeffmubin;
} LTCUBE_DATA_STRUCT;

void freeltcube(LTCUBE_DATA_STRUCT *lfcubeds);
int loadltcube(char *filename, LTCUBE_DATA_STRUCT *lfcubeds);
