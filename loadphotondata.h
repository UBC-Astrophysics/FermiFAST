
#define SAFEFREE(X) if ((X)!=NULL) free((void *) (X));X=NULL



/* array values for photon data */

/* we only keep the first five */
#define ENERGY 0
#define COSTHETA 1
#define DIFRSP_GAL 2
#define DIFRSP_ISO 3
#define EFFAREAT 4
#define EFFAREA 5
#define NPHOTON_LOADDATA 4
#define NPHOTON_DATA 6

#define THETA 1
#define RA 2
#define DEC 3

#define EFFAREAMIN 1e-5
#define EFFAREATMIN 1e-9

#if 0
typedef 
struct photon_data_struct {
  float *photondata[NPHOTON_DATA];
  double *data=NULL;
  int ntot=0, d=4;
  double  e0=400, e1=1e32, energy_min=1e32;
} PHOTON_DATA_STRUCT; 

int loadphotondata(char *filename, char *passfile, PHOTON_DATA_STRUCT *photds);
#else
int loadphotondata(char *filename, char *passfile);
#endif

