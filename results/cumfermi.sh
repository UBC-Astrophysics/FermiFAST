#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumfermi \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match@'log10($2*45/atan(1.0)):$1' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped@'log10($2*45/atan(1.0)):$1' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped@'log10($2*45/atan(1.0)):($1)*(1.0-0.270163)+0.270163*3034' \
	--math /xrange -4:1 \
	'325*(1-exp(-10**(2*x)/2/0.005/0.005))+325*(1-exp(-10**(2*x)/2/0.02/0.02))+150*(1-exp(-10**(2*x)/2/0.1/0.1))+(3034-800)*(1-exp(-10**(2*x)/2/2.6/2.6))' \
	'3034*(1-exp(-10**(2*x)/2/2.6/2.6))' \
	-x 'Distance to Nearest FF Source [Degrees]' \
	-y 'Cumulative Number' --xrange -3.2:1.2  --xlog --yrange 0:3050
