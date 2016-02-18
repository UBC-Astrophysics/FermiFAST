ctioga2 --line-style none --marker Circle --marker-scale 0.2 --name sigcorr \
	--xlog --ylog 'unique_peaks.txt@$6**2:-$25' \
	--xlabel '$[S(r^2)]^2$' --ylabel '$-\ln \mathrm{Prob}(\log L)$'
