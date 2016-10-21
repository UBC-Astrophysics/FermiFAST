#!/bin/sh
runcmd () {
    if [ -e $2 ] 
	then
	echo $2 exists
	echo Skipping $1
    else
	echo Running $1
	if [ -n "${3}" ] 
	    then
	    $1 > $3
	else
	    $1
	fi
    fi
}
if [ $# -lt 1 ]
    then
    echo doanalysis.sh _Name_ [  RA DEC Radius [ TMin TMax ] ]
    exit
fi
echo Processing Data for $1
if [ $# -lt 4 ]
    then
    a=`echo L*_PH00.fits`
    runcmd "gtvcut suppress_gtis=yes infile=$a table=EVENTS" ${1}_gtvcut.out ${1}_gtvcut.out
    if [ -e ${1}_center.out ] 
	then
	echo ${1}_center.out exists
    else
	awk '/CIRCLE/ { sub(/CIRCLE/,"",$0); gsub(/[,\(\)]/," ",$0); print $2, $3, $4}' ${1}_gtvcut.out  > ${1}_center.out
    fi
    read xref yref radius <  ${1}_center.out
    if [ -e ${1}_events.txt ] 
	then
	echo ${1}_events.txt exists
    else
	ls *_PH*.fits > ${1}_events.txt
    fi
else
    ls /Software/all_weeks/lat_photon_weekly_w*_p202_v001_gti.fits > ${1}_events.txt
    xref=$2
    yref=$3
    radius=$4
    if [ $# -lt 6 ] 
	then
	echo Reading time interval from Spacecraft.fits
	fold -w 80  Spacecraft.fits | head -40 | awk '($1=="TSTART") { ts=$3 } ($1=="TSTOP") { te=$3 } END { print ts, te }' > ${1}_interval.dat
	read tmin tmax < ${1}_interval.dat
    fi
fi
if [ $# -gt 5 ] 
    then
    tmin=$5
    tmax=$6
fi
tmin=${tmin:-INDEF}
tmax=${tmax:-INDEF}
SRCRAD=$[radius+10]
echo RACENTER=$xref deg DECCENTER=$yref deg RADIUS=$radius deg SRCRAD=$SRCRAD deg Time Interval: $tmin - $tmax MET

runcmd "gtselect evclass=2 infile=@${1}_events.txt outfile=${1}_gtselect.fits ra=$xref dec=$yref rad=$radius tmin=${tmin} tmax=${tmax} emin=100 emax=300000 zmax=100" ${1}_gtselect.fits

if [ -e ${1}_input_model.xml ]
    then
    echo I will use ${1}_input_model.xml
else
    echo I will build an input model.
    python createxml.py $1 $xref $yref
    echo I have created ${1}_input_model.xml
fi

runcmd "gtmktime scfile=Spacecraft.fits filter=(DATA_QUAL>0)&&(LAT_CONFIG==1) roicut=yes evfile=${1}_gtselect.fits outfile=${1}_gti.fits" ${1}_gti.fits

runcmd "gtbin evfile=${1}_gti.fits scfile=Spacecraft.fits outfile=${1}_cmap.fits algorithm=CMAP nxpix=300 nypix=300 binsz=0.2 coordsys=CEL proj=AIT xref=$xref yref=$yref axisrot=0" ${1}_cmap.fits

runcmd "gtbin evfile=${1}_gti.fits scfile=Spacecraft.fits outfile=${1}_ccube.fits algorithm=CCUBE ebinalg=LOG emin=100 emax=100000 enumbins=30 nxpix=300 nypix=300 binsz=0.2 coordsys=CEL proj=AIT xref=$xref yref=$yref axisrot=0" ${1}_ccube.fits

runcmd "gtltcube evfile=${1}_gti.fits scfile=Spacecraft.fits outfile=${1}_ltcube.fits dcostheta=0.025 binsz=1" ${1}_ltcube.fits

runcmd "gtexpcube2 infile=${1}_ltcube.fits outfile=${1}_expcube.fits irfs=P7REP_SOURCE_V15 xref=$xref yref=$yref nxpix=360 nypix=360 pixscale=0.25 coordsys=GAL axisrot=0 proj=AIT emin=100 emax=300000 enumbins=30" ${1}_expcube.fits

runcmd "gtsrcmaps cmap=${1}_ccube.fits expcube=${1}_ltcube.fits srcmdl=${1}_input_model.xml bexpmap=${1}_expcube.fits outfile=${1}_srcmaps.fits irfs=P7REP_SOURCE_V15" ${1}_srcmaps.fits

runcmd "gtlike refit=no plot=no sfile=${1}_binned_output_model.xml scfile=Spacecraft.fits evfile=${1}_gti.fits srcmdl=${1}_input_model.xml expcube=${1}_ltcube.fits bexpmap=${1}_expcube.fits statistic=BINNED optimizer=NEWMINUIT cmap=${1}_srcmaps.fits irfs=P7REP_SOURCE_V15" ${1}_binned_output_model.xml
