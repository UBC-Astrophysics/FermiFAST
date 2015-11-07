#include <stdio.h>
#include <stdlib.h>
#include "j_minimizer.h"

double *vector(int nl,int nh)
{
	double *v;

	v=(double *)malloc((unsigned) (nh-nl+1)*sizeof(double));
	if (!v) { printf("allocation failure in vector()"); return v; }
	return v-nl;
}

double **matrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	double **m;

	m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
	if (!m) { printf("allocation failure 1 in matrix()"); return m; }
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(double *) malloc((unsigned) (nch-ncl+1)*sizeof(double));
		if (!m[i]) { printf("allocation failure 2 in matrix()"); return 0; }
		m[i] -= ncl;
	}
	return m;
}


void free_vector(double *v,int nl,int nh)
{
	free((char*) (v+nl));
}



void free_matrix(double **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}
