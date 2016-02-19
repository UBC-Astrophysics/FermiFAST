awk '($1!="#") {print $63}' 1GeV_match.dat | sort -r -g | awk '{print $1, NR}' > 1GeV_cumsig.dat
awk '($6!~"inf") {print $6}' fermi-3psc.catalogue.dat | sort -r -g | awk '{print $1, NR}' >  3psc_cumsig.dat
ctioga2 --xlog --ylog --name sign-dist 1GeV_cumsig.dat \
	3psc_cumsig.dat \
	--xlabel '3FGL Significance [sigma-units]' \
        --ylabel 'Cumulative Number of Sources'


