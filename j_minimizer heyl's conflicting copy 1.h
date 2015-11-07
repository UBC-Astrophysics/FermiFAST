typedef
struct j_linmin_struct {
  int ncom;	
  double *pcom,*xicom,(*nrfunc)(double [], void *);
  void *localdata;
} J_LINMIN_STRUCT;

void j_mnbrak(double *ax,double *bx,double *cx,double *fa,double *fb,double *fc,
	      double (*func)(double a, void *localdata), void *localdata);
double j_dbrent(double ax,double bx,double cx,double (*f)(double, double *, void *),double tol,double *xmin, void *localdata);

double j_brent(double ax,double bx,double cx,double (*f)(double, void *),double tol,double *xmin, void *localdata);

void j_linmin( double p[],double xi[],int n,double *fret,double (*func)(double x[], void *localdata),void *localdata);

void j_powell( double p[],double **xi, int n,double ftol,int *iter,
	       double *fret,double (*func)(double [],void *), void *localdata);

void j_dfpmin(double p[],int n,double ftol,int *iter,double *fret,
	    double (*func)(double [], void *), double (*fdfunc)(double [], double *, void *), 
	    void *localdata);

double *vector(int, int);
double **matrix(int, int, int, int);
void free_vector(double *, int, int);
void free_matrix(double **, int, int, int, int);
