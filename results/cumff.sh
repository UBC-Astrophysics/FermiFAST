#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumff \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):($1)*(800.0-750.0)/800.0+750.0' \
	--math /xrange -4:1 \
	'350*(1-exp(-10**(2*x)/2/0.0035/0.0035))+300*(1-exp(-10**(2*x)/2/0.015/0.015))+100*(1-exp(-10**(2*x)/2/0.05/0.05))+50.0*(1-exp(-10**(2*x)/2/0.7/0.7))' \
	'800*(1-exp(-10**(2*x)/2/0.7/0.7))' \
	-x 'Distance to Nearest Fermi 3PSC Source [Degrees]' \
	-y 'Cumulative Number' --xrange -4:1  --xlog --yrange 0:800
