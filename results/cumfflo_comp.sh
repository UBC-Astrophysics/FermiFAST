#	unearf_cum@'log10($2*45/atan(1.0)):$1-3' \
#
# 5   (800-755)/1727.0 = 2.6%
# 4.5 (923-865)/1727.0 = 3.3%
# 4   (1076-995)/1076 = 7.5%
# 3.5 (1312-1180)/1312 = 10.1%
# 3   (1727-1400)/1727 = 18.9%
#
# after debugging match_kd!
#
# 5    (800-775)/1727.0
# 
#
#
ctioga2 --mark --name cumfflo_comp \
	cum_all_ts_data_1GeV_hires_12.5_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_10.125_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_8_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_6.125_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_4.5_uniq_fermi_match@'log10($2*45/atan(1.0)):$1' \
	cum_all_ts_data_1GeV_hires_4.5_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):$1' \
	--line-style Dashes \
	cum_all_ts_data_1GeV_hires_4.5_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):(1-0.795761)*$1+0.795761*1727' \
	cum_all_ts_data_1GeV_hires_6.125_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):(1-0.897635)*$1+0.897635*1312' \
	cum_all_ts_data_1GeV_hires_8_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):(1-0.937972)*$1+0.937972*1076' \
	cum_all_ts_data_1GeV_hires_10.125_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):(1-0.95124)*$1+0.95124*923' \
	cum_all_ts_data_1GeV_hires_12.5_uniq_fermi_match_flipped@'log10($2*45/atan(1.0)):(1-0.957478)*$1+0.957478*800' \
	-x 'Distance to Nearest Fermi 3FGL Source [Degrees]' \
	-y 'Cumulative Number' --xrange -2:1  --xlog --yrange 0:1800
#	--math /xrange -4:1 \
#	'1400*10**(2*(x+0.895))' \
