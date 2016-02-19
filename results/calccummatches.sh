awk '($1!="#"){ i++; print i,$(NF-1),$NF}' unique_peaks_maximized_selected.txt | awk -f fliplist.awk > ra_dec_flipped.lst
~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-3psc.catalogue_flipped ra_dec_flipped.lst -x1 2 -y1 3 -x2 2 -y2 3 \
    | grep -v '#' \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match
~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-3psc.catalogue_flipped ra_dec_flipped.lst -x1 2 -y1 3 -x2 6 -y2 7 \
    | grep -v '#' \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped
. cumfermi.sh

awk '($5*$5>100)' ra_dec_flipped.lst | ~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-3psc.catalogue_flipped - -x1 2 -y1 3 -x2 2 -y2 3 \
    | grep -v '#' \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match_hib
awk '($5*$5>100)' ra_dec_flipped.lst | ~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-3psc.catalogue_flipped - -x1 2 -y1 3 -x2 6 -y2 7 \
    | grep -v '#' \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped_hib
. cumfermi_hib.sh

~/Documents/kd-match-0.3.0/match_kd \
    -eq ra_dec_flipped.lst fermi-3psc.catalogue_flipped -x1 2 -y1 3 -x2 2 -y2 3 \
    | grep -v '#' \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match
~/Documents/kd-match-0.3.0/match_kd \
    -eq ra_dec_flipped.lst fermi-3psc.catalogue_flipped -x1 2 -y1 3 -x2 6 -y2 7 \
    | grep -v '#' \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped
. cumff.sh

awk '($5*$5>100)' fermi-3psc.catalogue_flipped | ~/Documents/kd-match-0.3.0/match_kd \
    -eq ra_dec_flipped.lst - -x1 2 -y1 3 -x2 2 -y2 3 \
    | grep -v '#' \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match_hib
awk '($5*$5>100)' fermi-3psc.catalogue_flipped | ~/Documents/kd-match-0.3.0/match_kd \
    -eq ra_dec_flipped.lst - -x1 2 -y1 3 -x2 6 -y2 7 \
    | grep -v '#' \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped_hib
. cumff_hib.sh

