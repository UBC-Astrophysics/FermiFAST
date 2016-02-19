#!/usr/bin/env python
#
# getpeaks.py
#
# Jeremy Heyl
#
# This file is part of ``fermi-fast'', a suite of programs for rapidly identifying
# point sources in the Fermi LAT photon event files.
# Copyright (C) 2015 Jeremy Heyl <heyl@phas.ubc.ca>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
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
#
#


from argparse import ArgumentParser

import math as mt
import numpy as np
import healpy as hp
import matplotlib.pyplot as plt
import gzip

def _parse_command_line_arguments():
    """
    Parse and return command line arguments
    """
    parser = ArgumentParser(
        description=(
            'Command-line tool to generate a list of unique peaks from a TS file from FermiFAST'
        ),
    )
    parser.add_argument(
        'ts-file',
        type=str,
        help=(
            'A file containing the TS sky map'
        ),
    )
    parser.add_argument(
        'peak-file',
        type=str,
        help=(
            'A file containing the peaks'
        ),
    )
    
    parser.add_argument('--skiprows',
                        type=int,
                        help='number of rows to skip at the top (default 26)',
                        required=False)
    parser.set_defaults(skiprows=26)
    parser.add_argument('--column',
                        type=int,
                        help='number of the column to use starting from zero (default 24)',
                        required=False)
    parser.set_defaults(column=24)

    arguments = vars(parser.parse_args())
    return arguments


def IndexToDeclRa(NSIDE,index):
    theta,phi=hp.pixelfunc.pix2ang(NSIDE,index)
    return np.degrees(mt.pi/2.0-theta),np.degrees(phi)

def DeclRaToIndex(decl,RA,NSIDE):
    return hp.pixelfunc.ang2pix(NSIDE,np.radians(90.-decl),np.radians(RA))

def get_peaks(file,peakfile,skiprows=26,column=24,siglevel=-12.5):
    data = np.loadtxt(file,skiprows=skiprows)
    print(np.shape(data))
    nside=int(round(mt.sqrt(len(data)/12)))
    print('nside = %d' % nside)
    # keep just the most significant data
    keepdata=data[data[:,column]<siglevel]
    print(np.shape(keepdata))
    data=keepdata
    index=np.array(data[:,0]).astype(int)
    
    # from the most significant to the least significant
    sortedpositions=np.argsort(data[:,column])
    peaklist=[]
    hpeaklist=[]
    hotlist=index[sortedpositions]
    for n,healpixnum,pos in zip(range(len(sortedpositions)),hotlist,sortedpositions):
        hotter_neighbours=np.intersect1d(hp.pixelfunc.get_all_neighbours(nside,healpixnum),hotlist[:n],assume_unique=True)
        if (len(hotter_neighbours)==0):
            peaklist.append(pos)
            hpeaklist.append(hpeaklist)

    print("# number of unique peaks= %d" % len(peaklist))
    np.savetxt(peakfile,data[peaklist],header='#           1                  2                              3                     4                        5                        6                            7                    8                          9                        10                        11                       12                       13                      14                       15                         16                       17                       18                      19                       20                       21                        22                      23                     24                           25\n\
#      Healpix                 N                             TS                  MEANR2                  MEANFRAC                   SIGR2                      SIGFRAC                AFRAC                       RA                       DEC                      BVALUE                   NBACK                   BACK_OFF               ln(PTSNew)                DeltaSig                   csource                  cesource                   cback                  ceback                   sumL0                     sumL1                  2(DeltaSum)                TSPSF                  APSF                      ln(ProbPSF)\n')

    

def _main():

    """
    This is the main routine.
    """

    args=_parse_command_line_arguments()
    get_peaks(args['ts-file'],args['peak-file'],skiprows=args['skiprows'],column=args['column'])


#------------------------------------------------------------------------------
# Start program execution.
#

if __name__ == '__main__':

    _main()
