for sig in 12.5 10.125 8 6.125 4.5
do
    awk "(\$25<-${sig} && \$24>0)" unique_peaks_1GeV_losig.txt > unique_peaks_1GeV_to${sig}.txt
    ~/Documents/kd-match-0.3.0/match_kd \
	-eq unique_peaks_1GeV_to${sig}.txt fermi-3psc.catalogue_flipped -x1 9 -y1 10 -x2 2 -y2 3 \
	| grep -v '#' \
	| awk '{print $26}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_${sig}_uniq_fermi_match
    ~/Documents/kd-match-0.3.0/match_kd \
	-eq unique_peaks_1GeV_to${sig}.txt fermi-3psc.catalogue_flipped -x1 9 -y1 10 -x2 6 -y2 7 \
	| grep -v '#' \
	| awk '{print $26}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_${sig}_uniq_fermi_match_flipped
    
    awk '($5*$5>100)' fermi-3psc.catalogue_flipped | ~/Documents/kd-match-0.3.0/match_kd \
							 -eq -x1 9 -y1 10 unique_peaks_1GeV_to${sig}.txt - -x2 2 -y2 3 \
	| grep -v '#' \
	| awk '($10*$10>100) {print $26}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_${sig}_fermi_match_hib
    awk '($5*$5>100)' fermi-3psc.catalogue_flipped | ~/Documents/kd-match-0.3.0/match_kd \
							 -eq -x1 9 -y1 10 unique_peaks_1GeV_to${sig}.txt -  -x2 6 -y2 7 \
	| grep -v '#' \
	| awk '($10*$10>100) {print $26}' | sort -g | awk '{print NR, $0}' > cum_all_ts_data_1GeV_hires_${sig}_uniq_fermi_match_flipped_hib

done
. cumfflo_comp.sh
. cumfflo_comp_hib.sh
