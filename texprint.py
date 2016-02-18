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

def _parse_command_line_arguments():
    """
    Parse and return command line arguments
    """
    parser = ArgumentParser(
        description=(
            'Command-line tool to generate a list of unique from a TS file from FermiFAST'
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
                        help='number of rows to skip at the top (default 0)',
                        required=False)
    parser.set_defaults(skiprows=0)
    arguments = vars(parser.parse_args())
    return arguments


def texprint(file,skiprows=0):
    data = np.loadtxt(file,skiprows=skiprows)
    print("""\\begin{tabular}{l|rrrrrrrrrrr}
    \hline
    Source & \multicolumn{1}{c}{RA} & \multicolumn{1}{c}{Dec}  & \multicolumn{1}{c}{$N_\mathrm{photons}$}  &\multicolumn{1}{c}{$\bar r^2$} & \multicolumn{1}{c}{$\bar f$} & \multicolumn{1}{c}{$S(r^2)$} & \multicolumn{1}{c}{$S(f)$} & \multicolumn{1}{c}{$A_f$} & \multicolumn{1}{c}{$TS_\mathrm{PSF}$} & \multicolumn{1}{c}{$A_\mathrm{PSF}$} & \multicolumn{1}{c}{$\ln P(TS)$} \\\\
    \hline
""")
    for row in data:
        print('Name  & $%6.2f$ & $%6.2f$ & %6d & $%5.3f$ & $%5.3f$ & $%6.1f$ & $%6.1f$ & $%4.2f$ & $%9.2f$ & $%4.2f$ & $%9.2f$ \\\\ ' % (row[8],row[9],row[1],row[3],row[4],row[5],row[6],row[7],row[22],row[23],row[24]))

    print("\end{tabular}")

    
def _main():

    """
    This is the main routine.
    """

    args=_parse_command_line_arguments()
    texprint(args['ts-file'],skiprows=args['skiprows'])


#------------------------------------------------------------------------------
# Start program execution.
#

if __name__ == '__main__':

    _main()
