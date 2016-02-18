~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-2psc.catalogue_flipped all_ts_data_1GeV_hires_uniq_flipped -x1 2 -y1 3 -x2 2 -y2 3 \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match
~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-2psc.catalogue_flipped all_ts_data_1GeV_hires_uniq_flipped -x1 2 -y1 3 -x2 6 -y2 7 \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped
. cumfermi.sh

awk '($5*$5>100)' all_ts_data_1GeV_hires_uniq_flipped | ~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-2psc.catalogue_flipped - -x1 2 -y1 3 -x2 2 -y2 3 \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match_hib
awk '($5*$5>100)' all_ts_data_1GeV_hires_uniq_flipped | ~/Documents/kd-match-0.3.0/match_kd \
    -eq fermi-2psc.catalogue_flipped - -x1 2 -y1 3 -x2 6 -y2 7 \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped_hib
. cumfermi_hib.sh

~/Documents/kd-match-0.3.0/match_kd \
    -eq all_ts_data_1GeV_hires_uniq_flipped fermi-2psc.catalogue_flipped -x1 2 -y1 3 -x2 2 -y2 3 \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match
~/Documents/kd-match-0.3.0/match_kd \
    -eq all_ts_data_1GeV_hires_uniq_flipped fermi-2psc.catalogue_flipped -x1 2 -y1 3 -x2 6 -y2 7 \
    | awk '{print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped
. cumff.sh

awk '($5*$5>100)' fermi-2psc.catalogue_flipped | ~/Documents/kd-match-0.3.0/match_kd \
    -eq all_ts_data_1GeV_hires_uniq_flipped - -x1 2 -y1 3 -x2 2 -y2 3 \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match_hib
awk '($5*$5>100)' fermi-2psc.catalogue_flipped | ~/Documents/kd-match-0.3.0/match_kd \
    -eq all_ts_data_1GeV_hires_uniq_flipped - -x1 2 -y1 3 -x2 6 -y2 7 \
    | awk '($5*$5>100) {print $8}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped_hib
. cumff_hib.sh

