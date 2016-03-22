~/Documents/kd-match-0.3.0/match_kd \
    -eq  unique_0.1GeV_peaks_radec.lst fermi-3psc.catalogue_flipped  -x2 2 -y2 3 \
    | ~/Documents/kd-match-0.3.0/match_kd \
    -eq - ra_dec_flipped.lst -x2 2 -y2 3 > 100MeV_fermi_1GeV_match
awk '($3<0.00778 && $11>0.0095)' 100MeV_fermi_1GeV_match > 100MeV_fermi_match
awk '($3>0.00778 && $11<0.0095)' 100MeV_fermi_1GeV_match > 100MeV_1GeV_match
awk '($3>0.00778 && $11>0.0095)' 100MeV_fermi_1GeV_match > 100MeV_new_match
