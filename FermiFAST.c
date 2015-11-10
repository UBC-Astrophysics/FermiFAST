/*
This file is part of ``fermi-fast'', a suite of programs for rapidly identifying
point sources in the Fermi LAT photon event files.
Copyright (C) 2014 Jeremy Heyl <heyl@phas.ubc.ca>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include <sys/times.h>
#include <unistd.h>
#include "kdtree.h"
#include "kdtree_fits_io.h"
#if USE_CHEALPIX
#include "chealpix.h"
#define healpix_to_xyzarr(I,HEALPIXLEVEL,DX,DY,PT) (pix2vec_ring(HEALPIXLEVEL,I, PT))
#else
#include "healpix.h"
#endif

#include "fitstable.h"
#include "fitsioutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "loadpsffile.h"
#include "loadeffarea.h"
#include "loadltcube.h"
#include "calcefft.h"
#include "loadphotondata.h"
#include "calcpixeldata.h"
#define HEALPIXLEVEL 128

#define RadDegree .01745329251994329576
#define AlphaGP2000 192.85950833333333333333 // 12h 51m 26.282s                                                                                                                         
#define DeltaGP2000 0.47347878572656315300 // 27d 7m 42.01s                                                                                                                             
#define SinDeltaGP2000 0.45598511375759453457
#define CosDeltaGP2000 0.88998740217571258131
#define LNCP2000_G 122.932 // longitude of NCP 2000 in Gal                                                                                                                              

float *photondata[NPHOTON_DATA];
double *photontime;
double *data=NULL;
int ntot=0, d=4;
double  e0=10, e1=1e32, energy_min=1e32;

int debug;

PSF_DATA_STRUCT psfds;
AEFF_DATA_STRUCT aeffds;
LTCUBE_DATA_STRUCT ltcubeds;


int
main(int argc,char *argv[]) {
  struct tms first_time, start_time, second_time;
  double cpu_time_used;
  int i, j, ncurr, nleaf=32, nqueries=0, readmode=0;
  double *minval, *maxval;
  double rad;
  float *rad_scale=NULL;
  kdtree_t *kd;
  double ra, dec, sum, maxsqr=2, maxrat2, maxpercentile;
  double ehold, shold;
  char *passfile, buffer[255];

  fitstable_t* tab;
  float *ra_data=NULL, *dec_data=NULL, *energy_data=NULL, *time_data=NULL;
  qfits_header* hdr;
  tfits_type flt = fitscolumn_float_type(), chtype = fitscolumn_char_type();
  char **argptr, *outputfile=NULL, *inputfile=NULL, *conversion_type=NULL;  /* 0=Front 1=Back */ 
  int healpixlevel=HEALPIXLEVEL, healpixmin=0, healpixmax=12*HEALPIXLEVEL*HEALPIXLEVEL-1, passfile_loaded=0;
  double *dptr, *ddone, val;
  float f_cutoff=0.05, oneminusfcutoff;
  char *livetimecubefilename=NULL;
  char *filenames[1024];
  int ntot_array[1024], filecnt=0, calcmode = CALC_SPECTRUM,  treetype=KDTT_DSS;
  
  if (!(passfile=malloc(255*sizeof(char)))) {
    printf("# Could not allocate passfile in main() %s:%d\n",__FILE__,__LINE__);
    return -1;
  }

  strncpy(passfile,"P7REP_SOURCE_V15",255);
  times(&first_time);
  
  
  if (argc<2) {
    printf("Format:\n\n   %s [pass_file_option] fits_files [options]\n\n\
   where the following options must appear first before any fits files:\n\n\
   -p pass_file        Fermi analysis pass file name - default %s\n\
   -f fraction_cutoff  PSF fraction of counting region - default %g\n\n\
   -lt livetimecubefile required. Only the final one is loaded.\n\n\
   Energy cuts apply to subsequently loaded fits files.\n\n\
   -e0 min energy      [MeV] default %g\n\
   -e1 max energy      [MeV] default %g\n\n\
   The following options can appear anywhere in any order and apply from\n\
   from left to right.\n\
   -c  calcmode        set calculation mode default %d\n\
   -d  debug_level     default %d\n\
   -i  input_kd_tree   file to read kd_tree (discard position from fits files)\n\
   -o  output_kd_tree  file to write kd_tree\n\
   -hl healpixlevel    default %d\n\
   -h0 healpixmin      default %d\n\
   -h1 healpixmax      default %d\n\
   -nl nleaf           default %d\n\
   -r  readmode        default %d\n\
       0     - loop from healpixmin to healpixmax\n\
       1 (h) - read healpix from STDIN\n\
       2 (e) - read RA and DEC from STDIN (degrees)\n\
       3 (s) - read RA and DEC from STDIN (sexagesimal)\n\
       4 (g) - read l and b from STDIN (degrees)\n\
       5 (p) - read x, y, z from STDIN\n\n\
   If readmode is non-zero, you can override the setting for a line\n\
   by adding the appropriate letter at the beginning of the line\n\n\
   -tt tree_type        set tree type default %d\n\n",
	   argv[0],
	   passfile,f_cutoff,
	   e0,e1,calcmode,debug,
	   healpixlevel,
	   healpixmin,healpixmax,nleaf,readmode,treetype);
    return -1;
  }

  printf("#");
  for (argptr=argv;argptr<argv+argc;argptr++) {
    printf(" %s",*argptr);
  }
  printf("\n");

  for (argptr=argv+1;argptr<argv+argc;argptr++) {
    if (strstr(*argptr,"-hl")) {
      if (++argptr<argv+argc) {
	i=atoi(*argptr);
	if (i>0) healpixlevel=i;
      }
    } else if (strstr(*argptr,"-h0")) {
      if (++argptr<argv+argc) {
	i=atoi(*argptr);
	if (i>0) healpixmin=i;
      }
    } else if (strstr(*argptr,"-c")) {
      if (++argptr<argv+argc) {
	calcmode=atoi(*argptr);
      }
    } else if (strstr(*argptr,"-tt")) {
      if (++argptr<argv+argc) {
	treetype=atoi(*argptr);
      }
    } else if (strstr(*argptr,"-r")) {
      if (++argptr<argv+argc) {
	readmode=atoi(*argptr);
      }
    } else if (strstr(*argptr,"-h1")) {
      if (++argptr<argv+argc) {
	i=atoi(*argptr);
	if (i>0) healpixmax=i;
      }
    } else if (strstr(*argptr,"-nl")) {
      if (++argptr<argv+argc) {
	i=atoi(*argptr);
	if (i>0) nleaf=i;
      }
    } else if (strstr(*argptr,"-f")) {
      if (++argptr<argv+argc) {
	f_cutoff=atof (*argptr);
      }
    } else if (strstr(*argptr,"-d")) {
      if (++argptr<argv+argc) {
	debug=atoi (*argptr);
      }
    } else if (strstr(*argptr,"-p")) {
      if (++argptr<argv+argc) {
	passfile=*argptr;
      }
    } else if (strstr(*argptr,"-lt")) {
      if (++argptr<argv+argc) {
	livetimecubefilename=*argptr;
      }
    } else if (strstr(*argptr,"-e0")) {
      if (++argptr<argv+argc) {
	e0=atof(*argptr);
      }
    } else if (strstr(*argptr,"-e1")) {
      if (++argptr<argv+argc) {
	e1=atof(*argptr);
      }
    } else if (strstr(*argptr,"-o")) {
      if (++argptr<argv+argc) {
	outputfile=*argptr;
      }
    } else if (strstr(*argptr,"-i")) {
      if (++argptr<argv+argc) {
	inputfile=*argptr;
	SAFEFREE( data);
      }
    } else {
      /* load up the pass files and livetime file*/
      if (!passfile_loaded) {
	if (livetimecubefilename==NULL) {
	  printf("You must designate a live-time-cube file in main() %s:%d\n",__FILE__,__LINE__);
	  return -1;
	}
	if (loadltcube(livetimecubefilename, &ltcubeds)) return -1;
	if (loadpsffile(passfile,&psfds,f_cutoff)) return -1; 
	if (loadeffarea(passfile,&aeffds)) return -1;
	passfile_loaded=1;
	printf("# Test: %d\n",ltcubeds.aeffmubin[0]);
      }
      if (loadphotondata(*argptr,passfile)) {
	printf("# Cannot open %s in main() %s:%d\n",*argptr,__FILE__,__LINE__);
	return -1;
      }
      if (filecnt<1024) {
	filenames[filecnt]=*argptr;
	ntot_array[filecnt]=ntot;
	filecnt++;
      }
    }
  }
  times(&second_time);
  cpu_time_used = ((double) (second_time.tms_utime - first_time.tms_utime)) / sysconf(_SC_CLK_TCK);

  printf("# Load files and convert catalogue %ld %g seconds\n",(long) (second_time.tms_utime - first_time.tms_utime),cpu_time_used);

  if (healpixmax>12*healpixlevel*healpixlevel-1) {
    healpixmax=12*healpixlevel*healpixlevel-1;
  }
  printf("# HEALPIXLEVEL = %d (%d - %d)\n",healpixlevel,healpixmin,healpixmax);

  oneminusfcutoff=1-f_cutoff;
  printf("# PSF Fraction Enclosed= %g; ntot = %d; Nleaf = %d; %g - %g MeV\n",oneminusfcutoff,ntot,nleaf,e0,e1);

  maxsqr=0;
  ddone=data+ntot*d;
  for (dptr=data+3;dptr<ddone;dptr+=d) {
    val=fabs(*dptr);
    if (val>maxsqr) { maxsqr=val; }
  }
  printf("# max radius= %g degrees; energy_min is %g MeV maxsqr= %g\n",sqrt(maxsqr)/M_PI*180,energy_min,maxsqr);
  
  for (dptr=data+3;dptr<ddone;dptr+=d) {
    val=*dptr;
    if (val<0) {
      *dptr=-sqrt(maxsqr+val);
    } else {
      *dptr=sqrt(maxsqr-val);
    }
  }

  if (inputfile==NULL) {
    if ( (kd=kdtree_new(ntot,d,nleaf))==NULL) {
      printf("# kdtree is NULL after kdtree_new in main() %s:%d\n",__FILE__,__LINE__);
      return -1;
    }
    if ( (minval=(double *) calloc(d,sizeof(double)))==NULL) {
      printf("# failed to allocate minval in main() %s:%d\n",__FILE__,__LINE__);
      return -1;
    }
    if ( (maxval=(double *) calloc(d,sizeof(double)))==NULL) {
      printf("# failed to allocate maxval in main() %s:%d\n",__FILE__,__LINE__);
      return -1;
    }
    for (i=0;i<d;i++) {
      minval[i]=1E32; maxval[i]=-1E32;
    }
    for (j=0;j<ntot;j++) {
      for (i=0;i<d;i++) {
	double val=data[j*d+i];
	if (val<minval[i]) minval[i]=val;
	if (val>maxval[i]) maxval[i]=val;
      }
    }
    printf("# Min-Val  Max-Val\n");
    for (i=0;i<d;i++) {
      printf("# %8g %8g\n",minval[i],maxval[i]);
    }
    kd->minval=minval; 
    kd->maxval=maxval;
    kd->treetype = treetype;
    
    times(&first_time);
    kdtree_convert_data(kd, data, ntot, d, nleaf, kd->treetype);
    times(&second_time);
    cpu_time_used = ((double) (second_time.tms_utime - first_time.tms_utime)) / sysconf(_SC_CLK_TCK);
    printf("# Convert Data %ld %g seconds\n",(long) (second_time.tms_utime - first_time.tms_utime),cpu_time_used);
    times(&first_time);
    if (kdtree_build(kd,kd->data.s,ntot,d,nleaf,kd->treetype,KD_BUILD_SPLIT)==NULL) {
      printf("# unable to build tree in main() %s:%d\n",__FILE__,__LINE__);
      return -1;
    }
    times(&second_time);
    cpu_time_used = ((double) (second_time.tms_utime - first_time.tms_utime)) / sysconf(_SC_CLK_TCK);
    printf("# Build Tree %ld %g seconds\n",(long) (second_time.tms_utime - first_time.tms_utime),cpu_time_used);
  } else {
    if ( (kd=kdtree_fits_read(inputfile, NULL,&hdr))==NULL) {
      printf("# unable to load tree from %s in main() %s:%d\n",inputfile,__FILE__,__LINE__);
      return -1;
    }
  }

  if (outputfile!=NULL) {
    qfits_header* hdr_out;
    if ((hdr_out=qfits_header_default())==NULL) {
      printf("# hdr_out is NULL in main() %s:%d, not adding header information\n",__FILE__,__LINE__);
    } else {
      fits_header_add_double(hdr_out, "MAXSQR", maxsqr,  "fitskd_v3: maximum squared radius; search within this");
      fits_header_add_double(hdr_out, "F_CUTOFF", f_cutoff,  "fitskd_v3: fraction of PSF outside search radius");
      fits_header_add_double(hdr_out,"EMIN", e0,  "fitskd_v3: minimum energy cutoff");
      fits_header_add_double(hdr_out,"EMAX", e1,  "fitskd_v3: maximum energy cutoff");
      fits_header_add_double(hdr_out, "F_CUTOFF", f_cutoff,  "fitskd_v3: fraction of PSF outside search radius");
      fits_header_add_int(hdr_out, "NPHOTONF", filecnt, "fitskd_v3: number of photon files");
      for (i=0;i<filecnt;i++) {
	sprintf(buffer,"FILE%04d",i);
	fits_header_addf_longstring(hdr_out,buffer,"fitskd_v3: photon file","%s",filenames[i]);
	sprintf(buffer,"NTOT%04d",i);
	fits_header_add_int(hdr_out,buffer,ntot_array[i],"fitskd_v3: offset at end of photon file");
      }
    kdtree_fits_write(kd,outputfile,hdr_out);
    }
  }

  printf("# Number of data points: %d, number of nodes: %d\n",kdtree_n(kd),kdtree_nnodes(kd));

  printf("*\n");
  kdtree_memory_report(kd);
  printf("*\n");

  /* if ( (pt=(double *) calloc(d,sizeof(double)))==NULL) {
    return -1;
    } */
  /* pt[0]=1; for (i=1;i<d;i++) pt[i]=0;
   res = kdtree_rangesearch(kd, pt, 0.04); */

  times(&first_time);

  printpixelheader();

  if (readmode==0) {
#pragma omp parallel for schedule(guided)
    for (i=healpixmin;i<=healpixmax;i++) {
      double pt[4];
      kdtree_qres_t* res;
      pt[3]=0;
      nqueries++;
      healpix_to_xyzarr(i,healpixlevel,0.0,0.0,pt);
      res = kdtree_rangesearch_options(kd, pt, maxsqr,	0);
      if (res->nres>0) {
	calcpixeldata(res->inds, data, photondata, photontime, oneminusfcutoff, 1.0, 
		      &psfds, res->nres, pt, maxsqr, healpixlevel, i, d, debug,calcmode,0.0,0.0,NULL);
      }
      kdtree_free_query(res);
    }
  } else {
#define READLINELENGTH 255
#define NALLOCSTEP 256

    int localreadmode=abs(readmode);
    int nalloc=NALLOCSTEP, nlines=0, linecnt;
    char *filebuffer;
    
      
#ifndef __AVAILABILITY__
    void __sincospi(double ang, double *sinval, double *cosval);
#endif
    if ((debug>0) && (calcmode & CALC_SPECTRUM)) {
      printinfocolumns();
    }

    if ((filebuffer=calloc(sizeof(char),nalloc*READLINELENGTH))==NULL) {
      printf("Unable to allocate the file read buffer in main() %s:%d\n",__FILE__,__LINE__);
      return -1; 
    }
    /* read in the entire file, so we can run the various queries in parallel */
    while (fgets(filebuffer+nlines*READLINELENGTH,READLINELENGTH-1,stdin)) {
      nlines++;
      if (nlines>=nalloc) {
	if ( (filebuffer=realloc(filebuffer,(nalloc+NALLOCSTEP)*READLINELENGTH*sizeof(char)))==NULL) {
	  printf("Unable to reallocate the file read buffer in main() %s:%d\n",__FILE__,__LINE__);
	  return -1; 
	}
	nalloc+=NALLOCSTEP;
      }
    }
    /* in this loop we focus on potential sources so the number of photons may vary greatly */
#pragma omp parallel for schedule(dynamic)
    for (linecnt=0;linecnt<nlines;linecnt++) {
      char *bptr=filebuffer+linecnt*READLINELENGTH, readstr[]="hesgp", *rpos;
      double bfactor;
      double ra, dec, rad;
      int nres, healpixval=-1;
      double pt[4];
      kdtree_qres_t* res;

      pt[3]=0.0;
      /* remove the final carriage return */
      bptr[strlen(bptr)-1]=0;
      if ( (rpos=strchr(readstr,bptr[0]))!=NULL) {
	localreadmode=rpos-readstr+1;
	if (localreadmode>5) {
	  localreadmode=abs(readmode);
	} else {
	  bptr++;
	}
      }

      if (localreadmode==1) {
	if ( sscanf(bptr,"%d %lg",&healpixval,&bfactor) < 2 ) { bfactor=1; }
	if (healpixval<=healpixmax) 
	  healpix_to_xyzarr(healpixval,healpixlevel,0.0,0.0,pt);
      } else if (localreadmode==2 || localreadmode==3 || localreadmode==4) {
	healpixval=-1;
	if (localreadmode==2 || localreadmode==4) {
	  if ( sscanf(bptr,"%lg %lg %lg",&ra,&dec,&bfactor) < 3 ) { bfactor=1; }
	  if (localreadmode==4) {
	    double sinx,cosx, siny, cosy;
	    // Convert Galactic to the J2000
	    __sincospi((LNCP2000_G-ra)/180.0,&sinx,&cosx);
	    __sincospi(dec/180.0,&siny,&cosy);
	    dec=asin(SinDeltaGP2000*siny+CosDeltaGP2000*cosy*cosx)/RadDegree;
	    ra=atan2(cosy*sinx,CosDeltaGP2000*siny-SinDeltaGP2000*cosy*cosx)/RadDegree+AlphaGP2000;
	  }
	} else {
	  double d_d, d_m, d_s, r_h, r_m, r_s; 
	  if ( sscanf(bptr,"%lg %lg %lg %lg %lg %lg %lg",&r_h,&r_m,&r_s,&d_d,&d_m,&d_s,&bfactor) < 7 ) { bfactor=1; }
	  ra=15.0*(r_h+(r_m+r_s/60.0)/60.0);
	  if (d_d>0) {
	    dec=(d_d+(d_m+d_s/60.0)/60.0);
	  } else {
	    dec=(d_d-(d_m+d_s/60.0)/60.0);
	  }
	}
	__sincospi(ra/180.0,pt+1,pt);
	__sincospi(dec/180.0,pt+2,&rad);
	pt[0]*=rad;
	pt[1]*=rad;
	i=-1;
      } else if (localreadmode==5) {
	if ( sscanf(bptr,"%lg %lg %lg %lg",pt,pt+1,pt+2,&bfactor) < 4 ) { bfactor=1; }
	healpixval=-1;
      }

      nqueries++;
      res = kdtree_rangesearch_options(kd, pt, maxsqr, 0);
      nres=res->nres;

      if (readmode>0 && healpixval<healpixmax) {
	if (nres>0) {
	  calcpixeldata(res->inds, data, photondata, photontime, oneminusfcutoff, bfactor, 
			&psfds, nres, pt, maxsqr, healpixlevel, healpixval, d, debug,
			calcmode,ra,dec,filebuffer+linecnt*READLINELENGTH);
	
	}
      } else {
	int itercnt=0;
	double sum;
	while (1) {
	  double pt_old[3], dum;
	  int icnt;
	  pt_old[0]=pt[0]; pt_old[1]=pt[1]; pt_old[2]=pt[2];
	  if (nres>0) {
	    minpixeldata(res->inds, data, photondata, photontime, oneminusfcutoff, bfactor, 
			 &psfds, nres, pt, maxsqr, healpixlevel, healpixval, d, -1, 0,
			 &ra,&dec);
	  }
	  i=-1;
	  __sincospi(ra/180.0,pt+1,pt);
	  __sincospi(dec/180.0,pt+2,&rad);
	  pt[0]*=rad;
	  pt[1]*=rad;
	  sum=0;
	  for (icnt=0;icnt<3;icnt++) {
	    dum=pt_old[icnt]-pt[icnt];
	    sum+=dum*dum;
	  }
	  if (sum<1e-6 || itercnt>5) 
	    break;

	  nqueries++;
	  res = kdtree_rangesearch_options_reuse(kd, res, pt, maxsqr, 0);
	  nres=res->nres;
	}
	if (nres>0) {
	  calcpixeldata(res->inds, data, photondata, photontime, oneminusfcutoff, bfactor, 
			&psfds, nres, pt, maxsqr, healpixlevel, healpixval, d, debug,
			calcmode,ra,dec,filebuffer+linecnt*READLINELENGTH);
	}
      }
      kdtree_free_query(res);
    }
  }
  times(&second_time);
  cpu_time_used = ((double) (second_time.tms_utime - first_time.tms_utime)) / sysconf(_SC_CLK_TCK);
  printf("# %d Queries %ld %g seconds\n",nqueries,(long) (second_time.tms_utime - first_time.tms_utime),cpu_time_used);

  freepsdfdata(&psfds);
  freeeffarea(&aeffds);
  freeltcube(&ltcubeds);

  // kdtree_free(kd);
  SAFEFREE( data);
  return 0;
}
