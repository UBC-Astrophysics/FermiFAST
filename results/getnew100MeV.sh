~/Documents/kd-match-0.3.0/match_kd \
    -eq  unique_0.1GeV_peaks_radec.lst fermi-3psc.catalogue_flipped  -x2 2 -y2 3 \
    | ~/Documents/kd-match-0.3.0/match_kd \
    -eq - ra_dec_flipped.lst -x2 2 -y2 3 > 100MeV_fermi_1GeV_match
awk '($3<0.00778 && $11>0.0095)' 100MeV_fermi_1GeV_match > 100MeV_fermi_match
awk '($3>0.00778 && $11<0.0095)' 100MeV_fermi_1GeV_match > 100MeV_1GeV_match
awk '($3>0.00778 && $11>0.0095)' 100MeV_fermi_1GeV_match > 100MeV_new_match
awk '($1!="#") {print $2,$3}' ra_dec_flipped.lst \
    | cat - unique_0.1GeV_peaks_radec.lst \
    | ~/Documents/kd-match-0.3.0/match_kd \
	  -eq  fermi-3psc.catalogue_flipped  - -x1 2 -y1 3 \
    | awk '($1!="#") {print $1,$8}' > combo.match
awk '($1!="#") {print $2,$3}' ra_dec_flipped.lst \
    | cat - unique_0.1GeV_peaks_radec.lst \
    | ~/Documents/kd-match-0.3.0/match_kd \
	  -eq  fermi-3psc.catalogue_flipped  - -x1 6 -y1 7 \
    | awk '($1!="#") {print $1,$8}' > combo.match_flipped

