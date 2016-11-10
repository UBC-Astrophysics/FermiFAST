#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumff \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):($1)*(800.0-765.0)/800.0+765.0' \
	--math /xrange -4:1 \
	'325*(1-exp(-10**(2*x)/2/0.005/0.005))+345*(1-exp(-10**(2*x)/2/0.018/0.018))+95*(1-exp(-10**(2*x)/2/0.11/0.11))+35.0*(1-exp(-10**(2*x)/2/1.5/1.5))' \
	'800*(1-exp(-10**(2*x)/2/1.5/1.5))' \
	-x 'Distance to Nearest Fermi 3FGL Source [Degrees]' \
	-y 'Cumulative Number' --xrange -4:1  --xlog --yrange 0:800
