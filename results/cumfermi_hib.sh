#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumfermi_hib \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_hib@'log10($2*45/atan(1.0)):$1' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped_hib@'log10($2*45/atan(1.0)):$1' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped_hib@'log10($2*45/atan(1.0)):$1*(2200.0-590.0)/2200+590.0' \
	--math /xrange -4:1 \
	'300*(1-exp(-10**(2*x)/2/0.005/0.005))+250*(1-exp(-10**(2*x)/2/0.02/0.02))+30*(1-exp(-10**(2*x)/2/0.1/0.1))+(2200-680)*(1-exp(-10**(2*x)/2/2.8/2.8))' \
	'2200*(1-exp(-10**(2*x)/2/2.8/2.8))' \
	-x 'Distance to Nearest FF Source [Degrees]' \
	-y 'Cumulative Number' --xrange -3.2:1.2  --xlog --yrange 0:2200
