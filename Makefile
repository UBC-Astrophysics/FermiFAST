# set CC to what makes most sense for you
# CC = /opt/local/bin/gcc-mp-4.8
CFLAGS = -fopenmp -O3 -I../astrometry.net-0.46/qfits-an -I../astrometry.net-0.46/util
#
# FermiFAST
#
FERMIFASTO = FermiFAST.o loadpsffile.o calcefft.o loadeffarea.o loadltcube.o gammq.o loadphotondata.o calcpixeldata.o j_dbrent.o j_mnbrak.o j_powell.o j_linmin.o j_brent.o j_utils.o
# If you don't have qsort_r on your machine, uncomment the following

# FERMIFASTO += j_qsort_r.o
# CFLAGS += -DNEED_QSORT_R
FermiFAST : $(FERMIFASTO)
	$(CC) -fopenmp -o FermiFAST $(FERMIFASTO) -lpthread -lm  ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/util/libanbase.a ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/qfits-an/libqfits.a ../astrometry.net-0.46/libkd/libkd.a   ../astrometry.net-0.46/util/libanutils.a 
fermifast : FermiFAST
	cp FermiFAST fermifast
FermiFAST_clean :
	rm FermiFAST $(FERMIFASTO)
	make FermiFAST
FermiFAST.o : loadeffarea.h loadltcube.h loadpsffile.h calcefft.h calcpixeldata.h \
	loadphotondata.h
loadeffarea.o : loadeffarea.h
loadltcube.o : loadltcube.h 
loadpsffile.o : loadpsffile.h 
calcefft.o : calcefft.h loadeffarea.h loadltcube.h
calcpixeldata.o : calcpixeldata.h loadpsffile.h loadphotondata.h j_minimizer.h
j_dbrent.o : j_minimizer.h
j_mnbrak.o : j_minimizer.h
j_powell.o : j_minimizer.h
j_linmin.o : j_minimizer.h
j_brent.o :  j_minimizer.h
j_utils.o :  j_minimizer.h
j_dfpmin.o : j_minimizer.h
#
# peakfind.c
#
PEAKFINDO = peakfind.o loadfile.o kdtree.o
peakfind : $(PEAKFINDO)
	$(CC) -o peakfind $(PEAKFINDO) -lm  
peakfind_clean :
	rm peakfind $(PEAKFINDO)
	make peakfind
loadfile.o : loadfile.h
peakfind.o : loadfile.h
peakfind.o : kdtree.h
kdtree.o   : kdtree.h
##################
## These are older versions ##
##################
FITSKDO = fitskd.o 
fitskd : $(FITSKDO)
	gcc -o fitskd $(FITSKDO) -lpthread -lm  ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/util/libanbase.a ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/qfits-an/libqfits.a ../astrometry.net-0.46/libkd/libkd.a   ../astrometry.net-0.46/util/libanutils.a 
FITSKD_V2O = fitskd_v2.o 
fitskd_v2 : $(FITSKD_V2O)
	gcc -o fitskd_v2 $(FITSKD_V2O) -lpthread -lm  ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/util/libanbase.a ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/qfits-an/libqfits.a ../astrometry.net-0.46/libkd/libkd.a   ../astrometry.net-0.46/util/libanutils.a 
#
#
#
all = fitskd fitskd_v2
USEKDO = usekd.o
usekd : $(USEKDO)
	gcc -o usekd $(USEKDO) ../astrometry.net-0.46/libkd/libkd.a ../astrometry.net-0.46/qfits-an/libqfits.a ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46//util/libanbase.a -lm -L/opt/local/lib -lgsl
PHOTONKDO = photonkd.o
photonkd : $(PHOTONKDO)
	gcc -o photonkd $(PHOTONKDO) ../astrometry.net-0.46/libkd/libkd.a ../astrometry.net-0.46/qfits-an/libqfits.a ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/util/libanbase.a -lm -L/opt/local/lib -lgsl
QUICKSO = quicksolve.o
quicksolve : $(QUICKSO)
	gcc -o quicksolve $(QUICKSO) -lpthread -lm  ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/util/libanbase.a ../astrometry.net-0.46/util/libanutils.a ../astrometry.net-0.46/qfits-an/libqfits.a ../astrometry.net-0.46/libkd/libkd.a   ../astrometry.net-0.46/util/libanutils.a 
FermiFAST.o : loadeffarea.h loadltcube.h loadpsffile.h calcefft.h
loadeffarea.o : loadeffarea.h
