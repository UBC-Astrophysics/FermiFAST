ctioga2 --line-style none --marker Circle --marker-scale 0.4 --name sign-comp \
	--xlog --ylog 1GeV_match.dat@'sqrt(-$25*2):$63' \
	1GeV_match.dat@'sqrt(-$25*2):(if $57>0.003 then $63 else 0.0/0.0 end)' \
	1GeV_match.dat@'sqrt(-$25*2):(if $57>0.017 then $63 else 0.0/0.0 end)' \
	--xrange 0.69:2.6 --yrange 0.69:3.1 \
	--xlabel 'FF Significance [sigma-units]' \
	--ylabel '3FGL Significance [sigma-units]' \
	--draw-line 0.6989700043,0.6989700043 3,3 \
	--draw-line 0.6989700043,1.176091259 3,3.4771212547

