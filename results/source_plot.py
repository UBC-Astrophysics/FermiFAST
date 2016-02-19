import numpy as np
import eq2gal
import math
#np.seterr("raise")

from pylab import *

vec_eq2gal=np.vectorize(eq2gal.eq2gal)
flong,flat=np.loadtxt("fermi-3psc.catalogue.dat",usecols=(3,4),unpack=True)
flong=np.where(flong>180,flong-360,flong)
ffra,ffdec=np.loadtxt("1GeV_match.dat",usecols=(54,55),unpack=True)
fflong, fflat = vec_eq2gal(np.radians(ffra),np.radians(ffdec))
fflong=np.where(fflong>math.pi,fflong-2*math.pi,fflong)

fflra,ffldec,apsf=np.loadtxt("unique_0.1GeV_peaks.txt",usecols=(8,9,23),unpack=True)
fflra=fflra[apsf>0]
ffldec=ffldec[apsf>0]
ffllong, ffllat = vec_eq2gal(np.radians(fflra),np.radians(ffldec))
ffllong=np.where(ffllong>math.pi,ffllong-2*math.pi,ffllong)

subplot(111, projection="mollweide")
scatter(ffllong,ffllat,81,'r')
scatter(fflong,fflat,25,'b')
scatter(np.radians(flong),np.radians(flat),1,'k')
savefig("source_plot.pdf")
