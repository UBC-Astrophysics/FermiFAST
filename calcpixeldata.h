#define CALC_SPECTRUM 1
#define CALC_PERCENTILE 2
#define CALC_POWFIT 4
#define OUTPUT_RADIAL 8

#define WEIGHTMIN 1e-3

double
calcpixeldata(u32 *indexarray, double *data, float *photondata[], 
	      double *photontime,
	      double oneminusfcutoff, double bfactor,
	      PSF_DATA_STRUCT *psfds, 
	      int nres, double *pt, double maxsqr, int healpixlevel, 
	      int i, int d, int debug, int calcmode, double ra, double dec, char *extrastring);

double
minpixeldata(u32 *indexarray, double *data, float *photondata[], 
	      double *photontime,
	      double oneminusfcutoff, double bfactor,
	      PSF_DATA_STRUCT *psfds, 
	      int nres, double *pt, double maxsqr, int healpixlevel, 
	     int i, int d, int debug, int calcmode, double *ra, double *dec);
void printpixelheader();
void printinfocolumns();
