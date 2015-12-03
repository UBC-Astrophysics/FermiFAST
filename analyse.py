#!/usr/bin/env python
#
# analyse.py
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
    parser.add_argument('--skiprows',
                        type=int,
                        help='number of rows to skip at the top (default 32)',
                        required=False)
    parser.set_defaults(skiprows=32)
    parser.add_argument('--column',
                        type=int,
                        help='number of the column to use starting from zero (default 22)',
                        required=False)
    parser.set_defaults(column=22)

    arguments = vars(parser.parse_args())
    return arguments


def IndexToDeclRa(NSIDE,index):
    theta,phi=hp.pixelfunc.pix2ang(NSIDE,index)
    return np.degrees(mt.pi/2.0-theta),np.degrees(phi)

def DeclRaToIndex(decl,RA,NSIDE):
    return hp.pixelfunc.ang2pix(NSIDE,np.radians(90.-decl),np.radians(RA))

def plot_tsfile(file,skiprows=32,column=22):

    
    data = np.loadtxt(file,skiprows=skiprows,unpack=True)

    print len(data[0])
    nside=int(round(mt.sqrt(len(data[9])/12)))
    tsarray=np.zeros(nside*nside*12)
    index=np.array(data[0]).astype(int)

    '''
    data2=np.transpose(0*data)
    data2[index]=np.transpose(data)
    data2=np.transpose(data2)
    print(data2[0])
    print(data2[10])
    return
    '''

    tsarray[index]=data[column]
    tsarray_s = hp.sphtfunc.smoothing(tsarray,sigma = 1.023/nside)

    hp.mollview(np.arcsinh(tsarray)/mt.log(10.0),coord=['C','G'], title='TS Map', unit='prob',xsize = 2048)
    hp.graticule()
    plt.savefig("%s_c%d.png" % (file,column))
    plt.show()

    hp.mollview(np.arcsinh(tsarray_s)/mt.log(10.0),coord=['C','G'], title='TS Map Smoothed', unit='prob',xsize = 2048)
    hp.graticule()
    plt.savefig("%s_c%d__s.png" % (file,column))
    plt.show()


def _main():

    """
    This is the main routine.
    """

    args=_parse_command_line_arguments()
    plot_tsfile(args['ts-file'],skiprows=args['skiprows'],column=args['column'])


#------------------------------------------------------------------------------
# Start program execution.
#

if __name__ == '__main__':

    _main()
