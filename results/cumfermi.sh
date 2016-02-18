#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumfermi \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match@'log10($2*45/atan(1.0)):$1-3' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped@'log10($2*45/atan(1.0)):$1-3' \
	cum_fermi_all_ts_data_1GeV_hires_uniq_match_flipped@'log10($2*45/atan(1.0)):($1-3)*(1873.0-900.0)/1873.0+900.0' \
	--math /xrange -2:1 \
	'900*(1-exp(-10**(2*x)*121))+285*(1-exp(-10**(2*x)*1.69))+688*(1-exp(-10**(2*x)*0.27))' \
	'1873.0/(1873.0-900.0)*(285*(1-exp(-10**(2*x)*1.69))+688*(1-exp(-10**(2*x)*0.27)))' \
	-x 'Distance to Nearest FF Source [Degrees]' \
	-y 'Cumulative Number' --xrange -2:1  --xlog --yrange 0:1900
