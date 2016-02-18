ctioga2 --line-style no --marker Circle --marker-scale 0.4 --name changelogL --xlog  \
	unique_peaks_maximized_selected.txt@'$23:($49-$23)/$49' \
	unique_peaks_maximized_selected.txt@'$23:(if $54<1 then ($49-$23)/$49 else 0.0/0.0 end)' \
	--yrange -2:1 \
	--xlabel '$\log L_\mathrm{initial}$' --ylabel '$(\log L_\mathrm{final}-\log L_\mathrm{initial})/\log L_\mathrm{initial}$'
