#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumff_hib \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_hib@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped_hib@'log10($2*45/atan(1.0)):$1' \
	--math /xrange -4:1 \
	-c Red --line-style dashes \
	'300*(1-exp(-10**(2*x)/2/0.005/0.005))+230*(1-exp(-10**(2*x)/2/0.015/0.015))+42*(1-exp(-10**(2*x)/2/0.05/0.05))+8*(1-exp(-10**(2*x)/2/1.5/1.5))' \
	-c Green --line-style dashes \
	'580*(1-exp(-10**(2*x)/2/1.5/1.5))' \
	-c Blue --line-style solid --text \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped_hib@'log10($2*45/atan(1.0)):$1*(580.0-572.0)/580.0+572.0' \
	-x 'Distance to Nearest Fermi 3FGL Source [Degrees]' \
	-y 'Cumulative Number' --xrange -4:1  --xlog --yrange 0:650

