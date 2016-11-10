ctioga2 --name expmap2 --draw-image expmap_data.png 0,0 1.3,1 \
	--math \
	--legend-inside rt /scale 1.2 \
	--legend-line '$\sigma=0.10, \gamma= 2.90$' \
	-l 'PSF' \
	--line-style dashes -c Red \
	'10' \
	-l 'Region of Interest' \
	--line-style 3,3,1,3 -c Blue \
         '10' \
	-l 'Exposure' \
	--line-style solid -c Green \
	'10' \
	--xrange 0:1.5 --yrange 0:1.1 -x '$\delta p$' -y 'Weight'
