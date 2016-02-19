#!/usr/bin/env python
#
# analyse.py
#
# Jeremy Heyl
#
import math as mt
import numpy as np
import healpy as hp
import matplotlib.pyplot as plt

import math as mt
import numpy as np
import healpy as hp
import matplotlib.pyplot as plt


def IndexToDeclRa(NSIDE,index):
    theta,phi=hp.pixelfunc.pix2ang(NSIDE,index)
    return np.degrees(mt.pi/2.0-theta),np.degrees(phi)

def DeclRaToIndex(decl,RA,NSIDE):
    return hp.pixelfunc.ang2pix(NSIDE,np.radians(90.-decl),np.radians(RA))

def _main():

    """
    This is the main routine.
    """

    data = np.loadtxt("all_ts_data_0.1GeVplus.gz",skiprows=32,unpack=True)

    print len(data[0])
    nside=int(round(mt.sqrt(len(data[9])/12)))
    tsarray=data[0]*0
    RA=data[8]
    decl=data[9]
#    index=DeclRaToIndex(decl,RA,nside)
    index=hp.nest2ring(nside,np.array(data[0]).astype(int))
    tsarray[index]=data[22]
    tsarray = hp.sphtfunc.smoothing(tsarray,sigma = 0.008)

 #   tsmap=0*tsarray


    hp.mollview(np.arcsinh(tsarray)/mt.log(10.0),coord='C', title='TS Map', unit='prob',xsize = 2048)
#    hp.graticule()
    plt.savefig("ts.png")
    plt.show()


#------------------------------------------------------------------------------
# Start program execution.
#

if __name__ == '__main__':

    _main()
