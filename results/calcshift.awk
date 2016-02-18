# calculate the shift from one to the next and output the final position
($1=="#")
($24>0 && $1!="#") {
    ra1=$9*atan2(1,1)/45; dec1=$10*atan2(1,1)/45;
    ra2=$35*atan2(1,1)/45; dec2=$36*atan2(1,1)/45;
    logL1=$23; logL2=$49;
    deldec =dec2-dec1;
    delra=ra2-ra1;
    a= sin(deldec/2) * sin(deldec/2) + cos(dec1) * cos(dec2) * sin(delra/2)*sin(delra/2)
    c = 2 * atan2(sqrt(a),sqrt(1-a));
    c = c * 45/atan2(1,1)
    # if the likelihood decrease or the position moved too much, use the old position
    if (logL2<logL1 || c>0.5) {
	ra=ra1*45/atan2(1,1)
	dec=dec1*45/atan2(1,1)
    } else {
	ra=ra2*45/atan2(1,1)
	dec=dec2*45/atan2(1,1)
    }
    printf("%s %6.4f %12.8f %12.8f\n",$0,c,ra,dec);
}
