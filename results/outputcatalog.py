import pyfits

hdulist = pyfits.open("gll_psc_v16.fits.gz")
tabledata = hdulist[1].data

Gal_RA  = tabledata.field('RAJ2000')
Gal_DEC = tabledata.field('DEJ2000')
Gal_LAT  = tabledata.field('GLAT')
Gal_LONG = tabledata.field('GLON')
SIGNIF = tabledata.field('Signif_Avg')
SNAME = tabledata.field('Source_Name')
ASSOC1 = tabledata.field('ASSOC1')
for ff in zip(range(1,len(Gal_RA)+1),Gal_RA,Gal_DEC,Gal_LONG,Gal_LAT,SIGNIF,SNAME,ASSOC1):
    print("%6d %15.8e %15.8e %15.8e %15.8e %15.8e %s %s" % ff)
    
