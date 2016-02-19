#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumfermi \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match@'log10($2*45/atan(1.0)):$1' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped@'log10($2*45/atan(1.0)):$1' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped@'log10($2*45/atan(1.0)):($1)*(3034-800.0)/3034+800.0' \
	--math /xrange -4:1 \
	'300*(1-exp(-10**(2*x)/2/0.0035/0.0035))+350*(1-exp(-10**(2*x)/2/0.01/0.01))+150*(1-exp(-10**(2*x)/2/0.07/0.07))+(3034-800)*(1-exp(-10**(2*x)/2/1.3/1.3))' \
	'3034*(1-exp(-10**(2*x)/2/1.3/1.3))' \
	-x 'Distance to Nearest FF Source [Degrees]' \
	-y 'Cumulative Number' --xrange -4:1  --xlog --yrange 0:3050
