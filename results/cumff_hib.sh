#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumff_hib \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_hib@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped_hib@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped_hib@'log10($2*45/atan(1.0)):$1*(1082.0-1020.0)/1082.0+1020.0' \
	--math /xrange -2:1 \
	'820*(1-exp(-10**(2*x)*100))+200*(1-exp(-10**(2*x)*8))+62*(1-exp(-10**(2*x)*0.45))' \
	-x 'Distance to Nearest Fermi 2PSC Source [Degrees]' \
	-y 'Cumulative Number' --xrange -2:1  --xlog --yrange 0:1100
