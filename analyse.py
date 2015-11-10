#!/usr/bin/env python
#
# analyse.py
#
# Jeremy Heyl
#
# This file is part of ``fermi-fast'', a suite of programs for rapidly identifying
# point sources in the Fermi LAT photon event files.
# Copyright (C) 2015 Jeremy Heyl <heyl@phas.ubc.ca>

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
# OF SUCH DAMAGE.




from argparse import ArgumentParser

import math as mt
import numpy as np
import healpy as hp
import matplotlib.pyplot as plt

def _parse_command_line_arguments():
    """
    Parse and return command line arguments
    """
    parser = ArgumentParser(
        description=(
            'Command-line tool to generate a skymap from a TS file from FermiFAST'
        ),
    )
    parser.add_argument(
        'ts-file',
        type=str,
        help=(
            'A file containing the TS sky map'
        ),
    )
    arguments = vars(parser.parse_args())
    return arguments


def IndexToDeclRa(NSIDE,index):
    theta,phi=hp.pixelfunc.pix2ang(NSIDE,index)
    return np.degrees(mt.pi/2.0-theta),np.degrees(phi)

def DeclRaToIndex(decl,RA,NSIDE):
    return hp.pixelfunc.ang2pix(NSIDE,np.radians(90.-decl),np.radians(RA))

def plot_tsfile(file):
    data = np.loadtxt(file,skiprows=25,unpack=True)

    print len(data[0])
    nside=int(round(mt.sqrt(len(data[9])/12)))
    tsarray=data[0]*0
    RA=data[8]
    decl=data[9]
#    index=DeclRaToIndex(decl,RA,nside)
#    index=hp.nest2ring(nside,np.array(data[0]).astype(int))
    index=np.array(data[0]).astype(int)
    tsarray[index]=data[22]
    tsarray_s = hp.sphtfunc.smoothing(tsarray,sigma = 1.023/nside)

#   tsmap=0*tsarray


    hp.mollview(np.arcsinh(tsarray)/mt.log(10.0),coord=['C','G'], title='TS Map', unit='prob',xsize = 2048)
    hp.graticule()
    plt.savefig("%s.png" % file)
    plt.show()

    hp.mollview(np.arcsinh(tsarray_s)/mt.log(10.0),coord=['C','G'], title='TS Map Smoothed', unit='prob',xsize = 2048)
    hp.graticule()
    plt.savefig("%s_s.png" % file)
    plt.show()


def _main():

    """
    This is the main routine.
    """

    args=_parse_command_line_arguments()
    plot_tsfile(args['ts-file'])


#------------------------------------------------------------------------------
# Start program execution.
#

if __name__ == '__main__':

    _main()
