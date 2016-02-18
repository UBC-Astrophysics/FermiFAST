#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
ctioga2 --mark --name cumff \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match@'log10($2*45/atan(1.0)):$1-3' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):$1-3' \
	cum_all_ts_data_1GeV_hires_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):($1-3)*(1568.0-1400.0)/1568.0+1400.0' \
	--math /xrange -2:1 \
	'920*(1-exp(-10**(2*x)*121))+500*(1-exp(-10**(2*x)*8))+148*(1-exp(-10**(2*x)*0.45))' \
	-x 'Distance to Nearest Fermi 2PSC Source [Degrees]' \
	-y 'Cumulative Number' --xrange -2:1  --xlog --yrange 0:1600
