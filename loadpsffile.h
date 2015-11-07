/* array values for PSF data */
#define ENERG_LO 0
#define ENERG_HI 1
#define CTHETA_LO 2
#define CTHETA_HI 3
#define NCORE 4
#define NTAIL 5
#define SCORE 6
#define STAIL 7
#define GCORE 8
#define GTAIL 9
#define FCORE 10
#define NORMCORE 11
#define NORMTAIL 12
#define FACTORCORE 13
#define FACTORTAIL 14
#define RMAX2 15
#define NPSF_DATA 16

#define rmax2_funk(PSFDS,CONV_TYPE,MU,ENERGY) (PSFDS.psfdata[CONV_TYPE][RMAX2][(int) ((MU-PSFDS.mumin)/PSFDS.mustep)*PSFDS.nE + (int) ((log10(ENERGY)-PSFDS.lemin)/PSFDS.lestep)])

typedef 
struct psf_data_struct {
  float ***psfdata, f_cutoff, mustep, lestep, mumin, lemin;
  int nE, nMu;
} PSF_DATA_STRUCT;

double SPE_squared_front(double X);
double SPE_squared_back(double X);
double ickingfunk(double x2);
double ikingfunk(double x2);

double psfi(double r2, double energy, double ctheta, int conv_type, PSF_DATA_STRUCT *psfds, double *fraction);
void freepsdfdata(PSF_DATA_STRUCT *psfds);
int loadpsffile(char *passfile, PSF_DATA_STRUCT *psfds, float f_cutoff);

