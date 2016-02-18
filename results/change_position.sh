ctioga2 --line-style no --marker Circle --marker-scale 0.4 --name change_position --xlog --ylog \
	unique_peaks_maximized_selected.txt@23:54 \
	unique_peaks_maximized_selected.txt@'$23:(if ($49-$23)>0 then $54 else 0.0/0.0 end)' \
	--yrange -2.9:0.9 \
	--xlabel '$\log L_\mathrm{initial}$' --ylabel '$\Delta \phi$ [degrees]'
