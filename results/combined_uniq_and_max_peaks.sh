# plot correlation between Af and Apsf
. acorr.sh
# plot correlation between significances
. sigcorr.sh
tail +26 max_unique.txt | ~/Documents/kd-match-0.3.0/match_kd -eq -x1 9 -y1 10 -x2 26 -y2 27 unique_peaks.txt - > unique_peaks_maximized.txt
# find sources with a positive value of APSF and calculate the shift and final positions
awk -f calcshift.awk unique_peaks_maximized.txt > unique_peaks_maximized_selected.txt
# plot change in log L
. changelogL.sh
# plot change in positionL
. change_position.sh

